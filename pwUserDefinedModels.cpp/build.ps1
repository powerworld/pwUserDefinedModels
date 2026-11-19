#Requires -Version 7
<#
.SYNOPSIS
    Builds the C++ models (CMakeLists.txt) with CMake, Ninja and clang.

.DESCRIPTION
    Runs the preset named after the mode (Debug or Release) and builds the pwUDM.<Model> targets.
    The DLLs land in build\Win64\<Mode>\bin.
    clang++, cmake and ninja are taken from PATH. If one is missing, the folders under C:\tools
    (llvm-mingw\bin, cmake\bin, ninja) are tried; set PWUDM_TOOLS to use another root folder.

.PARAMETER Mode
    Debug or Release (default Release).

.PARAMETER Model
    One or more of CLOD, GENCLS, IEEEST, IEEET1, MTDCCONV, MTDCLINE, TGOV1 (default: all seven).
    Separate several models with commas or spaces.

.EXAMPLE
    pwsh .\build.ps1

.EXAMPLE
    pwsh .\build.ps1 -Mode Debug -Model TGOV1,CLOD
#>
[CmdletBinding()]
param(
    [Parameter(Position = 0)]
    [ValidateSet('Debug', 'Release')]
    [string] $Mode = 'Release',

    [Parameter(Position = 1, ValueFromRemainingArguments)]
    [ArgumentCompleter({ param($command, $parameter, $word) 'CLOD', 'GENCLS', 'IEEEST', 'IEEET1', 'MTDCCONV', 'MTDCLINE', 'TGOV1' | Where-Object { $_ -like "$word*" } })]
    [string[]] $Model = @('CLOD', 'GENCLS', 'IEEEST', 'IEEET1', 'MTDCCONV', 'MTDCLINE', 'TGOV1')
)

$ErrorActionPreference = 'Stop'

function Fail([string] $Message, [int] $Code = 1) {
    Write-Host "build.ps1: $Message" -ForegroundColor Red
    exit $Code
}

# "pwsh -File build.ps1 -Model TGOV1,CLOD" passes one string, so split a comma list here
$allModels = 'CLOD', 'GENCLS', 'IEEEST', 'IEEET1', 'MTDCCONV', 'MTDCLINE', 'TGOV1'
$Model = @($Model -split ',' | ForEach-Object { $_.Trim() } | Where-Object { $_ })
foreach ($name in $Model) {
    if ($name -notin $allModels) {
        Fail "Unknown model '$name'. Use one or more of: $($allModels -join ', ')."
    }
}

# The toolchain: use what is on PATH, and add the usual folders for whatever is not
$toolsRoot = if ($env:PWUDM_TOOLS) { $env:PWUDM_TOOLS } else { 'C:\tools' }
$toolFolders = @{
    'clang++' = Join-Path $toolsRoot 'llvm-mingw\bin'
    'cmake'   = Join-Path $toolsRoot 'cmake\bin'
    'ninja'   = Join-Path $toolsRoot 'ninja'
}
foreach ($tool in $toolFolders.Keys) {
    if (-not (Get-Command $tool -ErrorAction SilentlyContinue) -and (Test-Path -LiteralPath $toolFolders[$tool])) {
        $env:PATH = "$($toolFolders[$tool]);$env:PATH"
    }
    if (-not (Get-Command $tool -ErrorAction SilentlyContinue)) {
        Fail "'$tool' was not found. Put it on PATH, or set PWUDM_TOOLS to a folder that holds llvm-mingw\bin, cmake\bin and ninja."
    }
}

# Presets are resolved from the source folder
Push-Location -LiteralPath $PSScriptRoot
try {
    Write-Host "=== configure $Mode"
    & cmake --preset $Mode
    if ($LASTEXITCODE -ne 0) {
        Fail "CMake configure failed for $Mode (exit code $LASTEXITCODE)." $LASTEXITCODE
    }

    $targets = @($Model | ForEach-Object { "pwUDM.$_" })   # an array even for one model, or the splat below splits the string
    Write-Host "=== build $Mode : $($targets -join ' ')"
    & cmake --build --preset $Mode --target @targets
    if ($LASTEXITCODE -ne 0) {
        Fail "Build failed for $Mode (exit code $LASTEXITCODE)." $LASTEXITCODE
    }
}
finally {
    Pop-Location
}

Write-Host ''
Write-Host "DLLs are in $(Join-Path $PSScriptRoot "build\Win64\$Mode\bin")"

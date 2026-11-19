#Requires -Version 7
<#
.SYNOPSIS
    Builds the Delphi models (pwUDM.dproj) with Delphi and MSBuild.

.DESCRIPTION
    Each model is a build configuration of the one project, named pwUDM.<Model>_<Mode>, for example pwUDM.TGOV1_Release.
    The DLLs land in build\Win64\<Mode>.
    The RAD Studio folder is the BDS environment variable if it is set, otherwise the newest installation
    registered on the machine. The project was made with Delphi 10.2 Tokyo (RAD Studio 19.0).

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

# Which Delphi: the BDS environment variable if it is set, otherwise the newest RAD Studio registered on this machine
$bds = $env:BDS
if (-not $bds) {
    $installs = foreach ($key in 'HKCU:\Software\Embarcadero\BDS', 'HKLM:\SOFTWARE\WOW6432Node\Embarcadero\BDS', 'HKLM:\SOFTWARE\Embarcadero\BDS') {
        if (Test-Path -LiteralPath $key) {
            foreach ($item in Get-ChildItem -LiteralPath $key) {
                $rootDir = (Get-ItemProperty -LiteralPath $item.PSPath).RootDir
                $version = $null
                if ($rootDir -and [version]::TryParse($item.PSChildName, [ref] $version) -and
                    (Test-Path -LiteralPath (Join-Path $rootDir 'bin\rsvars.bat'))) {
                    [pscustomobject]@{ Version = $version; RootDir = $rootDir }
                }
            }
        }
    }
    $newest = $installs | Sort-Object Version -Descending | Select-Object -First 1
    if (-not $newest) {
        Fail 'No RAD Studio installation was found. Install Delphi, or set the BDS environment variable to your RAD Studio folder.'
    }
    $bds = $newest.RootDir
}
$bds = $bds.TrimEnd('\')
$rsvars = Join-Path $bds 'bin\rsvars.bat'
if (-not (Test-Path -LiteralPath $rsvars)) {
    Fail "Delphi was not found at '$bds' (no bin\rsvars.bat). Set the BDS environment variable to your RAD Studio folder."
}
Write-Host "Delphi: $bds"

# Take over the environment that Delphi's rsvars.bat sets (BDS, BDSCOMMONDIR, PATH, ...)
foreach ($line in Get-Content -LiteralPath $rsvars) {
    if ($line -match '^@?SET\s+([^=\s]+)=(.*)$') {
        [Environment]::SetEnvironmentVariable($Matches[1], [Environment]::ExpandEnvironmentVariables($Matches[2]))
    }
}

# rsvars.bat points at the old .NET 3.5 MSBuild; the project file needs 4.0
$msbuild = Join-Path $env:SystemRoot 'Microsoft.NET\Framework\v4.0.30319\MSBuild.exe'
$project = Join-Path $PSScriptRoot 'pwUDM.dproj'

foreach ($name in $Model) {
    $config = "pwUDM.${name}_$Mode"
    Write-Host "=== $config"
    & $msbuild $project -nologo -v:minimal -t:Build "-p:Config=$config" -p:Platform=Win64
    if ($LASTEXITCODE -ne 0) {
        Fail "MSBuild failed for $config (exit code $LASTEXITCODE)." $LASTEXITCODE
    }
}

Write-Host ''
Write-Host "DLLs are in $(Join-Path $PSScriptRoot "build\Win64\$Mode")"

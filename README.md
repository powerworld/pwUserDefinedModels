# pwUserDefinedModels
Solutions for creating a DLL that can be recognized by the PowerWorld Transient Stability add-on.

## C++ (`pwUserDefinedModels.cpp`)

One CMake project builds every model as its own DLL (`pwUDM.<MODEL>.cpp.dll`) with Ninja and clang. The code uses C++23 modules: no header files, every `.cppm` holds exported declarations, its `.cpp` holds the definitions, and the standard library comes from `import std;`.

```
pwUserDefinedModels.cpp/
  CMakeLists.txt, CMakePresets.json
  src/Core/           shared library: the PowerWorld DLL contract, helpers, per-model-class signals
  src/<NAME>/         one DLL per model: CLOD, GENCLS, IEEEST, IEEET1, TGOV1
```

Requirements: clang 23+ with libc++ (the [llvm-mingw](https://github.com/mstorsjo/llvm-mingw) UCRT x86_64 build works and needs no Visual Studio), CMake 4.2+, and Ninja, all on `PATH`. The project is Windows-only because the DLL contract uses `__declspec` and `__stdcall`.

```
cd pwUserDefinedModels.cpp
pwsh .\build.ps1                              # all models, Release (add -Mode Debug for Debug)
pwsh .\build.ps1 -Mode Release -Model TGOV1   # one model
```

`build.ps1` (PowerShell 7) runs the preset and builds the targets; it finds the toolchain on `PATH`, or under `C:\tools` (set `PWUDM_TOOLS` for another root). The same thing by hand:

```
cmake --preset Debug          # or: Release
cmake --build --preset Debug
```

Everything for a mode lands in `build/Win64/<Debug|Release>`, and the DLLs in its `bin` folder. Build one model with `cmake --build --preset Debug --target pwUDM.TGOV1`. Warnings are errors (`-Wall -Wextra -Wpedantic -Werror`), with `-fno-exceptions -fno-rtti`. Keep the repository and `build/` on local storage and exclude them (and the LLVM folder) from antivirus real-time scanning; module builds create many small files.

To add a model, create `src/<NAME>/pwUDM.<NAME>.cppm` and `.cpp` and add a target block for it to `CMakeLists.txt` (copy an existing model's block; each one lists its own sources and DLL name). After renaming or deleting a `.cppm`, clear stale module artifacts with `ninja -C build/Win64/Debug -t cleandead` instead of deleting `build/`.

## Delphi (`pwUserDefinedModels.pas`)

One Delphi project (`pwUDM.dproj`) builds every model as its own DLL (`pwUDM.<MODEL>.pas.dll`), laid out like the C++ folder. Each model is one unit: the `interface` holds its constants and the exported routine declarations, the `implementation` holds the code and the `exports` clause. A Delphi project compiles one binary, so each model is a build configuration: `pwUDM.TGOV1_Release` defines `MODEL_TGOV1`, which selects the unit that `pwUDM.dpr` compiles in and names the DLL.

```
pwUserDefinedModels.pas/
  pwUDM.dpr, pwUDM.dproj, build.ps1
  src/Core/           shared units: the PowerWorld DLL contract types, helpers, per-model-class signals
  src/<NAME>/         one unit per model: CLOD, GENCLS, IEEEST, IEEET1, TGOV1
```

Requirements: Delphi (the project was made with 10.2 Tokyo, RAD Studio 19.0) and PowerShell 7 (`pwsh`) for the build script; the project targets 64-bit Windows. The script uses the RAD Studio folder in the `BDS` environment variable if it is set, otherwise the newest installation registered on the machine, and prints which one it picked. Set `BDS` to build with a specific version, for example `$env:BDS = 'C:\Program Files (x86)\Embarcadero\Studio\19.0'` for 10.2.

```
cd pwUserDefinedModels.pas
pwsh .\build.ps1                              # all models, Release (add -Mode Debug for Debug)
pwsh .\build.ps1 -Mode Release -Model TGOV1   # one model
```

The settings every configuration shares live in option sets under `options\`, and the configurations inherit exactly like the C++ presets, with the same names: `Base` &rarr; `Win64` &rarr; `Debug` or `Release` &rarr; `pwUDM.<NAME>_<Mode>` (the `pwUDM.<NAME>` targets in `CMakeLists.txt`). Each of the first four has its own option set, so `options\` is the counterpart of `CMakePresets.json`; `pwUDM.dproj` keeps only the per-model parts, like `CMakeLists.txt`. Everything for a mode lands in `build\Win64\<Debug|Release>`: the DLLs, with the compiled units in its `dcu` folder. In the IDE, open `pwUDM.dproj`, choose a configuration such as `pwUDM.TGOV1_Release` and build. To add a model, create `src/<NAME>/pwUDM.<NAME>.pas`, add a `MODEL_<NAME>` branch to `pwUDM.dpr` (library suffix and `uses`), and add `pwUDM.<NAME>_Release` and `pwUDM.<NAME>_Debug` build configurations that define `MODEL_<NAME>`.

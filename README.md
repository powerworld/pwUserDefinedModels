# pwUserDefinedModels
Solutions for creating a DLL that can be recognized by the PowerWorld Transient Stability add-on.

## Models

One example model for each of the seven user-defined model classes, written in both languages.

| Model | Class | What it is |
| :--- | :--- | :--- |
| TGOV1 | governor | the existing TGOV1 model |
| IEEET1 | exciter | the existing IEEET1 model |
| IEEEST | stabilizer | the existing IEEEST model |
| GENCLS | machine | the existing GENCLS model |
| CLOD | load | the existing CLOD model |
| MTDCCONV | MTDC converter | a generic converter: three sensor lags (DC current, DC voltage, AC voltage) and an integral current regulator that sets the cosine control angle |
| MTDCLINE | multi-terminal DC | a generic multi-terminal line: shares the pole current order between up to 8 converters (the number of terminals is a parameter, because Simulator does not tell the DLL) |

## Naming

The same names are used in every model and in both languages, so a reader can move between them.

| What | Name | Example |
| :--- | :--- | :--- |
| Model sizes | `N_F_PARAMS`, `N_I_PARAMS`, `N_S_PARAMS`, `N_STATES`, `N_ALGEBRAICS`, `N_NONWINDUPLIMITS` (in this order) | `N_STATES = 2` |
| Float parameter index | `PARAM_<Name>` | `PARAM_Tord` |
| Integer parameter index (own index space, from 0) | `IPARAM_<Name>` | `IPARAM_NumTerminals` |
| State index | `STATE_<Name>` | `STATE_ValvePos` |
| Algebraic index | `ALG_<Name>` | `ALG_IdRefInit` |
| Position in the non-windup list | `NONWINDUPINDEX_<StateName>` | `NONWINDUPINDEX_VR` |
| Hard-coded signal index | `HARDCODE_<CLASS>_<Name>` | `HARDCODE_GOV_Pref` |
| Pointer to an array in the shared record | `pFloatParamArr`, `pIntParamArr`, `pStatesArr`, `pIgnoreStatesArr`, `pAlgebraicsArr`, `pHardCodedArr`, `pDotXArr`, `pByteActiveLimitArr` | same name in every routine |
| Non-windup record field | `LimitStates`, `minLimits`, `maxLimits`, `activeLimits` | `nonWindUpLimits.LimitStates` |
| Local copy of a model parameter | `f<Name>`, inverse time constant `fInv<Name>` | `fInvTord` |
| Local copy of a state | `state<Name>` | `stateValve` |
| Derivative of a state | `dotState<Name>` (not `dotX`, which is the output array) | `dotStateValve` |
| Network or electrical local | `local_<Name>` | `local_V` |
| Nested helper procedure (lambda in C++) | `LOCAL_<Name>` | `LOCAL_NumberOfTerminals` |
| Argument of a nested helper | `_<Name>` | `_fPInitial` |
| Value read from Simulator (a hard-coded signal or a system option) | `<Name>_In` | `GenObjectMVABase_In` |
| Value written back to Simulator | `<Name>_Out` | `localUsedP_Out` |
| Scratch value | `temp<Name>` | `tempPrevState` |
| Any other local | lowerCamel | `cosDelta`, `maxOrder` |
| Helper that is private to a model (anonymous namespace in C++) | `LOCAL_<Name>` | `LOCAL_HardCodedIndex` |
| Constant that is private to a model | `MAX_<Name>` for a capacity limit, `GLOBAL_<Name>` for a model constant | `MAX_TERMINALS`, `GLOBAL_DL_PCoeff` |
| File-level variable | none: every bit of state lives in the record Simulator passes in | |

CLOD keeps the names of the original model it was ported from (a state is `<Name>State`, for example `SpeedState`, and its derivative is `dot<Name>State`; its other locals such as `IML_fPInitial` and `temp_P` are kept too).

## C++ (`pwUserDefinedModels.cpp`)

One CMake project builds every model as its own DLL (`pwUDM.<MODEL>.cpp.dll`) with Ninja and clang. The code uses C++23 modules: no header files, every `.cppm` holds exported declarations, its `.cpp` holds the definitions, and the standard library comes from `import std;`.

```
pwUserDefinedModels.cpp/
  CMakeLists.txt, CMakePresets.json
  src/Core/           shared library: the PowerWorld DLL contract, helpers, per-model-class signals
  src/<NAME>/         one DLL per model: CLOD, GENCLS, IEEEST, IEEET1, MTDCCONV, MTDCLINE, TGOV1
```

Requirements: clang 23+ with libc++ (the [llvm-mingw](https://github.com/mstorsjo/llvm-mingw) UCRT x86_64 build works and needs no Visual Studio), CMake 4.2+, and Ninja, all on `PATH`. The project is Windows-only because the DLL contract uses `__declspec(dllexport)`.

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
  src/<NAME>/         one unit per model: CLOD, GENCLS, IEEEST, IEEET1, MTDCCONV, MTDCLINE, TGOV1
```

Requirements: Delphi (the project was made with 10.2 Tokyo, RAD Studio 19.0) and PowerShell 7 (`pwsh`) for the build script; the project targets 64-bit Windows. The script uses the RAD Studio folder in the `BDS` environment variable if it is set, otherwise the newest installation registered on the machine, and prints which one it picked. Set `BDS` to build with a specific version, for example `$env:BDS = 'C:\Program Files (x86)\Embarcadero\Studio\19.0'` for 10.2.

```
cd pwUserDefinedModels.pas
pwsh .\build.ps1                              # all models, Release (add -Mode Debug for Debug)
pwsh .\build.ps1 -Mode Release -Model TGOV1   # one model
```

The settings every configuration shares live in option sets under `options\`, and the configurations inherit exactly like the C++ presets, with the same names: `Base` &rarr; `Win64` &rarr; `Debug` or `Release` &rarr; `pwUDM.<NAME>_<Mode>` (the `pwUDM.<NAME>` targets in `CMakeLists.txt`). Each of the first four has its own option set, so `options\` is the counterpart of `CMakePresets.json`; `pwUDM.dproj` keeps only the per-model parts, like `CMakeLists.txt`. Everything for a mode lands in `build\Win64\<Debug|Release>`: the DLLs, with the compiled units in its `dcu` folder. In the IDE, open `pwUDM.dproj`, choose a configuration such as `pwUDM.TGOV1_Release` and build. To add a model, create `src/<NAME>/pwUDM.<NAME>.pas`, add a `MODEL_<NAME>` branch to `pwUDM.dpr` (library suffix and `uses`), and add `pwUDM.<NAME>_Release` and `pwUDM.<NAME>_Debug` build configurations that define `MODEL_<NAME>`.

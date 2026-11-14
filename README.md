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
cmake --preset windows-debug          # or: windows-release
cmake --build --preset windows-debug
```

DLLs land in `build/<preset>/bin`. Build one model with `cmake --build --preset windows-debug --target pwUDM.TGOV1`. Warnings are errors (`-Wall -Wextra -Wpedantic -Werror`), with `-fno-exceptions -fno-rtti`. Keep the repository and `build/` on local storage and exclude them (and the LLVM folder) from antivirus real-time scanning; module builds create many small files.

To add a model, create `src/<NAME>/pwUDM.<NAME>.cppm` and `.cpp` and add `pwudm_add_model(<NAME>)` to `CMakeLists.txt`. After renaming or deleting a `.cppm`, clear stale module artifacts with `ninja -C build/windows-debug -t cleandead` instead of deleting `build/`.

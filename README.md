# pwUserDefinedModels
Solutions for creating a DLL that can be recognized by the PowerWorld Transient Stability add-on.

## C++ (`pwUserDefinedModels.cpp`)

One CMake project builds every model as its own DLL (`pwUDM.<MODEL>.cpp.dll`) with Ninja and clang. The code follows the Orthodox C++23 + pure C++20 modules rules: no header files, every `.cppm` (exported declarations only) is paired with a `.cpp` (definitions), and the standard library comes from `import std;`.

```
pwUserDefinedModels.cpp/
  CMakeLists.txt, CMakePresets.json
  src/Core/           shared library: the PowerWorld DLL contract, helpers, per-model-class signals
  src/<NAME>/         one DLL per model: CLOD, GENCLS, IEEEST, IEEET1, TGOV1
  tools/              check_rules (rule gate) and new_module (scaffolding)
```

Requirements: clang 19+ with libc++ (the [llvm-mingw](https://github.com/mstorsjo/llvm-mingw) UCRT x86_64 build works and needs no Visual Studio), CMake 4.2+, and Ninja, all on `PATH`.

```
cd pwUserDefinedModels.cpp
cmake --preset windows-debug          # or: windows-release
cmake --build --preset windows-debug
```

DLLs land in `build/<preset>/bin`. Keep the repository and `build/` on local storage and exclude them (and the LLVM folder) from antivirus real-time scanning; module builds create many small files. A normal build first runs `check_rules`, which fails the build on any rule violation (header files, function bodies in a `.cppm`, templates, exceptions, RTTI casts, macros, `export import`, a `.cppm` that declares functions but has no `.cpp`, or an empty `.cpp`). Link jobs are throttled to 2 at a time. The presets are Windows-only because the DLL contract uses `__declspec` and `__stdcall`. Compiler flags include `-Wall -Wextra -Wpedantic -Werror -fno-exceptions -fno-rtti`. Build one model with `cmake --build --preset windows-debug --target pwUDM.TGOV1`.

Add a module or model with the scaffolding tool (run from `pwUserDefinedModels.cpp`; it creates the `.cppm`/`.cpp` pair and registers it in `CMakeLists.txt`):

```
build/windows-debug/new_module --model MYMODEL             # new model DLL pwUDM.MYMODEL.cpp.dll
build/windows-debug/new_module Core pwUDM.Foo               # new public module in a sub-library
build/windows-debug/new_module Core pwUDM.Foo --private
build/windows-debug/new_module Core pwUDM.Bar --declarations-only   # .cppm alone, no empty .cpp
```

A `.cppm` that only holds types, constants and `extern "C"` contract declarations needs no `.cpp`; `check_rules` rejects an empty one.

After renaming or deleting a `.cppm`, clear stale module artifacts with `ninja -C build/windows-debug -t cleandead` instead of deleting `build/`.

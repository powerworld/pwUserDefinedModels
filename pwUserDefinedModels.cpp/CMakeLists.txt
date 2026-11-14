cmake_minimum_required(VERSION 4.2)

project(pwUserDefinedModels LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# The PowerWorld DLL contract uses __declspec(dllexport) and __stdcall, so this project is Windows-only.
if(NOT WIN32)
    message(FATAL_ERROR "pwUserDefinedModels builds Windows DLLs. Use: cmake --preset windows-debug")
endif()

if(NOT CMAKE_GENERATOR MATCHES "Ninja")
    message(FATAL_ERROR "This project requires the Ninja generator for C++20 module dependency scanning. Use: cmake --preset windows-debug")
endif()

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(FATAL_ERROR "This project requires clang (the compile and link options are clang/lld specific). Use: cmake --preset windows-debug")
endif()

# 23 is the version this project is built and tested with (import std; with libc++).
if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 23)
    message(FATAL_ERROR "clang 23 or newer is required (found ${CMAKE_CXX_COMPILER_VERSION}).")
endif()

# DLLs go to bin/, import libraries and the Core archive go to lib/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Throttle concurrent link jobs in Ninja to prevent RAM saturation when building multiple DLLs
set_property(GLOBAL PROPERTY JOB_POOLS link_pool=2)
set(CMAKE_JOB_POOL_LINK link_pool)

# Compile and link options shared by every target (clang 23+ with lld)
add_library(pwudm_options INTERFACE)

target_compile_options(pwudm_options INTERFACE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    # The DLL contract fixes every export's parameter list, and most models ignore some of them.
    -Wno-unused-parameter
    -fno-exceptions
    -fno-rtti
    # CMake already adds -g for Debug; CodeView lets Visual Studio and WinDbg read the symbols
    $<$<CONFIG:Debug>:-gcodeview>
)

# -static makes each DLL carry its own libc++/MinGW runtime, so nothing has to be deployed next to it.
target_link_options(pwudm_options INTERFACE
    -fuse-ld=lld
    -static
    # Debug: write pwUDM.<NAME>.cpp.pdb next to each DLL
    $<$<CONFIG:Debug>:LINKER:--pdb=>
)

# ==============================================================================
# Core: the PowerWorld DLL contract, shared helpers and the per-model-class
#       signal indices and exports (imports std only)
# ==============================================================================
add_library(Core STATIC)
target_link_libraries(Core PRIVATE pwudm_options)
target_sources(Core
    PRIVATE
        src/Core/pwUDM.cpp
    PUBLIC
        FILE_SET CXX_MODULES FILES
        src/Core/pwUDM.cppm
        src/Core/pwUDM.Exciter.cppm
        src/Core/pwUDM.Governor.cppm
        src/Core/pwUDM.Load.cppm
        src/Core/pwUDM.Machine.cppm
        src/Core/pwUDM.Stabilizer.cppm
)

# ==============================================================================
# Models: one DLL per model, pwUDM.<NAME>.cpp.dll, built from
#         src/<NAME>/pwUDM.<NAME>.{cppm,cpp}. Models never link each other.
# ==============================================================================
function(pwudm_add_model name)
    add_library(pwUDM.${name} SHARED)
    target_link_libraries(pwUDM.${name} PRIVATE pwudm_options Core)
    target_sources(pwUDM.${name}
        PRIVATE
            src/${name}/pwUDM.${name}.cpp
        PRIVATE
            FILE_SET CXX_MODULES FILES
            src/${name}/pwUDM.${name}.cppm
    )
    set_target_properties(pwUDM.${name} PROPERTIES
        PREFIX ""                          # not libpwUDM.*
        OUTPUT_NAME "pwUDM.${name}.cpp"
        SUFFIX ".dll"
    )
endfunction()

pwudm_add_model(CLOD)
pwudm_add_model(GENCLS)
pwudm_add_model(IEEEST)
pwudm_add_model(IEEET1)
pwudm_add_model(TGOV1)

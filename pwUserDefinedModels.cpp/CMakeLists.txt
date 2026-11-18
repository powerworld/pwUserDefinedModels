cmake_minimum_required(VERSION 4.2)

project(pwUserDefinedModels LANGUAGES CXX)

# Presets always choose a build type; this only covers configuring by hand without one.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# The PowerWorld DLL contract uses __declspec(dllexport), so this project is Windows-only.
if(NOT WIN32)
    message(FATAL_ERROR "pwUserDefinedModels builds Windows DLLs. Use: cmake --preset Debug")
endif()

if(NOT CMAKE_GENERATOR MATCHES "Ninja")
    message(FATAL_ERROR "This project requires the Ninja generator for C++20 module dependency scanning. Use: cmake --preset Debug")
endif()

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(FATAL_ERROR "This project requires clang (the compile and link options are clang/lld specific). Use: cmake --preset Debug")
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
        src/Core/pwUDM.Core.cpp
    PUBLIC
        FILE_SET CXX_MODULES FILES
        src/Core/pwUDM.Core.cppm
        src/Core/pwUDM.Exciter.cppm
        src/Core/pwUDM.Governor.cppm
        src/Core/pwUDM.Load.cppm
        src/Core/pwUDM.Machine.cppm
        src/Core/pwUDM.Stabilizer.cppm
)

# ==============================================================================
# Models: one DLL per model, pwUDM.<NAME>.cpp.dll. Each target spells out its own
#         source files and DLL name; models never link each other.
# ==============================================================================
# The settings every model shares
function(pwudm_model_settings target outputName)
    target_link_libraries(${target} PRIVATE pwudm_options Core)
    set_target_properties(${target} PROPERTIES
        PREFIX ""                          # not libpwUDM.*
        OUTPUT_NAME "${outputName}"
        SUFFIX ".dll"
    )
endfunction()

add_library(pwUDM.CLOD SHARED)
target_sources(pwUDM.CLOD
    PRIVATE
        src/CLOD/pwUDM.CLOD.cpp
    PRIVATE
        FILE_SET CXX_MODULES FILES
        src/CLOD/pwUDM.CLOD.cppm
)
pwudm_model_settings(pwUDM.CLOD pwUDM.CLOD.cpp)

add_library(pwUDM.GENCLS SHARED)
target_sources(pwUDM.GENCLS
    PRIVATE
        src/GENCLS/pwUDM.GENCLS.cpp
    PRIVATE
        FILE_SET CXX_MODULES FILES
        src/GENCLS/pwUDM.GENCLS.cppm
)
pwudm_model_settings(pwUDM.GENCLS pwUDM.GENCLS.cpp)

add_library(pwUDM.IEEEST SHARED)
target_sources(pwUDM.IEEEST
    PRIVATE
        src/IEEEST/pwUDM.IEEEST.cpp
    PRIVATE
        FILE_SET CXX_MODULES FILES
        src/IEEEST/pwUDM.IEEEST.cppm
)
pwudm_model_settings(pwUDM.IEEEST pwUDM.IEEEST.cpp)

add_library(pwUDM.IEEET1 SHARED)
target_sources(pwUDM.IEEET1
    PRIVATE
        src/IEEET1/pwUDM.IEEET1.cpp
    PRIVATE
        FILE_SET CXX_MODULES FILES
        src/IEEET1/pwUDM.IEEET1.cppm
)
pwudm_model_settings(pwUDM.IEEET1 pwUDM.IEEET1.cpp)

add_library(pwUDM.TGOV1 SHARED)
target_sources(pwUDM.TGOV1
    PRIVATE
        src/TGOV1/pwUDM.TGOV1.cpp
    PRIVATE
        FILE_SET CXX_MODULES FILES
        src/TGOV1/pwUDM.TGOV1.cppm
)
pwudm_model_settings(pwUDM.TGOV1 pwUDM.TGOV1.cpp)

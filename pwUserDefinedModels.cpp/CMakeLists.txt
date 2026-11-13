cmake_minimum_required(VERSION 3.30)

# Enable experimental import std; support for CMake 3.30+ (adjust UUID if using newer CMake)
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")

project(pwUserDefinedModels LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_GENERATOR MATCHES "Ninja")
    message(FATAL_ERROR "This project requires the Ninja generator for C++20 module dependency scanning. Use: cmake --preset <os>-debug")
endif()

# Throttle concurrent link jobs in Ninja to prevent RAM saturation when building multiple binaries
set_property(GLOBAL PROPERTY JOB_POOLS link_pool=2)
set(CMAKE_JOB_POOL_LINK link_pool)

# Global deterministic compiler and linker gates (Cross-Platform Clang 19+ Standard)
add_library(ProjectCompileGates INTERFACE)

if(MSVC)
    target_compile_options(ProjectCompileGates INTERFACE
        /W4
        /WX
        /EHs-c-
        /GR-
        /permissive-
        $<$<CONFIG:Debug>:/Z7>
    )
    target_link_options(ProjectCompileGates INTERFACE
        $<$<CONFIG:Debug>:/INCREMENTAL>
    )
else()
    target_compile_options(ProjectCompileGates INTERFACE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        # Deviation: the DLL contract fixes every export's parameter list, and most models ignore some of them.
        -Wno-unused-parameter
        -fno-exceptions
        -fno-rtti
        $<$<CONFIG:Debug>:-g>
    )

    # Standardize on libc++ across Linux and macOS for uniform Clang import std; support
    if(UNIX AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(ProjectCompileGates INTERFACE -stdlib=libc++)
        target_link_options(ProjectCompileGates INTERFACE -stdlib=libc++)
    endif()

    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_compile_options(ProjectCompileGates INTERFACE $<$<CONFIG:Debug>:-gsplit-dwarf>)
        find_program(MOLD_LINKER mold)
        if(MOLD_LINKER)
            target_link_options(ProjectCompileGates INTERFACE -fuse-ld=mold)
        else()
            target_link_options(ProjectCompileGates INTERFACE -fuse-ld=lld)
        endif()
    elseif(APPLE)
        target_link_options(ProjectCompileGates INTERFACE -fuse-ld=lld)
    elseif(WIN32)
        target_compile_options(ProjectCompileGates INTERFACE $<$<CONFIG:Debug>:-gcodeview>)
        # Deviation: -static makes the DLLs and tools carry their own libc++/MinGW runtime,
        # so nothing has to be deployed next to the DLLs.
        target_link_options(ProjectCompileGates INTERFACE -fuse-ld=lld -static)
    endif()
endif()

# ==============================================================================
# Native C++23 Build & Rule Enforcement Tools (100% Python-free repository)
# ==============================================================================
add_executable(check_rules tools/check_rules.cpp)
target_link_libraries(check_rules PRIVATE ProjectCompileGates)

add_executable(new_module tools/new_module.cpp)
target_link_libraries(new_module PRIVATE ProjectCompileGates)

# Automatically run check_rules as a mandatory build gate before compiling targets
add_custom_target(VerifyOrthodoxRules ALL
    COMMAND $<TARGET_FILE:check_rules>
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS check_rules
    COMMENT "Enforcing Orthodox C++20 Module Rules..."
)

# Helper function to declare a strict sub-library
function(add_orthodox_sublibrary target_name)
    add_library(${target_name} STATIC)
    target_link_libraries(${target_name} PRIVATE ProjectCompileGates)
    add_dependencies(${target_name} VerifyOrthodoxRules)
endfunction()

# A file set name must be unique per scope, so internal (PRIVATE) modules use the
# 'private_modules' set while public modules use the default CXX_MODULES set.

# ==============================================================================
# Layer 0/1: Core (the PowerWorld DLL contract, shared helpers and the per-model-class
#            signal indices and exports; imports std only)
# ==============================================================================
add_orthodox_sublibrary(Core)
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
    PRIVATE
        FILE_SET private_modules TYPE CXX_MODULES FILES
)

# ==============================================================================
# Layer 3: Applications. Instead of one App executable, each model is its own DLL
#          (pwUDM.<NAME>.cpp.dll) built from src/<NAME>/pwUDM.<NAME>.{cppm,cpp}.
#          Models never link each other.
# ==============================================================================
function(pwudm_add_model name)
    set(model_dir src/${name})

    add_library(pwUDM.${name} SHARED)
    target_link_libraries(pwUDM.${name} PRIVATE ProjectCompileGates Core)
    add_dependencies(pwUDM.${name} VerifyOrthodoxRules)
    target_sources(pwUDM.${name}
        PRIVATE
            ${model_dir}/pwUDM.${name}.cpp
        PRIVATE
            FILE_SET CXX_MODULES FILES
            ${model_dir}/pwUDM.${name}.cppm
    )

    set_target_properties(pwUDM.${name} PROPERTIES
        PREFIX ""                          # not libpwUDM.*
        OUTPUT_NAME "pwUDM.${name}.cpp"    # same file names as the old MSVC projects
        SUFFIX ".dll"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    )
endfunction()

# [new_module:models]
pwudm_add_model(CLOD)
pwudm_add_model(GENCLS)
pwudm_add_model(IEEEST)
pwudm_add_model(IEEET1)
pwudm_add_model(TGOV1)

# Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT). All rights reserved.
# This software may be modified and distributed under the terms of the
# GNU Lesser General Public License v2.1 or any later version.

cmake_minimum_required(VERSION 3.5)
project(blockfactory LANGUAGES CXX VERSION 1.0)

if(BUILD_DOCS)
    add_subdirectory(doc)
    return()
endif()

# C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add custom functions / macros
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Build type
if(NOT CMAKE_CONFIGURATION_TYPES)
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, recommanded options are: Debug or Release" FORCE)
    endif()
    set(BUILD_TYPES "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${BUILD_TYPES})
endif()

# Libraries type
if(MSVC)
    option(BUILD_SHARED_LIBS "Compile BlockFactory as a shared library" FALSE)
else()
    option(BUILD_SHARED_LIBS "Compile BlockFactory as a shared library" TRUE)
endif()

# Build position independent code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Tweak linker flags in Linux
if(UNIX AND NOT APPLE)
    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        get_filename_component(LINKER_BIN ${CMAKE_LINKER} NAME)
            if(${LINKER_BIN} STREQUAL "ld")
                set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--unresolved-symbols=report-all")
            endif()
    endif()
endif()

# Settings for RPATH
if(NOT MSVC)
    option(ENABLE_RPATH "Enable RPATH installation" TRUE)
    mark_as_advanced(ENABLE_RPATH)
endif()

# Enable RPATH
include(AddInstallRPATHSupport)
add_install_rpath_support(BIN_DIRS ${CMAKE_INSTALL_PREFIX}/bin
                          LIB_DIRS ${CMAKE_INSTALL_PREFIX}/mex ${CMAKE_INSTALL_PREFIX}/lib
                          DEPENDS ENABLE_RPATH
                          USE_LINK_PATH)

option(FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." TRUE)
mark_as_advanced(FORCE_COLORED_OUTPUT)
if(${FORCE_COLORED_OUTPUT})
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
       add_compile_options(-fdiagnostics-color=always)
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
       add_compile_options(-fcolor-diagnostics)
    endif()
endif()

# Handle compiler warnings
include(TargetCompileWarnings)
option(ENABLE_WARNINGS "Enable verbose warnings" TRUE)

if(NOT DEFINED TREAT_WARNINGS_AS_ERRORS)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(TREAT_WARNINGS_AS_ERRORS TRUE)
    else()
        set(TREAT_WARNINGS_AS_ERRORS FALSE)
    endif()
endif()

# Export all symbols in Windows
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Add a postfix to Windows libraries compiled in debug
if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
endif()

# Control where binaries and libraries are placed in the build folder.
# This simplifies tests running in Windows.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

if(${CMAKE_VERSION} VERSION_LESS 3.7)
    find_package(Matlab COMPONENTS
        MX_LIBRARY
        ENG_LIBRARY
        MAIN_PROGRAM)
else()
    find_package(Matlab COMPONENTS
        MX_LIBRARY
        ENG_LIBRARY
        MAIN_PROGRAM
        SIMULINK)
    if(NOT CMAKE_MINIMUM_REQUIRED_VERSION VERSION_LESS 3.7)
      message(AUTHOR_WARNING "CMake minimum required version greater than 3.7. You can remove this.")
    endif()
endif()

if(NOT USES_MATLAB)
    option(USES_MATLAB "Compile also Matlab-related components" ${Matlab_FOUND})
endif()

add_subdirectory(deps)
add_subdirectory(sources)

if(USES_MATLAB)
    add_subdirectory(matlab)
endif()

include(AddUninstallTarget)

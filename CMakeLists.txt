cmake_minimum_required(VERSION 3.21)

project(PassWing VERSION 0.1.0 LANGUAGES C CXX)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not supported. Use a build directory or CMake preset.")
endif()

include(GNUInstallDirs)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(PASSWING_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
option(PASSWING_ENABLE_WEBENGINE "Enable the embedded HTML aircraft view" ON)
option(PASSWING_ENABLE_CLANG_TIDY "Run clang-tidy during compilation" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(PassWingDependencies)
include(PassWingProjectOptions)

add_subdirectory(src)

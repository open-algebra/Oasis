# ##############################################################################
# OASIS: Open Algebra Software for Inferring Solutions
#
# CMakeLists.txt - Top-level
# ##############################################################################

cmake_minimum_required(VERSION 3.18)
project(Oasis)

include(CTest)
include(FetchContent)

option(OASIS_BUILD_IO "Enables building extra modules for Oasis" OFF)
option(OASIS_BUILD_TESTS "Enables building unit tests for Oasis" ON)
option(OASIS_BUILD_WITH_COVERAGE
       "Enables building Oasis with code coverage enabled" OFF)
option(OASIS_BUILD_PARANOID
       "Enables the -Werror flag friends when building Oasis" OFF)

# Adds compiler flags for code coverage. Note that only the Clang compiler is
# currently supported for code coverage.
if(OASIS_BUILD_WITH_COVERAGE)
    add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
    add_link_options(-fprofile-instr-generate)
endif()

# Fetches dependencies and integrates them into the project.
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 10.2.1)

FetchContent_MakeAvailable(Catch2 fmt)

include(cmake/FetchEigen.cmake)
include(cmake/FetchTinyxml2.cmake)

# Processes the CMakeLists.txt for each target.
add_subdirectory(include)
add_subdirectory(src)

if(OASIS_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(OASIS_BUILD_IO)
    add_subdirectory(io)
endif()

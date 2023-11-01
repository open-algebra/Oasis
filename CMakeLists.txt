# ##############################################################################
# OASIS: Open Algebra Software for Inferring Solutions
#
# CMakeLists.txt - Top-level
# ##############################################################################

cmake_minimum_required(VERSION 3.26)
project(Oasis)

include(CTest)
include(FetchContent)

# Fetches dependencies and integrates them into the project.
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 10.0.0)

FetchContent_MakeAvailable(Catch2 fmt)

# Processes the CMakeLists.txt for dependencies that need special handling.
add_subdirectory(cmake/FetchEigen)
add_subdirectory(cmake/FetchTaskflow)

if(MSVC)
    add_compile_options("$<$<CONFIG:DEBUG>:/W3>" "$<$<CONFIG:DEBUG>:/WX>")
else()
    add_compile_options("$<$<CONFIG:DEBUG>:-Wall>" "$<$<CONFIG:DEBUG>:-Wextra>" "$<$<CONFIG:DEBUG>:-Wpedantic>" "$<$<CONFIG:DEBUG>:-Werror>")
endif()

# Processes the CMakeLists.txt for each target.
add_subdirectory(src)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

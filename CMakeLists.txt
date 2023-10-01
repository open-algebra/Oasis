cmake_minimum_required(VERSION 3.26)
project(Oasis)

include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0
)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 10.0.0
)

FetchContent_MakeAvailable(Catch2 fmt)

add_subdirectory(cmake/FetchEigen)
add_subdirectory(cmake/FetchTaskflow)

set(CMAKE_CXX_STANDARD 20)

add_library(Oasis src/Add.cpp src/BinaryExpression.cpp src/Expression.cpp src/Real.cpp
        src/Subtract.cpp
        include/Oasis/Subtract.hpp
        src/Multiply.cpp
        include/Oasis/Multiply.hpp
        src/Divide.cpp
        include/Oasis/Divide.hpp
        src/Variable.cpp
        include/Oasis/Variable.hpp
        src/Exponent.cpp
        include/Oasis/Exponent.hpp)
target_include_directories(Oasis PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Oasis PUBLIC fmt::fmt Eigen3::Eigen Taskflow)

get_target_property(Oasis_SOURCES Oasis SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Oasis_SOURCES})

add_library(Oasis::Oasis ALIAS Oasis)

add_executable(OasisTests tests/AddTests.cpp
        tests/SubtractTests.cpp
        tests/MultiplyTests.cpp
        tests/DivideTests.cpp
        tests/ExponentTests.cpp)
target_link_libraries(OasisTests PRIVATE Oasis::Oasis Catch2::Catch2 Catch2::Catch2WithMain)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(OasisTests)
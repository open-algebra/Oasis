cmake_minimum_required(VERSION 3.26)
project(Oasis)

include(FetchContent)

# Fetches the "Catch2" and "fmt" dependencies and integrates them into Oasis.
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

# Processes the CMakeLists.txt for the other dependencies.
add_subdirectory(cmake/FetchEigen)
add_subdirectory(cmake/FetchTaskflow)

# These variables MUST be modified accordingly whenever a new source (.cpp/.hpp) file is added.
set(Oasis_SOURCES 
        src/Add.cpp src/BinaryExpression.cpp src/Divide.cpp src/Expression.cpp src/Multiply.cpp 
        src/Real.cpp src/Subtract.cpp src/Variable.cpp
)

set(Oasis_HEADERS 
        include/Oasis/Add.hpp include/Oasis/BinaryExpression.hpp include/Oasis/Divide.hpp 
        include/Oasis/Expression.hpp include/Oasis/LeafExpression.hpp include/Oasis/Multiply.hpp 
        include/Oasis/Real.hpp include/Oasis/Subtract.hpp include/Oasis/Variable.hpp
)

set(Oasis_TESTS 
        tests/AddTests.cpp tests/DivideTests.cpp tests/MultiplyTests.cpp tests/SubtractTests.cpp
)

# Adds a library target called "Oasis" to be built from source files.
add_library(Oasis ${Oasis_SOURCES} ${Oasis_HEADERS})
add_library(Oasis::Oasis ALIAS Oasis)

target_compile_features(Oasis PUBLIC cxx_std_20)
target_include_directories(Oasis PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Oasis PUBLIC fmt::fmt Eigen3::Eigen Taskflow)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Oasis_SOURCES})

# Adds an executable target called "OasisTests" to be built from sources files.
add_executable(OasisTests ${Oasis_TESTS})

target_link_libraries(OasisTests PRIVATE Oasis::Oasis Catch2::Catch2WithMain)

# Automatically registers the tests with CTest.
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(OasisTests)
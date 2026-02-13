# ##############################################################################
# OASIS: Open Algebra Software for Inferring Solutions
#
# CMakeLists.txt - OASIS tests
# ##############################################################################

# These variables MUST be modified whenever a new test file is added.
set(Oasis_TESTS
    # cmake-format: sortable
    AddTests.cpp
    BinaryExpressionTests.cpp
    Common.hpp
    DifferentiateTests.cpp
    DivideTests.cpp
    ExponentTests.cpp
    IntegrateTests.cpp
    LinearTests.cpp
    LogTests.cpp
    MagnitudeTests.cpp
    MatrixTests.cpp
    MultiplyTests.cpp
    NegateTests.cpp
    PolynomialTests.cpp
    SubtractTests.cpp
    UnaryExpressionTests.cpp
    DefiniteIntegralTests.cpp)

# Adds an executable target called "OasisTests" to be built from sources files.
add_executable(OasisTests ${Oasis_TESTS})

if(MSVC)
    target_compile_options(OasisTests PRIVATE /W3 /WX)
    target_compile_options(OasisTests PRIVATE /bigobj)
else()
    target_compile_options(OasisTests PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_link_libraries(OasisTests PRIVATE Oasis::Oasis Catch2::Catch2WithMain)

if(OASIS_BUILD_IO)
    target_link_libraries(OasisTests PRIVATE Oasis::IO)
    target_compile_definitions(OasisTests PRIVATE OASIS_IO_ENABLED)
endif()

# Automatically registers the tests with CTest.
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(OasisTests)

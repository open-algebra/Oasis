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
    DivideTests.cpp
    ExponentTests.cpp
    LogTests.cpp
    MultiplyTests.cpp
    PolynomialTests.cpp
    SubtractTests.cpp)

# Adds an executable target called "OasisTests" to be built from sources files.
add_executable(OasisTests ${Oasis_TESTS})

if(MSVC)
    target_compile_options(OasisTests PRIVATE /W3 /WX)
    target_compile_options(OasisTests PRIVATE /bigobj)
else()
    target_compile_options(OasisTests PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_library(Oasis::Oasis ALIAS Oasis)
target_link_libraries(OasisTests PRIVATE Oasis::Oasis Catch2::Catch2WithMain)

# Automatically registers the tests with CTest.
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(OasisTests)

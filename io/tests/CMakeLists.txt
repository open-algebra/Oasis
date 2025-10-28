# These variables MUST be modified whenever a new test file is added.
set(OASIS_IO_TESTS_SOURCES
    # cmake-format: sortable
    InFixTests.cpp MathMLTests.cpp PALMTests.cpp TeXTests.cpp)

# Adds an executable target called "OasisTests" to be built from sources files.
add_executable(OasisIOTests ${OASIS_IO_TESTS_SOURCES})

if(MSVC)
    target_compile_options(OasisIOTests PRIVATE /W3 /WX)
    target_compile_options(OasisIOTests PRIVATE /bigobj)
else()
    target_compile_options(OasisIOTests PRIVATE -Wall -Wextra -Wpedantic
                                                -Werror)
endif()

target_link_libraries(OasisIOTests PRIVATE Oasis::IO Oasis::Oasis
                                           Catch2::Catch2WithMain)

# Automatically registers the tests with CTest.
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(OasisIOTests)

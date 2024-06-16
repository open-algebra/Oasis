# These variables MUST be modified whenever a new test file is added.
set(Oasis_EXTRAS_TESTS # cmake-format: sortable
                       InFixTests.cpp MathMLTests.cpp)

# Adds an executable target called "OasisTests" to be built from sources files.
add_executable(OasisExtrasTests ${Oasis_EXTRAS_TESTS})

if(MSVC)
    target_compile_options(OasisExtrasTests PRIVATE /W3 /WX)
    target_compile_options(OasisExtrasTests PRIVATE /bigobj)
else()
    target_compile_options(OasisExtrasTests PRIVATE -Wall -Wextra -Wpedantic
                                                    -Werror)
endif()

target_link_libraries(OasisExtrasTests PRIVATE Oasis::Extras Oasis::Oasis
                                               Catch2::Catch2WithMain)

# Automatically registers the tests with CTest.
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(OasisExtrasTests)

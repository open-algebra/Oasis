# ##############################################################################
# OASIS: Open Algebra Software for Inferring Solutions
#
# CMakeLists.txt - OASIS library
# ##############################################################################

# These variables MUST be modified whenever a new source file is added.
set(Oasis_SOURCES
    # cmake-format: sortable
    Add.cpp
    # DefiniteIntegral.cpp
    Derivative.cpp
    Divide.cpp
    EulerNumber.cpp
    Exponent.cpp
    Expression.cpp
    Imaginary.cpp
    Integral.cpp
    Linear.cpp
    Log.cpp
    Magnitude.cpp
    Matrix.cpp
    Multiply.cpp
    Negate.cpp
    Pi.cpp
    Real.cpp
    SimplifyVisitor.cpp
    Sine.cpp
    Subtract.cpp
    # Summation.cpp
    Undefined.cpp
    Variable.cpp)

# Adds a library target called "Oasis" to be built from source files.
add_library(Oasis ${Oasis_SOURCES})
add_library(Oasis::Oasis ALIAS Oasis)

string(REPLACE "." ";" VERSION_TOKENS ${CMAKE_CXX_COMPILER_VERSION})
list(GET VERSION_TOKENS 0 COMPILER_MAJOR_VERSION)
target_compile_definitions(
    Oasis PUBLIC OASIS_COMPILER_MAJOR_VERSION=${COMPILER_MAJOR_VERSION})

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_definitions(Oasis PUBLIC OASIS_COMPILER_CLANG)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_definitions(Oasis PUBLIC OASIS_COMPILER_GCC)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_definitions(Oasis PUBLIC OASIS_COMPILER_MSVC)
    target_compile_options(Oasis PRIVATE /bigobj)
endif()

if(OASIS_BUILD_PARANOID)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(Oasis PRIVATE /W3 /WX)
    else()
        target_compile_options(Oasis PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
endif()

target_compile_features(Oasis PUBLIC cxx_std_23)
target_link_libraries(Oasis PUBLIC Oasis::Headers Eigen3::Eigen
                                   gsl::gsl-lite-v1)

if(NOT OASIS_BUILD_JS)
    target_link_libraries(Oasis PUBLIC Boost::any Boost::callable_traits
                                       Boost::mpl)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Oasis_SOURCES})

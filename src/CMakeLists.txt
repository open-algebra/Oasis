# ##############################################################################
# OASIS: Open Algebra Software for Inferring Solutions
#
# CMakeLists.txt - OASIS library
# ##############################################################################

# These variables MUST be modified whenever a new source file is added.
set(Oasis_SOURCES
    # cmake-format: sortable
    Add.cpp
    Derivative.cpp
    Divide.cpp
    Exponent.cpp
    Expression.cpp
    Imaginary.cpp
    Integral.cpp
    Linear.cpp
    Log.cpp
    Multiply.cpp
    Negate.cpp
    Real.cpp
    Subtract.cpp
    Undefined.cpp
    Variable.cpp)

# Adds a library target called "Oasis" to be built from source files.
add_library(Oasis ${Oasis_SOURCES})
add_library(Oasis::Oasis ALIAS Oasis)

if(MSVC)
    target_compile_options(Oasis PRIVATE /W3 /WX)
    target_compile_options(Oasis PRIVATE /bigobj)
else()
    target_compile_options(Oasis PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_compile_features(Oasis PUBLIC cxx_std_20)
target_link_libraries(Oasis PUBLIC Oasis::Headers Eigen3::Eigen)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Oasis_SOURCES})

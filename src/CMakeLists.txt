# ##############################################################################
# OASIS: Open Algebra Software for Inferring Solutions
#
# CMakeLists.txt - OASIS library
# ##############################################################################

# These variables MUST be modified whenever a new source file is added.
set(Oasis_SOURCES
    # cmake-format: sortable
    Add.cpp
#    DefiniteIntegral.cpp
    Derivative.cpp
    Divide.cpp
    Exponent.cpp
    Expression.cpp
    Imaginary.cpp
    Linear.cpp
    Log.cpp
    Multiply.cpp
    Negate.cpp
    Real.cpp
    Subtract.cpp
#    Summation.cpp
    Undefined.cpp
    Variable.cpp)

set(Oasis_HEADERS
    # cmake-format: sortable
    ../include/Oasis/Add.hpp
    ../include/Oasis/BinaryExpression.hpp
    ../include/Oasis/BoundedBinaryExpression.hpp
    ../include/Oasis/BoundedExpression.hpp
    ../include/Oasis/BoundedUnaryExpression.hpp
#    ../include/Oasis/DefiniteIntegral.hpp
    ../include/Oasis/Derivative.hpp
    ../include/Oasis/Divide.hpp
    ../include/Oasis/Exponent.hpp
    ../include/Oasis/Expression.hpp
    ../include/Oasis/Imaginary.hpp
    ../include/Oasis/LeafExpression.hpp
    ../include/Oasis/Linear.hpp
    ../include/Oasis/Log.hpp
    ../include/Oasis/Multiply.hpp
    ../include/Oasis/Negate.hpp
    ../include/Oasis/Real.hpp
    ../include/Oasis/Serialization.hpp
    ../include/Oasis/Subtract.hpp
#    ../include/Oasis/Summation.hpp
    ../include/Oasis/UnaryExpression.hpp
    ../include/Oasis/Undefined.hpp
    ../include/Oasis/Variable.hpp)

# Adds a header-only library target called "OasisHeaders"
add_library(OasisHeaders INTERFACE)
add_library(Oasis::Headers ALIAS OasisHeaders)

target_compile_features(OasisHeaders INTERFACE cxx_std_20)
target_include_directories(OasisHeaders
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(OasisHeaders INTERFACE Taskflow)
target_sources(OasisHeaders INTERFACE ${Oasis_HEADERS})

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
target_include_directories(Oasis PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(Oasis PUBLIC Oasis::Headers Eigen3::Eigen)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Oasis_SOURCES})

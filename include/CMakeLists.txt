set(Oasis_HEADERS
    # cmake-format: sortable
    Oasis/Add.hpp
    Oasis/BinaryExpression.hpp
    Oasis/Concepts.hpp
    Oasis/Derivative.hpp
    Oasis/Divide.hpp
    Oasis/EulerNumber.hpp
    Oasis/Exponent.hpp
    Oasis/Expression.hpp
    Oasis/FwdDecls.hpp
    Oasis/Imaginary.hpp
    Oasis/Integral.hpp
    Oasis/LeafExpression.hpp
    Oasis/Linear.hpp
    Oasis/Log.hpp
    Oasis/Magnitude.hpp
    Oasis/MatchCast.hpp
    Oasis/Multiply.hpp
    Oasis/Negate.hpp
    Oasis/Pi.hpp
    Oasis/Real.hpp
    Oasis/RecursiveCast.hpp
    Oasis/SimplifyVisitor.hpp
    Oasis/Sine.hpp
    Oasis/Subtract.hpp
    Oasis/UnaryExpression.hpp
    Oasis/Undefined.hpp
    Oasis/Variable.hpp
    Oasis/Visit.hpp)

# Adds a header-only library target called "OasisHeaders"
add_library(OasisHeaders INTERFACE)
add_library(Oasis::Headers ALIAS OasisHeaders)

target_compile_features(OasisHeaders INTERFACE cxx_std_23)
target_include_directories(
    OasisHeaders INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                           $<INSTALL_INTERFACE:include>)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Oasis_HEADERS})

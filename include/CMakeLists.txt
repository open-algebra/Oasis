set(Oasis_HEADERS
    # cmake-format: sortable
    Oasis/Add.hpp
    Oasis/BinaryExpression.hpp
    Oasis/Derivative.hpp
    Oasis/Divide.hpp
    Oasis/EulerNumber.hpp
    Oasis/Exponent.hpp
    Oasis/Expression.hpp
    Oasis/Imaginary.hpp
    Oasis/Integral.hpp
    Oasis/LeafExpression.hpp
    Oasis/Linear.hpp
    Oasis/Log.hpp
    Oasis/Magnitude.hpp
    Oasis/Multiply.hpp
    Oasis/Negate.hpp
    Oasis/Pi.hpp
    Oasis/Real.hpp
    Oasis/Serialization.hpp
    Oasis/Subtract.hpp
    Oasis/UnaryExpression.hpp
    Oasis/Undefined.hpp
    Oasis/Variable.hpp)

# Adds a header-only library target called "OasisHeaders"
add_library(OasisHeaders INTERFACE)
add_library(Oasis::Headers ALIAS OasisHeaders)

target_compile_features(OasisHeaders INTERFACE cxx_std_20)
target_include_directories(
    OasisHeaders INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                           $<INSTALL_INTERFACE:include>)
target_link_libraries(OasisHeaders INTERFACE Taskflow)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Oasis_HEADERS})

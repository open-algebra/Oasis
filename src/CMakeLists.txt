############################################################
# OASIS: Open Algebra Software for Inferring Solutions
# CMakeLists.txt - OASIS library
############################################################

# These variables MUST be modified accordingly whenever a new source (.cpp/.hpp) file is added.
set(Oasis_SOURCES 
    Add.cpp BinaryExpression.cpp Divide.cpp Exponent.cpp Expression.cpp Log.cpp Multiply.cpp 
    Real.cpp Subtract.cpp Variable.cpp
)

set(Oasis_HEADERS 
    ../include/Oasis/Add.hpp ../include/Oasis/BinaryExpression.hpp ../include/Oasis/Divide.hpp 
    ../include/Oasis/Exponent.hpp ../include/Oasis/Expression.hpp 
    ../include/Oasis/LeafExpression.hpp ../include/Oasis/Log.hpp ../include/Oasis/Multiply.hpp 
    ../include/Oasis/Real.hpp ../include/Oasis/Subtract.hpp ../include/Oasis/Variable.hpp
)

# Adds a library target called "Oasis" to be built from source files.
add_library(Oasis ${Oasis_SOURCES} ${Oasis_HEADERS})

target_compile_features(Oasis PUBLIC cxx_std_20)
target_include_directories(Oasis PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(Oasis PUBLIC fmt::fmt Eigen3::Eigen Taskflow)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Oasis_SOURCES})
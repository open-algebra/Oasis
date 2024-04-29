set(Oasis_EXTRAS_SOURCES src/MathMLSerializer.cpp src/InFixSerializer.cpp)

set(Oasis_EXTRAS_HEADERS include/Oasis/MathMLSerializer.hpp
                         include/Oasis/InFixSerializer.hpp)

add_library(OasisExtras ${Oasis_EXTRAS_SOURCES} ${Oasis_EXTRAS_HEADERS})
add_library(Oasis::Extras ALIAS OasisExtras)

target_include_directories(OasisExtras
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(OasisExtras PUBLIC Oasis::Headers fmt::fmt
                                         tinyxml2::tinyxml2)

if(OASIS_BUILD_TESTS)
    add_subdirectory(tests)
endif()

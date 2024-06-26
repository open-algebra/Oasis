set(Oasis_EXTRAS_SOURCES
    # cmake-format: sortable
    src/FromString.cpp src/InFixSerializer.cpp src/MathMLSerializer.cpp)

set(Oasis_EXTRAS_HEADERS
    # cmake-format: sortable
    include/Oasis/FromString.hpp include/Oasis/InFixSerializer.hpp
    include/Oasis/MathMLSerializer.hpp)

add_library(OasisExtras ${Oasis_EXTRAS_SOURCES} ${Oasis_EXTRAS_HEADERS})
add_library(Oasis::Extras ALIAS OasisExtras)

target_include_directories(OasisExtras
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(OasisExtras PUBLIC Oasis::Oasis fmt::fmt
                                         tinyxml2::tinyxml2)

if(OASIS_BUILD_TESTS)
    add_subdirectory(tests)
endif()

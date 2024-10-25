set(OASIS_IO_SOURCES
    # cmake-format: sortable
    src/FromString.cpp src/InFixSerializer.cpp src/TeXSerializer.cpp
    src/MathMLSerializer.cpp)

set(OASIS_IO_HEADERS
    # cmake-format: sortable
    include/Oasis/FromString.hpp include/Oasis/InFixSerializer.hpp
        include/Oasis/TeXSerializer.hpp include/Oasis/MathMLSerializer.hpp)

add_library(OasisIO ${OASIS_IO_SOURCES} ${OASIS_IO_HEADERS})
add_library(Oasis::IO ALIAS OasisIO)

target_include_directories(OasisIO PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(OasisIO PUBLIC Oasis::Oasis fmt::fmt tinyxml2::tinyxml2)

if(OASIS_BUILD_TESTS)
    add_subdirectory(tests)
endif()

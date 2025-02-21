set(OASIS_IO_SOURCES
    # cmake-format: sortable
    src/FromString.cpp src/InFixSerializer.cpp src/MathMLSerializer.cpp
    src/TeXSerializer.cpp)

set(OASIS_IO_HEADERS
    # cmake-format: sortable
    include/Oasis/FromString.hpp include/Oasis/InFixSerializer.hpp
    include/Oasis/MathMLSerializer.hpp include/Oasis/TeXSerializer.hpp)

add_library(OasisIO ${OASIS_IO_SOURCES} ${OASIS_IO_HEADERS})
add_library(Oasis::IO ALIAS OasisIO)

target_include_directories(OasisIO PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(OasisIO PUBLIC Oasis::Oasis Microsoft.GSL::GSL tinyxml2::tinyxml2)

if(OASIS_BUILD_PARANOID)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(OasisIO PRIVATE /W3 /WX)
    else()
        target_compile_options(OasisIO PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
endif()

if(OASIS_BUILD_TESTS)
    add_subdirectory(tests)
endif()

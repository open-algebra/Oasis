set(OASIS_IO_SOURCES
    # cmake-format: sortable
    src/FromPALM.cpp
    src/FromString.cpp
    src/InFixSerializer.cpp
    src/MathMLSerializer.cpp
    src/PALMHelper.cpp
    src/PALMSerializer.cpp
    src/PALMTokenizer.cpp
    src/TeXSerializer.cpp)

set(OASIS_IO_HEADERS
    # cmake-format: sortable
    include/Oasis/FromPALM.hpp
    include/Oasis/FromString.hpp
    include/Oasis/InFixSerializer.hpp
    include/Oasis/MathMLSerializer.hpp
    include/Oasis/PALMSerializer.hpp
    include/Oasis/TeXSerializer.hpp
    src/PALMHelper.hpp)

add_library(OasisIO ${OASIS_IO_SOURCES} ${OASIS_IO_HEADERS})
add_library(Oasis::IO ALIAS OasisIO)

target_include_directories(OasisIO PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
if(NOT OASIS_BUILD_JS)
    target_link_libraries(OasisIO PUBLIC Boost::bimap)
    target_link_libraries(OasisIO PUBLIC Boost::assign)
    target_include_directories(OasisIO PUBLIC ${Boost_INCLUDE_DIRS})
    target_include_directories(OasisIO
                               PRIVATE ${Boost_SOURCE_DIR}/libs/assign/include)
endif()
target_link_libraries(OasisIO PUBLIC Oasis::Oasis tinyxml2::tinyxml2)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(OasisIO PUBLIC /utf-8)
endif()

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

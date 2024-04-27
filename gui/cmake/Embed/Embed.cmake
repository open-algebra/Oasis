set(EMBED_CMAKE_DIR ${CMAKE_CURRENT_LIST_DIR})

function(embed_file)
    
    set(OPTION_ARGS)
    
    set(ONE_VALUE_ARGS
        FILEPATH
        OUTPUT_INCLUDE_NAME
        OUTPUT_SOURCE_NAME)
    
    set(MULTI_VALUE_ARGS)
    
    cmake_parse_arguments(EMBED "${OPTION_ARGS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})
    
    cmake_path(GET EMBED_FILEPATH FILENAME EMBED_FILENAME)
    string(REGEX REPLACE "[.-]" "_" EMBED_FILENAME_CPPNAME ${EMBED_FILENAME})

    if (NOT EMBED_OUTPUT_INCLUDE_NAME)
        set(EMBED_OUTPUT_INCLUDE_NAME "${EMBED_FILENAME}.hpp")
    endif ()
    
    if (NOT EMBED_OUTPUT_SOURCE_NAME)
        set(EMBED_OUTPUT_SOURCE_NAME "${EMBED_FILENAME}.cpp")
    endif()
    
    file(READ ${EMBED_FILEPATH} "${EMBED_FILENAME}_DATA" HEX)
    
    string(REGEX MATCHALL "([A-Fa-f0-9][A-Fa-f0-9])" SEPARATED_HEX ${${EMBED_FILENAME}_DATA})
    set(DATA_SIZE 0)
    
    # Iterate through each of the bytes from the source file
    foreach (hex IN LISTS SEPARATED_HEX)
       # Write the hex string to the line with an 0x prefix
       # and a , postfix to seperate the bytes of the file.
       string(APPEND DATA "0x${hex},")
       math(EXPR DATA_SIZE "${DATA_SIZE}+1")
    endforeach()
    
    configure_file("${EMBED_CMAKE_DIR}/EmbedTemplate.hpp.in" ${EMBED_OUTPUT_INCLUDE_NAME})
    configure_file("${EMBED_CMAKE_DIR}/EmbedTemplate.cpp.in" ${EMBED_OUTPUT_SOURCE_NAME})
    
    add_library(${EMBED_FILENAME_CPPNAME}
        "${CMAKE_CURRENT_BINARY_DIR}/${EMBED_OUTPUT_INCLUDE_NAME}"
        "${CMAKE_CURRENT_BINARY_DIR}/${EMBED_OUTPUT_SOURCE_NAME}")
        
    target_include_directories(${EMBED_FILENAME_CPPNAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
    
endfunction()
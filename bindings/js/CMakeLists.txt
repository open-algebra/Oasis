set(OasisJS_SOURCES src/bindings.cpp)

add_library(OasisJS INTERFACE)
target_sources(OasisJS INTERFACE ${OasisJS_SOURCES})

target_compile_options(Oasis PUBLIC --use-port=boost_headers)
target_link_options(Oasis PUBLIC --use-port=boost_headers)

target_compile_options(OasisIO PUBLIC -fexceptions)
target_link_options(OasisIO PUBLIC -fexceptions)

target_link_options(
    OasisJS
    INTERFACE
    --no-entry
    --whole-archive
    -sMODULARIZE
    -sEXPORT_ES6
    -sEXPORT_NAME='loadOasis')

target_link_libraries(OasisJS INTERFACE embind Oasis::Oasis Oasis::IO)

function(configure_oasis_target target_name target_environment output_name)
    add_executable(${target_name})
    target_link_libraries(${target_name} PUBLIC OasisJS)
    set_target_properties(${target_name} PROPERTIES OUTPUT_NAME ${output_name})
    target_link_options(${target_name} PUBLIC
            -sENVIRONMENT=${target_environment}
            --emit-tsd
            "${CMAKE_CURRENT_BINARY_DIR}/${output_name}.d.ts")
endfunction()

configure_oasis_target(OasisJSWeb "'web'" "oasis-web")
configure_oasis_target(OasisJSNode "'node'" "oasis-node")


file(COPY package.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

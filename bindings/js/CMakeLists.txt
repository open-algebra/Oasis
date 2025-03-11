set(OasisJS_SOURCES src/bindings.cpp)

add_executable(OasisJS ${OasisJS_SOURCES})

target_compile_options(OasisIO PUBLIC -fexceptions)
target_link_options(OasisIO PUBLIC -fexceptions)

target_link_options(OasisJS PUBLIC
        --no-entry
        --whole-archive
        -sMODULARIZE
        -sEXPORT_ES6
        -sEXPORT_NAME='loadOasis'
        --emit-tsd "$<TARGET_FILE_DIR:OasisJS>/oasis.d.ts")

target_link_libraries(OasisJS PUBLIC embind Oasis::Oasis Oasis::IO)
set_target_properties(OasisJS PROPERTIES OUTPUT_NAME "oasis")
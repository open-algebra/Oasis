project(OasisGUI)

find_package(wxWidgets REQUIRED COMPONENTS core base webview)

include(cmake/Embed/Embed.cmake)

embed_file(FILEPATH assets/Fox.svg)
embed_file(FILEPATH assets/bootstrap-5.3.3-dist/css/bootstrap.min.css)
embed_file(FILEPATH assets/bootstrap-5.3.3-dist/js/bootstrap.bundle.min.js)

set(OasisGUI_SOURCES
    main.cpp
    components/KeypadButton/KeypadButton.cpp
    components/KeypadButton/KeypadButton.hpp
    views/DefaultView.cpp
    views/DefaultView.hpp
)

if(wxWidgets_USE_FILE) # not defined in CONFIG mode
    include(${wxWidgets_USE_FILE})
endif()

add_executable(OasisGUI ${OasisGUI_SOURCES})
target_link_libraries(OasisGUI Oasis::Oasis ${wxWidgets_LIBRARIES} Fox_svg bootstrap_min_css bootstrap_bundle_min_js)
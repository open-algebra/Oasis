project(OasisGUI)

find_package(wxWidgets REQUIRED COMPONENTS core base webview)

include(cmake/Embed/Embed.cmake)

embed_file(FILEPATH assets/Fox.svg)
embed_file(FILEPATH assets/bootstrap-5.3.3-dist/css/bootstrap.min.css)
embed_file(FILEPATH assets/bootstrap-5.3.3-dist/js/bootstrap.bundle.min.js)

execute_process(COMMAND npm run build WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/components/web-content)

embed_file(FILEPATH components/EquationViewer/web-content/dist/index.html)
embed_file(FILEPATH components/EquationViewer/web-content/dist/assets/index.js)
embed_file(FILEPATH components/EquationViewer/web-content/dist/assets/index.css)

set(OasisGUI_SOURCES
    main.cpp
    components/EquationViewer/EquationViewer.cpp
    components/EquationViewer/EquationViewer.hpp
    components/FunctionBuilder/FunctionBuilder.cpp
    components/FunctionBuilder/FunctionBuilder.hpp
    components/KeypadButton/KeypadButton.cpp
    components/KeypadButton/KeypadButton.hpp
    InputPreprocessor.cpp
    InputPreprocessor.hpp
    views/DefaultView/DefaultView.cpp
    views/DefaultView/DefaultView.hpp
    views/PreferencesPanel/PreferencesPanel.cpp
    views/PreferencesPanel/PreferencesPanel.hpp
)

if(wxWidgets_USE_FILE) # not defined in CONFIG mode
    include(${wxWidgets_USE_FILE})
endif()

add_executable(OasisGUI ${OasisGUI_SOURCES})
target_include_directories(OasisGUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(OasisGUI Oasis::Oasis Oasis::Extras ${wxWidgets_LIBRARIES} Fox_svg bootstrap_min_css bootstrap_bundle_min_js)
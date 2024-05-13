project(OasisGUI)

find_package(wxWidgets REQUIRED COMPONENTS core base webview)

include(cmake/Embed/Embed.cmake)

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

target_link_libraries(OasisGUI Oasis::Oasis Oasis::Extras ${wxWidgets_LIBRARIES})

add_custom_target(copy_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        ${CMAKE_CURRENT_BINARY_DIR}/assets
)
add_dependencies(OasisGUI copy_assets)
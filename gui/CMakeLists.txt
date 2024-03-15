project(OasisGUI)

find_package(wxWidgets REQUIRED COMPONENTS core base webview)

set(OasisGUI_SOURCES
    main.cpp
    views/ArithmeticView.cpp
    views/ArithmeticView.hpp
    components/KeypadButton/KeypadButton.cpp
    components/KeypadButton/KeypadButton.hpp
)

if(wxWidgets_USE_FILE) # not defined in CONFIG mode
    include(${wxWidgets_USE_FILE})
endif()

add_executable(OasisGUI ${OasisGUI_SOURCES})
target_link_libraries(OasisGUI Oasis::Oasis ${wxWidgets_LIBRARIES})
//
// Created by Matthew McCall on 11/9/22.
//

#ifndef OASIS_KEYPADBUTTON_HPP
#define OASIS_KEYPADBUTTON_HPP

#include "wx/sizer.h"
#include "wx/wx.h"

class KeypadButton : public wxWindow {
public:
    KeypadButton(wxFrame *parent, wxWindowID id, wxString text);

    void Init();

    void paintEvent(wxPaintEvent &evt);
    void paintNow();

    void render(wxDC &dc);

    // some useful events
    void mouseDown(wxMouseEvent &event);
    void mouseReleased(wxMouseEvent &event);
    void mouseLeftWindow(wxMouseEvent &event);
    void mouseMoved(wxMouseEvent& event);

    [[nodiscard]] wxString getText();

    DECLARE_EVENT_TABLE()

private:
    static constexpr int MIN_BUTTON_WIDTH = 50;
    static constexpr int MIN_BUTTON_HEIGHT = 25;

    bool pressedDown;
    bool hovered;
    wxString text;
    wxSize lastSize;
    int fontSize = 0;
    static int computeFontSize(const wxSize& dcSize) ;
};

#endif//OASIS_KEYPADBUTTON_HPP

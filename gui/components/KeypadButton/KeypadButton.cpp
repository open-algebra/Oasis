//
// Created by Matthew McCall on 11/9/22.
//

#include <cmath>
#include <utility>

#include "KeypadButton.hpp"

BEGIN_EVENT_TABLE(KeypadButton, wxPanel)

EVT_LEFT_DOWN(KeypadButton::mouseDown)
EVT_LEFT_UP(KeypadButton::mouseReleased)
EVT_MOTION(KeypadButton::mouseMoved)
EVT_LEAVE_WINDOW(KeypadButton::mouseLeftWindow)

// catch paint events
EVT_PAINT(KeypadButton::paintEvent)

END_EVENT_TABLE()

KeypadButton::KeypadButton(wxFrame* parent, const wxWindowID id, wxString text)
    : wxWindow(parent, id)
    , text(std::move(text))
    , pressedDown(false)
    , hovered(false)
{
    Init();
}

void KeypadButton::paintEvent(wxPaintEvent &evt) {
    wxPaintDC dc(this);
    render(dc);
}

void KeypadButton::paintNow() {
    wxClientDC dc(this);
    render(dc);
}

void KeypadButton::render(wxDC &dc) {

    dc.SetPen(wxPen(wxColour(0, 0, 0, 64)));

    if (pressedDown)
        dc.SetBrush(wxBrush(wxColour(127, 127, 127, 64)));
    else if (hovered)
        dc.SetBrush(wxBrush(wxColour(255, 255, 255, 128)));
    else
        dc.SetBrush(wxBrush(wxColour(225, 225, 225, 64)));

    const wxSize dcSize = dc.GetSize();

    if (lastSize != dcSize) {
        fontSize = computeFontSize(dcSize);
        lastSize = dcSize;
    }

    dc.DrawRoundedRectangle(0, 0, dcSize.GetWidth(), dcSize.GetHeight(), 4);
    dc.SetFont(wxFont(fontSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    wxSize textSize = dc.GetTextExtent(text);
    dc.DrawText(text, (dcSize.GetWidth() - textSize.GetWidth()) / 2, (dcSize.GetHeight() - textSize.GetHeight()) / 2);
}

int KeypadButton::computeFontSize(const wxSize& dcSize) { return static_cast<int>(2 * (std::log((dcSize.GetHeight() * 0.75) + 1)) + 9); }

void KeypadButton::mouseDown(wxMouseEvent &event) {
    pressedDown = true;
    paintNow();
}

void KeypadButton::mouseReleased(wxMouseEvent &event) {
    pressedDown = false;
    paintNow();
}

void KeypadButton::mouseLeftWindow(wxMouseEvent &event) {
    pressedDown = false;
    hovered = false;
    paintNow();
}
void KeypadButton::Init() {
    SetMinSize(wxSize(MIN_BUTTON_WIDTH, MIN_BUTTON_HEIGHT));
}

void KeypadButton::mouseMoved(wxMouseEvent &event) {
    hovered = true;
    paintNow();
}

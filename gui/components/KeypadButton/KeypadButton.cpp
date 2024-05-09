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

void KeypadButton::Init() {
    SetMinSize(wxSize(MIN_BUTTON_WIDTH, MIN_BUTTON_HEIGHT));
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

    const wxSize dcSize = dc.DeviceToLogicalRel(dc.GetSize());
    // dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW), 2));
    dc.SetPen(wxPen(wxColour(127, 127, 127, 32), 2));
    dc.SetBrush(wxBrush(wxTransparentColor));

    if (hovered) {
        const wxColour spotlightColor = wxSystemSettings::GetAppearance().IsDark() ? wxColour(255, 255, 255, 32) : *wxWHITE;
        dc.GradientFillConcentric(wxRect(dcSize * 2), spotlightColor, wxColour(255, 255, 255, wxALPHA_TRANSPARENT), lastMousePos);
        dc.DrawRectangle(1, 1, dcSize.GetWidth() - 1, dcSize.GetHeight() - 1);
    }

    if (pressedDown) {
        dc.SetBrush(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW)));
        dc.DrawRectangle(1, 1, dcSize.GetWidth() - 1, dcSize.GetHeight() - 1);
    }

    if (lastSize != dcSize) {
        fontSize = computeFontSize(dcSize);
        lastSize = dcSize;
    }

    const wxSize textSize = dc.GetTextExtent(text);
    dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT)));
    dc.SetFont(wxFont(fontSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
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

void KeypadButton::mouseMoved(wxMouseEvent& event)
{
    hovered = true;
    lastMousePos = event.GetPosition();
    paintNow();
}

wxString KeypadButton::getText()
{
    return text;
}

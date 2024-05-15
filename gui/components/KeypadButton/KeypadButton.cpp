//
// Created by Matthew McCall on 11/9/22.
//

#include <cmath>
#include <utility>

#include <wx/graphics.h>

#include "KeypadButton.hpp"

BEGIN_EVENT_TABLE(KeypadButton, wxWindow)

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

void KeypadButton::render(wxClientDC& dc)
{
    dc.SetBackground(wxBrush(*wxTRANSPARENT_BRUSH));
    dc.Clear();

    const wxSize dcSize = dc.DeviceToLogicalRel(dc.GetSize());

    dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW), 1));

    if (hovered) {
        dc.SetBrush(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT)));
    } else if (pressedDown) {
        dc.SetBrush(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW)));
    } else {
        dc.SetBrush(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE)));
    }

    dc.DrawRoundedRectangle(1, 1, dcSize.GetWidth() -1, dcSize.GetHeight() - 1, 4);

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
    paintNow();
}

wxString KeypadButton::getText()
{
    return text;
}

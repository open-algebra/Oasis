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

void KeypadButton::render(wxClientDC& dc) {
    const wxSize dcSize = dc.DeviceToLogicalRel(dc.GetSize());
    const wxColour spotlightColor = wxSystemSettings::GetAppearance().IsDark() ? wxColour(255, 255, 255, 32) : *wxWHITE;

    dc.SetPen(wxPen(wxColour(127, 127, 127, 32), 2));

    if (hovered) {
        dc.GradientFillConcentric(wxRect(dcSize * 2), spotlightColor, wxColour(255, 255, 255, wxALPHA_TRANSPARENT), lastMousePos);
    }

    if (pressedDown) {
        dc.SetBrush(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW)));
    }

    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);

    if (gc) {
        auto [relX, relY] = ScreenToClient(wxGetMousePosition());
        const unsigned borderThickness = 2;
        const unsigned borderWidth = dcSize.GetWidth();
        const unsigned borderHeight = dcSize.GetHeight();

        auto spotlightPath = gc->CreatePath();
        spotlightPath.AddRectangle(0, 0, borderWidth - borderThickness, borderThickness);
        spotlightPath.AddRectangle(borderWidth - borderThickness, 0, borderThickness, borderHeight - borderThickness);
        spotlightPath.AddRectangle(borderThickness, borderHeight - borderThickness, borderWidth - borderThickness, borderThickness);
        spotlightPath.AddRectangle(0, borderThickness, borderThickness, borderHeight - borderThickness);

        wxColour startColour = wxColour(255, 128, 128, 128);
        wxColour endColour = wxColour(128, 128, 255, 128);

        wxGraphicsBrush spotLightBrush = gc->CreateRadialGradientBrush(relX, relY, relX, relY, std::max(borderWidth, borderHeight), wxColour(127, 127, 127, 32), wxColour(127, 127, 127, wxALPHA_TRANSPARENT));
        gc->SetBrush(spotLightBrush);

        gc->FillPath(spotlightPath);
        delete gc;
    } else {

        dc.SetBrush(wxBrush(wxTransparentColor));
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

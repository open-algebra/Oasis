//
// Created by Matthew McCall on 5/10/24.
//

#include "PreferencesPanel.hpp"

#include <wx/choice.h>
#include <wx/clrpicker.h>
#include <wx/stattext.h>

PreferencesPanel::PreferencesPanel(wxWindow* parent, wxWindowID id)
    : wxDialog(parent, id, "Preferences")
{
            // Create labels
    const auto themeLabel = new wxStaticText(this, wxID_ANY, "Theme:");
    const auto colorLabel = new wxStaticText(this, wxID_ANY, "Accent Color:");

    // Dropdown menu for selecting theme
    wxArrayString themes;
    themes.Add("Light");
    themes.Add("Dark");
    themes.Add("Automatic");
    const auto themeChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, themes);
    themeChoice->SetSelection(2); // Set default selection to 'Automatic'

    // Color picker for selecting accent color
    const auto colorPicker = new wxColourPickerCtrl(this, wxID_ANY, *wxBLUE);

    // Button for accepting the changes
    const auto acceptButton = new wxButton(this, wxID_OK, "Accept");
    acceptButton->SetDefault();

    // Layout with wxFlexGridSizer for organizing labels and controls
    const auto flex_grid_sizer = new wxFlexGridSizer(2, 2, 4, 4);

    // Add items and their labels to the sizer
    flex_grid_sizer->Add(themeLabel, wxSizerFlags().Right().CenterVertical());
    flex_grid_sizer->Add(themeChoice);
    flex_grid_sizer->Add(colorLabel, wxSizerFlags().Right().CenterVertical());
    flex_grid_sizer->Add(colorPicker);

    // Layout with wxBoxSizer for vertical arrangement
    const auto mainSizer = new wxBoxSizer(wxVERTICAL);

    // Add grid sizer and button to main vertical sizer
    mainSizer->Add(flex_grid_sizer, wxSizerFlags().Expand().Border());
    mainSizer->AddSpacer(4);
    mainSizer->Add(acceptButton, wxSizerFlags().Right().Border(wxALL ^ wxTOP));

    SetSizerAndFit(mainSizer);

    themeChoice->Bind(wxEVT_CHOICE, [this](wxCommandEvent& event) {
        switch (event.GetSelection()) {
        case 0:
            preferences.theme = Preferences::Theme::Light;
            break;
        case 1:
            preferences.theme = Preferences::Theme::Dark;
            break;
        case 2:
            preferences.theme = Preferences::Theme::Auto;
            break;
        default:
            preferences.theme = Preferences::Theme::Auto;
            break;
        }
    });

    colorPicker->Bind(wxEVT_COLOURPICKER_CHANGED, [this](wxColourPickerEvent& event) {
        preferences.themeColor = event.GetColour().GetAsString(wxC2S_CSS_SYNTAX).ToStdString();
    });
}

Preferences PreferencesPanel::GetPreferences()
{
    return preferences;
}

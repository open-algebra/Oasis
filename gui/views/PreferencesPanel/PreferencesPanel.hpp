//
// Created by Matthew McCall on 5/10/24.
//

#ifndef OASIS_PREFERENCESPANEL_HPP
#define OASIS_PREFERENCESPANEL_HPP

#include <wx/dialog.h>

struct Preferences {

    enum class Theme {
        Light,
        Dark,
        Auto
    } theme;

    std::string themeColor;
};

class PreferencesPanel : public wxDialog {
public:
    PreferencesPanel(wxWindow* parent, wxWindowID id);
    Preferences GetPreferences();

private:
    Preferences preferences {};
};

#endif // OASIS_PREFERENCESPANEL_HPP

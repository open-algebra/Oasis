//
// Created by Matthew McCall on 5/12/24.
//

#include "fmt/format.h"
#include <wx/settings.h>

#include "EquationViewer.hpp"

void EquationViewer::setWebView(wxWebView* web_view)
{
    webView = web_view;
}

void EquationViewer::Init() const
{
    webView->LoadURL("memory:index.html");

    webView->Bind(wxEVT_WEBVIEW_LOADED,
        [this](wxWebViewEvent& event) {
            if (wxSystemSettings::GetAppearance().IsDark()) {
                this->setDarkTheme();
            } else {
                this->setLightTheme();
            }
        });

    // Bind System Colour Change Event to a handler
    webView->Bind(wxEVT_SYS_COLOUR_CHANGED, [this](wxSysColourChangedEvent& event) {
        // Check the system's current theme and update the application theme accordingly
        if (wxSystemSettings::GetAppearance().IsDark()) {
            this->setDarkTheme();
        } else {
            this->setLightTheme();
        }

        // Continue processing the event in case other handlers are interested
        event.Skip();
    });
}

void EquationViewer::addEntryToHistory(const std::string& query, const std::string& response) const
{
    std::string formattedQuery = query;
    std::string formattedResponse = response;
    std::ranges::replace(formattedQuery, '\n', ' ');
    std::ranges::replace(formattedResponse, '\n', ' ');

    const std::string js = fmt::format(R"(addToHistory("{}","{}"))", formattedQuery, formattedResponse);
    webView->RunScriptAsync(js);
}

void EquationViewer::setCurrentEntry(const std::string& entry) const
{
    std::string formattedEntry = entry;
    std::ranges::replace(formattedEntry, '\n', ' ');

    const std::string js = fmt::format(R"(setCurrentInput("{}"))", formattedEntry);
    webView->RunScriptAsync(js);
}

void EquationViewer::setLightTheme() const
{
    webView->RunScriptAsync("setLightTheme()");
}

void EquationViewer::setDarkTheme() const
{
    webView->RunScriptAsync("setDarkTheme()");
}

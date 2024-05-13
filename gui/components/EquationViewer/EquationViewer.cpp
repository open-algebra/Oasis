//
// Created by Matthew McCall on 5/12/24.
//

#include <regex>

#include "fmt/format.h"

#include "EquationViewer.hpp"

void EquationViewer::setWebView(wxWebView* web_view)
{
    webView = web_view;
}

void EquationViewer::LoadIndex() const
{
    webView->LoadURL("memory:index.html");
}

void EquationViewer::addEntryToHistory(const std::string& query, const std::string& response) const
{
    std::string formattedQuery = query;
    std::string formattedResponse = response;
    std::ranges::replace(formattedQuery, '\n', ' ');
    std::ranges::replace(formattedResponse, '\n', ' ');

    const std::string js = fmt::format(R"(addToHistory("{}","{}"))", formattedQuery, formattedResponse);
    webView->RunScript(js);
}

void EquationViewer::setCurrentEntry(const std::string& entry) const
{
    std::string formattedEntry = entry;
    std::ranges::replace(formattedEntry, '\n', ' ');

    const std::string js = fmt::format(R"(setCurrentInput("{}"))", formattedEntry);
    webView->RunScript(js);
}

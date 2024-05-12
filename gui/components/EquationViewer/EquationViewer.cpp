//
// Created by Matthew McCall on 5/12/24.
//

#include "fmt/format.h"

#include "EquationViewer.hpp"

EquationViewer::EquationViewer(wxWebView* webView) : webView(webView)
{
    webView->LoadURL("memory:index.html");
}

void EquationViewer::addEntryToHistory(const std::string& query, const std::string& response) const
{
    webView->RunScriptAsync(fmt::format(R"(import("memory:index.js").then(m => m.addEntryToHistory("{}","{}")))", query, response));
}

void EquationViewer::setCurrentEntry(const std::string& entry) const
{
    webView->RunScriptAsync(fmt::format(R"(import("memory:index.js").then(m => m.setCurrentEntry("{}")))", entry));
}

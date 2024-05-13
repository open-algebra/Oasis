//
// Created by Matthew McCall on 5/12/24.
//

#ifndef EQUATIONVIEWER_HPP
#define EQUATIONVIEWER_HPP

#include <string>

#include <wx/webview.h>

class EquationViewer {
public:

    void setWebView(wxWebView* web_view);

    void LoadIndex() const;
    void addEntryToHistory(const std::string& query, const std::string& response) const;
    void setCurrentEntry(const std::string& entry) const;

private:
    wxWebView* webView = nullptr;
};



#endif //EQUATIONVIEWER_HPP

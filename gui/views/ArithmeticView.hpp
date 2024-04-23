//
// Created by Matthew McCall on 2/16/24.
//

#ifndef ARITHMETICVIEW_HPP
#define ARITHMETICVIEW_HPP

#include "wx/frame.h"

#include "Oasis/Expression.hpp"

class wxWebView;
class wxTextCtrl;

class ArithmeticView final : public wxFrame {
public:
    ArithmeticView();

private:
    std::string currentInput;
    std::unique_ptr<Oasis::Expression> currentExpression;

    std::vector<std::pair<std::unique_ptr<Oasis::Expression>, std::unique_ptr<Oasis::Expression>>> history;

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* body;

    void renderPage(wxWebView* webView);
    void onEnter(wxWebView* webView, wxTextCtrl* textCtrl);
};



#endif //ARITHMETICVIEW_HPP

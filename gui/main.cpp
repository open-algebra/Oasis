//
// Created by Matthew McCall on 2/16/24.
//
#include "views/ArithmeticView.hpp"

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class Application final : public wxApp {
public:
    bool OnInit() final;
};

bool Application::OnInit()
{
    auto* arithmeticView = new  ArithmeticView();
    return arithmeticView->Show(true);
}

wxIMPLEMENT_APP(Application);
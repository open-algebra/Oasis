//
// Created by Matthew McCall on 2/16/24.
//

#include <wx/menu.h>
#include <wx/msgdlg.h>

#include "ArithmeticView.hpp"

ArithmeticView::ArithmeticView()
    : wxFrame(nullptr, wxID_ANY, "Arithmetic")
{
    auto* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    auto* menuBar = new wxMenuBar;
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);

    Bind(wxEVT_MENU, &ArithmeticView::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &ArithmeticView::OnExit, this, wxID_EXIT);
}

void ArithmeticView::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("Oasis GUI");
}

void ArithmeticView::OnExit(wxCommandEvent& event)
{
    Close(true);
}

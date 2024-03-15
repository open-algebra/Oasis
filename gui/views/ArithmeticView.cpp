//
// Created by Matthew McCall on 2/16/24.
//

#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/webview.h>

#include "ArithmeticView.hpp"


#include "../components/KeypadButton/KeypadButton.hpp"

ArithmeticView::ArithmeticView()
    : wxFrame(nullptr, wxID_ANY, "Arithmetic")
{
    auto* mainSizer = new wxBoxSizer(wxVERTICAL);

//    auto* splitter = new wxSplitterWindow(this, wxID_ANY);
//    splitter->SetSashGravity(0.5);
//    splitter->SetMinimumPaneSize(20);

//    mainSizer->Add(splitter, wxSizerFlags(1).Expand());

    auto *webView = wxWebView::New(this, wxID_ANY);
    webView->SetPage("<html><body><h1>Hello, world!</h1></body></html>", "");

    auto *keypad = new wxGridSizer(5, 4, 4, 4);

    auto* keyClear = new KeypadButton(this, wxID_ANY, "Clear");
    auto* keyLeftParens = new KeypadButton(this, wxID_ANY, "(");
    auto* keyRightParens = new KeypadButton(this, wxID_ANY, ")");
    auto* keyDivide = new KeypadButton(this, wxID_ANY, "÷");
    auto* key7 = new KeypadButton(this, wxID_ANY, "7");
    auto* key8 = new KeypadButton(this, wxID_ANY, "8");
    auto* key9 = new KeypadButton(this, wxID_ANY, "9");
    auto* keyMultiply = new KeypadButton(this, wxID_ANY, "*");
    auto* key4 = new KeypadButton(this, wxID_ANY, "4");
    auto* key5 = new KeypadButton(this, wxID_ANY, "5");
    auto* key6 = new KeypadButton(this, wxID_ANY, "6");
    auto* keySubtract = new KeypadButton(this, wxID_ANY, "-");
    auto* key1 = new KeypadButton(this, wxID_ANY, "1");
    auto* key2 = new KeypadButton(this, wxID_ANY, "2");
    auto* key3 = new KeypadButton(this, wxID_ANY, "3");
    auto* keyAdd = new KeypadButton(this, wxID_ANY, "+");
    auto* key0 = new KeypadButton(this, wxID_ANY, "0");
    auto* keyNegate = new KeypadButton(this, wxID_ANY, "(-)");
    auto* keyDot = new KeypadButton(this, wxID_ANY, ".");
    auto* keyEnter = new KeypadButton(this, wxID_ANY, "Enter");

    keypad->Add(keyClear, wxSizerFlags().Expand());
    keypad->Add(keyLeftParens, wxSizerFlags().Expand());
    keypad->Add(keyRightParens, wxSizerFlags().Expand());
    keypad->Add(keyDivide, wxSizerFlags().Expand());
    keypad->Add(key7, wxSizerFlags().Expand());
    keypad->Add(key8, wxSizerFlags().Expand());
    keypad->Add(key9, wxSizerFlags().Expand());
    keypad->Add(keyMultiply, wxSizerFlags().Expand());
    keypad->Add(key4, wxSizerFlags().Expand());
    keypad->Add(key5, wxSizerFlags().Expand());
    keypad->Add(key6, wxSizerFlags().Expand());
    keypad->Add(keySubtract, wxSizerFlags().Expand());
    keypad->Add(key1, wxSizerFlags().Expand());
    keypad->Add(key2, wxSizerFlags().Expand());
    keypad->Add(key3, wxSizerFlags().Expand());
    keypad->Add(keyAdd, wxSizerFlags().Expand());
    keypad->Add(key0, wxSizerFlags().Expand());
    keypad->Add(keyNegate, wxSizerFlags().Expand());
    keypad->Add(keyDot, wxSizerFlags().Expand());
    keypad->Add(keyEnter, wxSizerFlags().Expand());

//    auto* keypadPanel = new wxPanel(this, wxID_ANY);
//    keypadPanel->SetSizerAndFit(keypad);

//    splitter->SplitVertically(webView, keypadPanel);
    mainSizer->Add(webView, wxSizerFlags(1).Expand());
    mainSizer->Add(keypad, wxSizerFlags(1).Expand().Border(wxALL, 4));

    SetSizerAndFit(mainSizer);

    auto* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    auto* menuBar = new wxMenuBar;
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);

    Bind(wxEVT_MENU, [=](wxCommandEvent& event) {
        wxMessageBox("Oasis GUI");
    }, wxID_ABOUT);

    Bind(wxEVT_MENU, [=](wxCommandEvent& event) {
        Close(true);
    }, wxID_EXIT);
}
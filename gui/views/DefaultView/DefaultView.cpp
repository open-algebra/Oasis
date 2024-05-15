//
// Created by Matthew McCall on 2/16/24.
//

#include <fstream>
#include <vector>

#include "fmt/core.h"

#include "tinyxml2.h"

#include <wx/config.h>
#include <wx/fs_mem.h>
#include <wx/gbsizer.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/webview.h>
#include <wx/webviewfshandler.h>

#include "Oasis/FromString.hpp"

#include "components/EquationViewer/EquationViewer.hpp"
#include "components/FunctionBuilder/FunctionBuilder.hpp"
#include "components/KeypadButton/KeypadButton.hpp"
#include "views/PreferencesPanel/PreferencesPanel.hpp"

#include "InputPreprocessor.hpp"
#include "DefaultView.hpp"

std::string ExpressionToMathMLStr(const std::unique_ptr<Oasis::Expression>& expr)
{
    tinyxml2::XMLDocument doc;
    Oasis::MathMLSerializer serializer { doc };
    tinyxml2::XMLPrinter printer;

    tinyxml2::XMLElement* mathElement = doc.NewElement("math");
    mathElement->SetAttribute("display", "block");
    doc.InsertFirstChild(mathElement);

    expr->Serialize(serializer);
    tinyxml2::XMLElement* queryElement = serializer.GetResult();
    mathElement->InsertFirstChild(queryElement);

    doc.Print(&printer);
    return printer.CStr();
}

std::string ReadFileIntoString(const std::string& path) {
    std::ifstream f(path, std::ios::in | std::ios::binary);
    if (!f) {
        // Handle file open failure
        return "";
    }

    // Obtain the size of the file
    const auto sz = std::filesystem::file_size(path);

    // Create a buffer
    std::string result(sz, '\0');

    // Read the whole file into the buffer
    f.read(result.data(), sz);

    return result;
}

DefaultView::DefaultView()
    : wxFrame(nullptr, wxID_ANY, "OASIS")
{
    wxConfigBase* config = wxConfigBase::Get();

    wxFileSystem::AddHandler(new wxMemoryFSHandler);

    const auto& indexHTML = ReadFileIntoString("assets/index.html");
    wxMemoryFSHandler::AddFile("index.html", indexHTML);

    const auto& bootstrapJS = ReadFileIntoString("assets/bootstrap-5.3.3-dist/js/bootstrap.bundle.js");
    wxMemoryFSHandler::AddFile("bootstrap.bundle.js", bootstrapJS);

    const auto& bootstrapCSS = ReadFileIntoString("assets/bootstrap-5.3.3-dist/css/bootstrap.css");
    wxMemoryFSHandler::AddFile("bootstrap.css", bootstrapCSS);

    const auto& indexJS = ReadFileIntoString("assets/index.js");
    wxMemoryFSHandler::AddFile("index.js", indexJS);

    const auto& foxSVG = ReadFileIntoString("assets/Fox.svg");
    wxMemoryFSHandler::AddFile("Fox.svg", foxSVG);

    CreateStatusBar();
    SetStatusText("Welcome to OASIS!");

    const bool horizontalLayout = config->ReadBool("HorizontalLayout", false);
    auto* mainSizer = new wxBoxSizer(horizontalLayout ? wxHORIZONTAL : wxVERTICAL);

    auto* webView = wxWebView::New();

#ifdef __APPLE__
    webView->RegisterHandler(wxSharedPtr<wxWebViewHandler>(new wxWebViewFSHandler("memory")));
#endif

    webView->Create(this, wxID_ANY);

#ifndef __APPLE__
    webView->RegisterHandler(wxSharedPtr<wxWebViewHandler>(new wxWebViewFSHandler("memory")));
#endif

    webView->Bind(wxEVT_WEBVIEW_ERROR, [=](wxWebViewEvent& evt) {
        wxMessageBox("Error: " + evt.GetString(), "Error", wxICON_ERROR);
    });

#ifndef NDEBUG
    webView->EnableAccessToDevTools();
#endif

    viewer_.setWebView(webView);
    viewer_.Init();

    auto* inputSizer = new wxBoxSizer(wxVERTICAL);

    // Add a textfield
    auto* textFieldSizer = new wxBoxSizer(wxHORIZONTAL);
    auto* textFieldLabel = new wxStaticText(this, wxID_ANY, "Input:");
    auto* textField = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    textField->SetHint("E.g. 2x+3x");

    textFieldSizer->Add(textFieldLabel, wxSizerFlags().Center());
    textFieldSizer->AddSpacer(4);
    textFieldSizer->Add(textField, wxSizerFlags(1).Expand());

    auto* keypad = new wxGridBagSizer(4, 4);

    auto* keyClear = new KeypadButton(this, wxID_ANY, "Clear");
    auto* keyBackspace = new KeypadButton(this,wxID_ANY, "\u232B");
    auto* keyDDX = new KeypadButton(this, wxID_ANY, "d/dx");
    auto* keyInt = new KeypadButton(this, wxID_ANY, "\u222B");
    auto* keyLog = new KeypadButton(this, wxID_ANY, "log");
    auto* keyExp = new KeypadButton(this, wxID_ANY, "\u2227");
    auto* keyX = new KeypadButton(this, wxID_ANY, "\U0001D465");
    auto* keyi = new KeypadButton(this, wxID_ANY, "\U0001D456");
    auto* keyComma = new KeypadButton(this, wxID_ANY, ",");
    auto* keyLeftParens = new KeypadButton(this, wxID_ANY, "(");
    auto* keyRightParens = new KeypadButton(this, wxID_ANY, ")");
    auto* keyDivide = new KeypadButton(this, wxID_ANY, "\u00F7");
    auto* key7 = new KeypadButton(this, wxID_ANY, "7");
    auto* key8 = new KeypadButton(this, wxID_ANY, "8");
    auto* key9 = new KeypadButton(this, wxID_ANY, "9");
    auto* keyMultiply = new KeypadButton(this, wxID_ANY, "\u00D7");
    auto* key4 = new KeypadButton(this, wxID_ANY, "4");
    auto* key5 = new KeypadButton(this, wxID_ANY, "5");
    auto* key6 = new KeypadButton(this, wxID_ANY, "6");
    auto* keySubtract = new KeypadButton(this, wxID_ANY, "\u2212");
    auto* key1 = new KeypadButton(this, wxID_ANY, "1");
    auto* key2 = new KeypadButton(this, wxID_ANY, "2");
    auto* key3 = new KeypadButton(this, wxID_ANY, "3");
    auto* keyAdd = new KeypadButton(this, wxID_ANY, "+");
    auto* key0 = new KeypadButton(this, wxID_ANY, "0");
    auto* keyNegate = new KeypadButton(this, wxID_ANY, "(-)");
    auto* keyDot = new KeypadButton(this, wxID_ANY, ".");
    auto* keyEnter = new KeypadButton(this, wxID_ANY, "Enter");

    keypad->Add(keyClear, {0, 0}, wxDefaultSpan, wxSizerFlags().Expand().GetFlags());
    keypad->Add(keyBackspace, {0, 1}, wxDefaultSpan, wxSizerFlags().Expand().GetFlags());
    keypad->Add(keyDDX, {0, 2}, wxDefaultSpan, wxSizerFlags().Expand().GetFlags());
    keypad->Add(keyInt, {0, 3}, wxDefaultSpan, wxSizerFlags().Expand().GetFlags());

    keypad->Add(keyX, {1, 0}, wxDefaultSpan, wxSizerFlags().Expand().GetFlags());
    keypad->Add(keyi, {1, 1}, wxDefaultSpan, wxSizerFlags().Expand().GetFlags());
    keypad->Add(keyLog, {1, 2}, wxDefaultSpan, wxSizerFlags().Expand().GetFlags());
    keypad->Add(keyExp, {1, 3}, wxDefaultSpan, wxSizerFlags().Expand().GetFlags());

    keypad->Add(keyComma, {2, 0}, wxDefaultSpan, wxSizerFlags().Expand().GetFlags());
    keypad->Add(keyLeftParens, {2, 1}, wxDefaultSpan, wxSizerFlags().Expand().GetFlags());
    keypad->Add(keyRightParens, {2, 2}, wxDefaultSpan, wxSizerFlags().Expand().GetFlags());
    keypad->Add(keyDivide, {2, 3}, wxDefaultSpan, wxSizerFlags().Expand().GetFlags());

    keypad->Add(key7, {3, 0}, wxDefaultSpan, wxSizerFlags().Expand().GetFlags());
    keypad->Add(key8, {3, 1}, wxDefaultSpan, wxSizerFlags().Expand().GetFlags());
    keypad->Add(key9, {3, 2}, wxDefaultSpan, wxSizerFlags().Expand().GetFlags());
    keypad->Add(keyMultiply, {3, 3}, wxDefaultSpan, wxSizerFlags().Expand().GetFlags());

    keypad->Add(key4, {4, 0}, wxDefaultSpan, wxSizerFlags().Expand().GetFlags());
    keypad->Add(key5, {4, 1}, wxDefaultSpan, wxSizerFlags().Expand().GetFlags());
    keypad->Add(key6, {4, 2}, wxDefaultSpan, wxSizerFlags().Expand().GetFlags());
    keypad->Add(keySubtract, {4, 3}, wxDefaultSpan, wxSizerFlags().Expand().GetFlags());

    keypad->Add(key1, {5, 0}, wxDefaultSpan, wxSizerFlags().Expand().GetFlags());
    keypad->Add(key2, {5, 1}, wxDefaultSpan, wxSizerFlags().Expand().GetFlags());
    keypad->Add(key3, {5, 2}, wxDefaultSpan, wxSizerFlags().Expand().GetFlags());
    keypad->Add(keyAdd, {5, 3}, wxDefaultSpan, wxSizerFlags().Expand().GetFlags());

    keypad->Add(key0, {6, 0}, wxDefaultSpan, wxSizerFlags().Expand().GetFlags());
    keypad->Add(keyDot, {6, 1}, wxDefaultSpan, wxSizerFlags().Expand().GetFlags());
    keypad->Add(keyNegate, {6, 2}, wxDefaultSpan, wxSizerFlags().Expand().GetFlags());
    keypad->Add(keyEnter, {6, 3}, wxDefaultSpan, wxSizerFlags().Expand().GetFlags());

    // Add rows
    for (int i = 0; i < keypad->GetRows(); ++i) {
        keypad->AddGrowableRow(i);
    }

    // Add columns
    for (int i = 0; i < keypad->GetCols(); ++i) {
        keypad->AddGrowableCol(i);
    }

    inputSizer->AddSpacer(4);
    inputSizer->Add(textFieldSizer, wxSizerFlags().Expand());
    inputSizer->AddSpacer(4);
    inputSizer->Add(keypad, wxSizerFlags(1).Expand());
    inputSizer->AddSpacer(4);

    mainSizer->Add(webView, wxSizerFlags(1).Expand());
    mainSizer->Add(inputSizer, wxSizerFlags(1).Expand().HorzBorder());

    SetSizerAndFit(mainSizer);

    auto* menuFile = new wxMenu;
    menuFile->Append(wxID_PREFERENCES);

    auto* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    auto* menuFunctions = new wxMenu;
    const wxMenuItem* itemDerivative = menuFunctions->Append(wxID_ANY, "Derivative...");
    const wxMenuItem* itemIntegral = menuFunctions->Append(wxID_ANY, "Integral...");
    const wxMenuItem* itemLogarithm = menuFunctions->Append(wxID_ANY, "Logarithm...");

    auto* menuBar = new wxMenuBar;
    auto* menuView = new wxMenu;

    // Create a "Layout" submenu
    auto* layoutSubMenu = new wxMenu;

    // "Vertical" menu item
    auto* verticalMenuItem = layoutSubMenu->AppendRadioItem(wxID_ANY, "Vertical");

    // "Horizontal" menu item
    auto* horizontalMenuItem = layoutSubMenu->AppendRadioItem(wxID_ANY, "Horizontal");

    if (horizontalLayout) {
        horizontalMenuItem->Check(true);
    } else {
        verticalMenuItem->Check(true);
    }

    // Add the "Layout" submenu to the "View" menu
    menuView->AppendSubMenu(layoutSubMenu, "Layout");

    menuView->Append(wxID_ZOOM_IN, "Zoom In");
    menuView->Append(wxID_ZOOM_OUT, "Zoom Out");
    menuView->Append(wxID_ZOOM_100, "Reset Zoom");

    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuFunctions, "&Functions");
    menuBar->Append(menuView, "&View");
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);

    Bind(wxEVT_MENU, [=](wxCommandEvent&) {
        auto preferencesPanel = PreferencesPanel(this, wxID_ANY);
        if (preferencesPanel.ShowModal() != wxID_OK) {
            return;
        }
    });

    Bind(wxEVT_MENU, [=](wxCommandEvent& event) { wxMessageBox("https://github.com/matthew-mccall/Oasis", "Open Algebra Software for Inferring Solutions", wxICON_INFORMATION); }, wxID_ABOUT);
    Bind(wxEVT_MENU, [=](wxCommandEvent& event) { Close(true); }, wxID_EXIT);

    Bind(wxEVT_MENU, [=](wxCommandEvent& event)
    {
        auto derivativeBuilder = FunctionBuilder(this, wxID_ANY, "Derivative Builder", "dd", "Function", "Variable");
        if (derivativeBuilder.ShowModal() != wxID_OK) {
            return;
        }

        const auto composed_function = derivativeBuilder.getComposedFunction();
        textField->WriteText(composed_function);
    }, itemDerivative->GetId());

    Bind(wxEVT_MENU, [=](wxCommandEvent& event)
    {
        auto integralBuilder = FunctionBuilder(this, wxID_ANY, "Integral Builder", "in", "Function", "Variable");
        if (integralBuilder.ShowModal() != wxID_OK) {
            return;
        }

        const auto composed_function = integralBuilder.getComposedFunction();
        textField->WriteText(composed_function);
    }, itemIntegral->GetId());

    Bind(wxEVT_MENU, [=](wxCommandEvent& event)
    {
        auto logBuilder = FunctionBuilder(this, wxID_ANY, "Logarithm Builder", "log", "Base", "Argument");
        if (logBuilder.ShowModal() != wxID_OK) {
            return;
        }

        const auto composed_function = logBuilder.getComposedFunction();
        textField->WriteText(composed_function);
    }, itemLogarithm->GetId());


    Bind(wxEVT_MENU, [=](wxCommandEvent& event)
    {
        webView->SetZoomType(wxWEBVIEW_ZOOM_TYPE_LAYOUT);
        webView->SetZoomFactor(webView->GetZoomFactor() + 0.1f);
    }, wxID_ZOOM_IN);

    Bind(wxEVT_MENU, [=](wxCommandEvent& event)
    {
        webView->SetZoomType(wxWEBVIEW_ZOOM_TYPE_LAYOUT);
        webView->SetZoomFactor(webView->GetZoomFactor() - 0.1f);
    }, wxID_ZOOM_OUT);

    Bind(wxEVT_MENU, [=](wxCommandEvent& event)
    {
        webView->SetZoomType(wxWEBVIEW_ZOOM_TYPE_LAYOUT);
        webView->SetZoom(wxWEBVIEW_ZOOM_MEDIUM);
    }, wxID_ZOOM_100);


    // Bind events to the menu items
    Bind(wxEVT_MENU, [=](wxCommandEvent& event)
    {
        // Use VERTICAL layout
        mainSizer->SetOrientation(wxVERTICAL);
        verticalMenuItem->Check(true);
        config->Write("HorizontalLayout", false);
        Layout();
    }, verticalMenuItem->GetId());

    Bind(wxEVT_MENU, [=](wxCommandEvent& event)
    {
        // Use HORIZONTAL layout
        mainSizer->SetOrientation(wxHORIZONTAL);
        horizontalMenuItem->Check(true);
        config->Write("HorizontalLayout", true);
        Layout();
    }, horizontalMenuItem->GetId());

    textField->Bind(wxEVT_TEXT, [this](wxCommandEvent& evt) {
        if (const auto textCtrl = dynamic_cast<wxTextCtrl*>(evt.GetEventObject())) {
            const std::string currentInput = textCtrl->GetValue().ToStdString();

            if (currentInput.empty()) {
                return;
            }

            const std::string infix = preprocessInput(currentInput);

            try {
                currentExpression = Oasis::FromInFix(infix);
            } catch (std::exception& e) {
                SetStatusText(e.what());
                return;
            }

            const std::string currentMathML = ExpressionToMathMLStr(currentExpression);
            viewer_.setCurrentEntry(currentMathML);

            SetStatusText("Successfully parsed input.");
        }
    });

    textField->Bind(wxEVT_TEXT_ENTER, [this, textField](wxCommandEvent& evt) { onEnter(viewer_, textField); });

    keyClear->Bind(wxEVT_LEFT_UP, [this, textField](wxMouseEvent& evt) {
        textField->SetValue("");
    });

    keyDDX->Bind(wxEVT_LEFT_UP, [this, textField](wxMouseEvent& evt)
    {
        const long insertion_point = textField->GetInsertionPoint();
        textField->WriteText("dd(,)");
        textField->SetInsertionPoint(insertion_point + 3);
    });

    keyInt->Bind(wxEVT_LEFT_UP, [this, textField](wxMouseEvent& evt)
    {
        const long insertion_point = textField->GetInsertionPoint();
        textField->WriteText("in(,)");
        textField->SetInsertionPoint(insertion_point + 3);
    });

    keyX->Bind(wxEVT_LEFT_UP, [this, textField](wxMouseEvent& evt)
    {
        textField->WriteText("x");
    });

    keyi->Bind(wxEVT_LEFT_UP, [this, textField](wxMouseEvent& evt)
    {
        textField->WriteText("i");
    });

    keyBackspace->Bind(wxEVT_LEFT_UP, [this, textField](wxMouseEvent& evt)
    {
        if (textField->IsEditable() && textField->GetInsertionPoint() > 0) {
            long position = textField->GetInsertionPoint();
            textField->Remove(position - 1, position);
        }
    });

    keyLog->Bind(wxEVT_LEFT_UP, [this, textField](wxMouseEvent& evt)
    {
        textField->WriteText("log()");
        textField->SetInsertionPoint(textField->GetInsertionPoint() - 1);
    });

    keyDivide->Bind(wxEVT_LEFT_UP, [this, textField](wxMouseEvent& evt) {
        textField->WriteText("/");
    });

    for (const auto key : { keyComma, keyLeftParens, keyRightParens, key7, key8, key9, key4, key5, key6, key1, key2, key3, key0, keyAdd, keyDot }) {
        key->Bind(wxEVT_LEFT_UP, [this, textField](wxMouseEvent& evt) {
            if (const auto button = dynamic_cast<KeypadButton*>(evt.GetEventObject())) {
                textField->WriteText(button->getText());
            }
        });
    }

    keySubtract->Bind(wxEVT_LEFT_UP, [this, textField](wxMouseEvent& evt) {
        textField->WriteText("-");
    });

    keyNegate->Bind(wxEVT_LEFT_UP, [this, textField](wxMouseEvent& evt) {
        wxString currentText = textField->GetValue();
        long cursorPos = textField->GetInsertionPoint();
        if (currentText.StartsWith("-"))
        {
            textField->SetValue(currentText.Mid(1)); // If it's already negative, remove the "-"
            textField->SetInsertionPoint(cursorPos-1);
        }
        else
        {
            textField->SetValue("-"+currentText); // If it's positive, add a "-" at the beginning
            textField->SetInsertionPoint(cursorPos+1);
        }
    });

    keyMultiply->Bind(wxEVT_LEFT_UP, [this, textField](wxMouseEvent& evt) {
        textField->WriteText("*");
    });

    keyExp->Bind(wxEVT_LEFT_UP, [this, textField](wxMouseEvent& evt) {
        textField->WriteText("^");
    });

    keyEnter->Bind(wxEVT_LEFT_UP, [this, textField](wxMouseEvent& evt) { onEnter(viewer_, textField); });
}

void DefaultView::onEnter(const EquationViewer& equationViewer, wxTextCtrl* textCtrl)
{
    if (currentExpression == nullptr) {
        return;
    }

    const auto result = currentExpression->Simplify();
    const std::string queryMathML = ExpressionToMathMLStr(currentExpression);
    const std::string responseMathML = ExpressionToMathMLStr(result);

    equationViewer.addEntryToHistory(queryMathML, responseMathML);
    equationViewer.setCurrentEntry("");

    textCtrl->SetValue("");
}
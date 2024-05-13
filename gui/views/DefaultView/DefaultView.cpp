//
// Created by Matthew McCall on 2/16/24.
//

#include <fstream>
#include <vector>

#include "fmt/core.h"

#include "tinyxml2.h"

#include <wx/fs_mem.h>
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
    wxFileSystem::AddHandler(new wxMemoryFSHandler);

    const auto& indexHTML = ReadFileIntoString("assets/index.html");
    wxMemoryFSHandler::AddFile("index.html", indexHTML);

    const auto& bootstrapJS = ReadFileIntoString("assets/bootstrap-5.3.3-dist/js/bootstrap.bundle.js");
    wxMemoryFSHandler::AddFile("bootstrap.bundle.js", bootstrapJS);

    const auto& bootstrapCSS = ReadFileIntoString("assets/bootstrap-5.3.3-dist/css/bootstrap.css");
    wxMemoryFSHandler::AddFile("bootstrap.css", bootstrapCSS);

    const auto& indexJS = ReadFileIntoString("assets/index.js");
    wxMemoryFSHandler::AddFile("index.js", indexJS);

    CreateStatusBar();
    SetStatusText("Welcome to OASIS!");

    auto* mainSizer = new wxBoxSizer(wxVERTICAL);

    auto* webView = wxWebView::New();

#ifdef __APPLE__
    webView->RegisterHandler(wxSharedPtr<wxWebViewHandler>(new wxWebViewFSHandler("memory")));
#endif

    webView->Create(this, wxID_ANY, "memory:index.html");

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

    auto* toolbarSizer = new wxBoxSizer(wxHORIZONTAL);
    auto* derivativeButton = new wxButton(this, wxID_ANY, "d/dx");
    auto* logarithmButton = new wxButton(this, wxID_ANY, "log");
    // auto* sinButton = new wxButton(this, wxID_ANY, "sin");
    // auto* cosButton = new wxButton(this, wxID_ANY, "cos");
    // auto* tanButton = new wxButton(this, wxID_ANY, "tan");

    toolbarSizer->Add(derivativeButton);
    toolbarSizer->AddSpacer(4);
    toolbarSizer->Add(logarithmButton);
    // toolbarSizer->AddSpacer(4);
    // toolbarSizer->Add(sinButton);
    // toolbarSizer->AddSpacer(4);
    // toolbarSizer->Add(cosButton);
    // toolbarSizer->AddSpacer(4);
    // toolbarSizer->Add(tanButton);

    // Add a textfield
    auto* textFieldSizer = new wxBoxSizer(wxHORIZONTAL);
    auto* textFieldLabel = new wxStaticText(this, wxID_ANY, "Input:");
    auto* textField = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    textField->SetHint("E.g. 2x+3x");

    textFieldSizer->Add(textFieldLabel, wxSizerFlags().Center());
    textFieldSizer->AddSpacer(4);
    textFieldSizer->Add(textField, wxSizerFlags(1).Expand());

    auto* keypad = new wxGridSizer(5, 4, 4, 4);

    auto* keyClear = new KeypadButton(this, wxID_ANY, "Clear");
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

    inputSizer->AddSpacer(4);
    inputSizer->Add(toolbarSizer, wxSizerFlags());
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
    const wxMenuItem* itemLogarithm = menuFunctions->Append(wxID_ANY, "Logarithm...");

    auto* menuBar = new wxMenuBar;
    auto* menuView = new wxMenu;

    // Create a "Layout" submenu
    auto* layoutSubMenu = new wxMenu;

    // "Vertical" menu item
    auto* verticalMenuItem = layoutSubMenu->AppendRadioItem(wxID_ANY, "Vertical");
    verticalMenuItem->Check(true); // Make this the default option

    // "Horizontal" menu item
    auto* horizontalMenuItem = layoutSubMenu->AppendRadioItem(wxID_ANY, "Horizontal");

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

        currentInput += derivativeBuilder.getComposedFunction();
        textField->SetValue(currentInput);
        textField->SetInsertionPointEnd();
    }, itemDerivative->GetId());

    Bind(wxEVT_MENU, [=](wxCommandEvent& event)
    {
        auto logBuilder = FunctionBuilder(this, wxID_ANY, "Logarithm Builder", "log", "Base", "Argument");
        if (logBuilder.ShowModal() != wxID_OK) {
            return;
        }

        currentInput += logBuilder.getComposedFunction();
        textField->SetValue(currentInput);
        textField->SetInsertionPointEnd();
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

        // Save the current window size
        wxSize currentSize = this->GetSize();

        // Swap the width and height values of the window size
        currentSize.Set(currentSize.GetHeight(), currentSize.GetWidth());

        // Set the new size to the window
        this->SetSize(currentSize);

        Fit();
        // Layout();
    }, verticalMenuItem->GetId());

    Bind(wxEVT_MENU, [=](wxCommandEvent& event)
    {
        // Use HORIZONTAL layout
        mainSizer->SetOrientation(wxHORIZONTAL);
        horizontalMenuItem->Check(true);

        // Save the current window size
        wxSize currentSize = this->GetSize();

        // Swap the width and height values of the window size
        currentSize.Set(currentSize.GetHeight(), currentSize.GetWidth());

        // Set the new size to the window
        this->SetSize(currentSize);

        Fit();
        // Layout();
    }, horizontalMenuItem->GetId());

    Bind(wxEVT_MOTION, [=](wxMouseEvent& evt) {
        keyClear->paintNow();
        keyLeftParens->paintNow();
        keyRightParens->paintNow();
        keyDivide->paintNow();
        key7->paintNow();
        key8->paintNow();
        key9->paintNow();
        key4->paintNow();
        key5->paintNow();
        key6->paintNow();
        key1->paintNow();
        key2->paintNow();
        key3->paintNow();
        key0->paintNow();
        keySubtract->paintNow();
        keyAdd->paintNow();
        keyNegate->paintNow();
        keyDot->paintNow();
        keyMultiply->paintNow();
        keyEnter->paintNow();
    });

    derivativeButton->Bind(wxEVT_BUTTON, [this, textField](wxCommandEvent& evt)
    {
        currentInput += "dd(";
        textField->SetValue(currentInput);
        textField->SetInsertionPointEnd();
    });

    logarithmButton->Bind(wxEVT_BUTTON, [this, textField](wxCommandEvent& evt)
    {
        currentInput += "log(";
        textField->SetValue(currentInput);
        textField->SetInsertionPointEnd();
    });

    textField->Bind(wxEVT_TEXT, [this](wxCommandEvent& evt) {
        if (const auto textCtrl = dynamic_cast<wxTextCtrl*>(evt.GetEventObject())) {
            currentInput = textCtrl->GetValue().ToStdString();
            const std::string infix = preprocessInput(currentInput);

            if (currentInput.empty()) {
                return;
            }

            try {
                currentExpression = Oasis::FromInFix(infix);
            } catch (std::exception& e) {
                SetStatusText(e.what());
                return;
            }

            lastReloadReason = LastReloadReason::OnInputChanged;

            const std::string currentMathML = ExpressionToMathMLStr(currentExpression);
            viewer_.setCurrentEntry(currentMathML);

            SetStatusText("Successfully parsed input.");
        }
    });

    textField->Bind(wxEVT_TEXT_ENTER, [this, textField](wxCommandEvent& evt) { onEnter(viewer_, textField); });

    keyClear->Bind(wxEVT_LEFT_UP, [this, textField](wxMouseEvent& evt) {
        currentInput.clear();
        textField->SetValue(currentInput);
    });

    keyLeftParens->Bind(wxEVT_LEFT_UP, [this, textField](wxMouseEvent& evt) {
        currentInput += "(";
        textField->SetValue(currentInput);
    });

    keyRightParens->Bind(wxEVT_LEFT_UP, [this, textField](wxMouseEvent& evt) {
        currentInput += ")";
        textField->SetValue(currentInput);
    });

    keyDivide->Bind(wxEVT_LEFT_UP, [this, textField](wxMouseEvent& evt) {
        currentInput += "/";
        textField->SetValue(currentInput);
    });

    for (const auto key : { key7, key8, key9, key4, key5, key6, key1, key2, key3, key0 }) {
        key->Bind(wxEVT_LEFT_UP, [this, textField](wxMouseEvent& evt) {
            if (auto button = dynamic_cast<KeypadButton*>(evt.GetEventObject())) {
                currentInput += button->getText();
                textField->SetValue(currentInput);
            }
        });
    }

    keySubtract->Bind(wxEVT_LEFT_UP, [this, textField](wxMouseEvent& evt) {
        currentInput += "-";
        textField->SetValue(currentInput);
    });

    keyAdd->Bind(wxEVT_LEFT_UP, [this, textField](wxMouseEvent& evt) {
        currentInput += "+";
        textField->SetValue(currentInput);
    });

    keyNegate->Bind(wxEVT_LEFT_UP, [this, textField](wxMouseEvent& evt) {
        currentInput = "-" + currentInput;
        textField->SetValue(currentInput);
    });

    keyDot->Bind(wxEVT_LEFT_UP, [this, textField](wxMouseEvent& evt) {
        currentInput += ".";
        textField->SetValue(currentInput);
    });

    keyMultiply->Bind(wxEVT_LEFT_UP, [this, textField](wxMouseEvent& evt) {
        currentInput += "*";
        textField->SetValue(currentInput);
    });

    keyEnter->Bind(wxEVT_LEFT_UP, [this, textField](wxMouseEvent& evt) { onEnter(viewer_, textField); });
}

void DefaultView::onEnter(const EquationViewer& equationViewer, wxTextCtrl* textCtrl)
{
    const auto result = currentExpression->Simplify();
    const std::string queryMathML = ExpressionToMathMLStr(currentExpression);
    const std::string responseMathML = ExpressionToMathMLStr(result);

    equationViewer.addEntryToHistory(queryMathML, responseMathML);
    equationViewer.setCurrentEntry("");

    // (Assuming currentInput holds a valid mathematical expression)
    currentInput.clear();
    textCtrl->SetValue(currentInput);
}
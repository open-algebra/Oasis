//
// Created by Matthew McCall on 2/16/24.
//

#include <wx/fs_mem.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/webview.h>
#include <wx/webviewfshandler.h>

#include <fmt/core.h>

#include "Oasis/FromString.hpp"

#include "../InputPreprocessor.hpp"
#include "../components/KeypadButton/KeypadButton.hpp"
#include "../components/FunctionBuilder/FunctionBuilder.hpp"
#include "DefaultView.hpp"

#include <Fox.svg.hpp>
#include <bootstrap.bundle.min.js.hpp>
#include <bootstrap.min.css.hpp>

DefaultView::DefaultView()
    : wxFrame(nullptr, wxID_ANY, "OASIS")
{
    wxFileSystem::AddHandler(new wxMemoryFSHandler);

    const auto& foxSvg = Fox_svg::get();
    wxMemoryFSHandler::AddFile("Fox.svg", foxSvg.data(), foxSvg.size());

    const auto& bootstrapCss = bootstrap_min_css::get();
    wxMemoryFSHandler::AddFile("bootstrap.min.css", bootstrapCss.data(), bootstrapCss.size());

    const auto& bootstrapJs = bootstrap_bundle_min_js::get();
    wxMemoryFSHandler::AddFile("bootstrap.bundle.min.js", bootstrapJs.data(), bootstrapJs.size());

    CreateStatusBar();
    SetStatusText("Welcome to OASIS!");

    auto* mainSizer = new wxBoxSizer(wxVERTICAL);

    auto* webView = wxWebView::New();

#ifdef __APPLE__
    webView->RegisterHandler(wxSharedPtr<wxWebViewHandler>(new wxWebViewFSHandler("memory")));
#endif

    webView->Create(this, wxID_ANY);

#ifndef __APPLE__
    webView->RegisterHandler(wxSharedPtr<wxWebViewHandler>(new wxWebViewFSHandler("memory")));
#endif

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

    mainSizer->Add(webView, wxSizerFlags(1).Expand().Border(wxDOWN));
    mainSizer->Add(toolbarSizer, wxSizerFlags().Border(wxLEFT | wxRIGHT | wxDOWN));
    mainSizer->Add(textFieldSizer, wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT | wxDOWN));
    mainSizer->Add(keypad, wxSizerFlags(1).Expand().Border(wxLEFT | wxRIGHT | wxDOWN));

    SetSizerAndFit(mainSizer);

    auto* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    auto* menuFunctions = new wxMenu;
    wxMenuItem* itemDerivative = menuFunctions->Append(wxID_ANY, "Derivative...");
    wxMenuItem* itemLogarithm = menuFunctions->Append(wxID_ANY, "Logarithm...");

    auto* menuBar = new wxMenuBar;
    auto* menuView = new wxMenu;

    menuView->Append(wxID_ZOOM_IN, "Zoom In");
    menuView->Append(wxID_ZOOM_OUT, "Zoom Out");
    menuView->Append(wxID_ZOOM_100, "Reset Zoom");

    menuBar->Append(menuFunctions, "&Functions");
    menuBar->Append(menuView, "&View");
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);

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

    textField->Bind(wxEVT_TEXT, [this, webView](wxCommandEvent& evt) {
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
            renderPage(webView);
            SetStatusText("Successfully parsed input.");
        }
    });

    textField->Bind(wxEVT_TEXT_ENTER, [this, textField, webView](wxCommandEvent& evt) { onEnter(webView, textField); });

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

    keyEnter->Bind(wxEVT_LEFT_UP, [this, textField, webView](wxMouseEvent& evt) { onEnter(webView, textField); });

    // Bind the event
    webView->Bind(wxEVT_WEBVIEW_LOADED, [this, webView](wxWebViewEvent& event) {
        if (lastReloadReason == LastReloadReason::ThemeChanged) {
            webView->RunScriptAsync(fmt::format(R"(
window.scrollTo(0,{});
            )", lastScrollHeight));
            return;
        }

        webView->RunScriptAsync(R"(
document.body.style.overflow = 'hidden';
document.getElementById('current').scrollIntoView({behavior: 'instant'});
setTimeout(function(){document.body.style.overflow = 'auto';}, 0);)");
    });

    // Create root element.
    tinyxml2::XMLElement* root = doc.NewElement("html");
    doc.InsertFirstChild(root);

    // Add head element.
    tinyxml2::XMLElement* head = doc.NewElement("head");

    // Add Bootstrap CSS link to the head element
    tinyxml2::XMLElement* bootstrapCssLink = doc.NewElement("link");

    bootstrapCssLink->SetAttribute("rel", "stylesheet");
    bootstrapCssLink->SetAttribute("href", "memory:bootstrap.min.css");

    head->InsertEndChild(bootstrapCssLink);

    Bind(wxEVT_SYS_COLOUR_CHANGED, [this, root, webView](wxSysColourChangedEvent&) {
        this->lastReloadReason = LastReloadReason::ThemeChanged;

        if (wxSystemAppearance appearance = wxSystemSettings::GetAppearance(); appearance.IsDark())
        {
            root->SetAttribute("data-bs-theme", "dark");
        }
        else
        {
            root->SetAttribute("data-bs-theme", "light");
        }

        renderPage(webView);
    });

    // Add style element to the head element
    root->InsertEndChild(head);

    // Add body element.
    body = doc.NewElement("body");

    currentDivWrapper = doc.NewElement("div");
    currentDivWrapper->SetAttribute("class", "d-flex justify-content-end mx-2"); // Set div as flexbox to align items with padding and shadow

    currentDiv = doc.NewElement("div");
    currentDiv->SetAttribute("class", "d-inline-flex p-2 text-bg-primary rounded border shadow"); // Set div as inline-flex to make it takes as much width as necessary with padding and shadow
    currentDiv->SetAttribute("id", "current");
    currentDivWrapper->InsertEndChild(currentDiv);

    body->InsertEndChild(currentDivWrapper);

    // Add Bootstrap JS script to the body element
    tinyxml2::XMLElement* bootstrapJsScript = doc.NewElement("script");
    bootstrapJsScript->SetAttribute("src", "memory:bootstrap.bundle.min.js");

    body->InsertEndChild(bootstrapJsScript);

    root->InsertEndChild(body);

    renderPage(webView);
}

void DefaultView::onEnter(wxWebView* webView, wxTextCtrl* textCtrl)
{
    // Perform calculation or other actions based on currentInput and then clear currentInput
    if (!currentExpression) {
        SetStatusText("No expression to evaluate!");
        return;
    }

    auto& [query, response] = history.emplace_back(std::move(currentExpression), currentExpression->Simplify());

    tinyxml2::XMLElement* queryRightAlign = doc.NewElement("div");
    queryRightAlign->SetAttribute("class", "d-flex justify-content-end mb-2 mx-2"); // Set div as flexbox to align items with padding and shadow

    tinyxml2::XMLElement* queryCard = doc.NewElement("div");
    queryCard->SetAttribute("class", "d-inline-flex p-2 text-bg-primary rounded border shadow"); // Set div as inline-flex to make it takes as much width as necessary with padding and shadow

    tinyxml2::XMLElement* queryMathML = query->ToMathMLElement(doc);
    tinyxml2::XMLElement* queryMath = doc.NewElement("math");

    queryMath->InsertFirstChild(queryMathML);
    queryCard->InsertEndChild(queryMath);

    queryRightAlign->InsertEndChild(queryCard);

    if (currentDivWrapper->PreviousSibling()) {
        body->InsertAfterChild(currentDivWrapper->PreviousSibling(), queryRightAlign);
    } else {
        queryRightAlign->SetAttribute("class", "d-flex justify-content-end m-2"); // Set div as flexbox to align items with padding and shadow
        body->InsertFirstChild(queryRightAlign);
    }

    tinyxml2::XMLElement* responseLeftAlign = doc.NewElement("div");
    responseLeftAlign->SetAttribute("class", "d-flex justify-content-begin align-items-center mb-2 mx-2");

    tinyxml2::XMLElement* responseCard = doc.NewElement("div");
    responseCard->SetAttribute("class", "d-inline-flex p-2 bg-text-light rounded border shadow mx-2");

    tinyxml2::XMLElement* responseAvatar = doc.NewElement("img");
    responseAvatar->SetAttribute("src", "memory:Fox.svg");
    responseAvatar->SetAttribute("class", "mr-3");
    responseAvatar->SetAttribute("width", "30");
    responseAvatar->SetAttribute("height", "30");
    responseLeftAlign->InsertEndChild(responseAvatar);

    if (response == nullptr) {
        tinyxml2::XMLText* errorText = doc.NewText("Error");
        responseCard->InsertEndChild(errorText);
    } else {
        tinyxml2::XMLElement* responseMathML = response->ToMathMLElement(doc);
        tinyxml2::XMLElement* responseMath = doc.NewElement("math");
        responseMath->InsertFirstChild(responseMathML);
        responseCard->InsertEndChild(responseMath);
    }

    responseLeftAlign->InsertEndChild(responseCard);

    body->InsertAfterChild(queryRightAlign, responseLeftAlign);

    lastReloadReason = LastReloadReason::OnEnter;
    renderPage(webView);

    // (Assuming currentInput holds a valid mathematical expression)
    currentInput.clear();
    textCtrl->SetValue(currentInput);
}

void DefaultView::renderPage(wxWebView* webView)
{
    wxString scrollHeightStr;
    webView->RunScript("window.scrollY", &scrollHeightStr);
    lastScrollHeight = std::stoi(scrollHeightStr.ToStdString());

    currentDiv->DeleteChildren();

    // Current expression to MathML.
    if (currentExpression) {
        // Creating new expressionElement
        tinyxml2::XMLElement* mathElement = doc.NewElement("math");
        tinyxml2::XMLElement* expressionElement = currentExpression->ToMathMLElement(doc);

        mathElement->InsertEndChild(expressionElement);

        // Adding new expressionElement to the currentDiv
        currentDiv->InsertEndChild(mathElement);
    }

    // Print the XML document to a string.
    tinyxml2::XMLPrinter printer;
    doc.Print(&printer);

    const std::string html_document = printer.CStr();
    webView->SetPage(html_document, "");
}
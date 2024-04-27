//
// Created by Matthew McCall on 2/16/24.
//

#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/webview.h>

#include <fmt/core.h>

#include "Oasis/FromString.hpp"

#include "../components/KeypadButton/KeypadButton.hpp"
#include "DefaultView.hpp"

namespace {

std::string convertToInfix(const std::string& input)
{
    std::string result;
    std::string operators = "+-*/^(),";

    for (char ch : input) {
        if (std::ranges::find(operators, ch) != operators.end()) {
            result += ' ';
            result += ch;
            result += ' ';
        } else {
            result += ch;
        }
    }
    return result;
}

}

void DefaultView::setStyles(tinyxml2::XMLElement* style)
{
    style->DeleteChildren();

    tinyxml2::XMLText* cssText = doc.NewText(R"css(
    div { padding: 0.5rem 0; }
    .query { border-bottom: 1px dotted; text-align: right; }
    .response { border-bottom: 1px dotted; }
    #current { text-align: right; }
)css");

    tinyxml2::XMLText* themeText;

    if (wxSystemAppearance appearance = wxSystemSettings::GetAppearance(); appearance.IsDark()) {
        themeText = doc.NewText(R"css(
        body { background-color: black; color: white; }
    )css");
    } else {
        themeText = doc.NewText(R"css(
        body { background-color: white; color: black; }
    )css");
    }

    // Add CSS text to the style element
    style->InsertEndChild(cssText);
    style->InsertEndChild(themeText);
}
DefaultView::DefaultView()
    : wxFrame(nullptr, wxID_ANY, "OASIS")
{
    CreateStatusBar();
    SetStatusText("Welcome to OASIS!");

    auto* mainSizer = new wxBoxSizer(wxVERTICAL);

    auto* webView = wxWebView::New(this, wxID_ANY);

    auto* toolbarSizer = new wxBoxSizer(wxHORIZONTAL);
    auto* derivativeButton = new wxButton(this, wxID_ANY, "d/dx");
    auto* logarithmButton = new wxButton(this, wxID_ANY, "log");
    auto* sinButton = new wxButton(this, wxID_ANY, "sin");
    auto* cosButton = new wxButton(this, wxID_ANY, "cos");
    auto* tanButton = new wxButton(this, wxID_ANY, "tan");

    toolbarSizer->Add(derivativeButton);
    toolbarSizer->AddSpacer(4);
    toolbarSizer->Add(logarithmButton);
    toolbarSizer->AddSpacer(4);
    toolbarSizer->Add(sinButton);
    toolbarSizer->AddSpacer(4);
    toolbarSizer->Add(cosButton);
    toolbarSizer->AddSpacer(4);
    toolbarSizer->Add(tanButton);

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

    auto* menuBar = new wxMenuBar;
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);

    Bind(wxEVT_MENU, [=](wxCommandEvent& event) { wxMessageBox("Oasis GUI"); }, wxID_ABOUT);

    Bind(wxEVT_MENU, [=](wxCommandEvent& event) { Close(true); }, wxID_EXIT);

    textField->Bind(wxEVT_TEXT, [this, webView](wxCommandEvent& evt) {
        if (const auto textCtrl = dynamic_cast<wxTextCtrl*>(evt.GetEventObject())) {
            currentInput = textCtrl->GetValue().ToStdString();
            const std::string infix = convertToInfix(currentInput);

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
document.getElementById('current').scrollIntoView();
setTimeout(function(){document.body.style.overflow = 'auto';}, 500);)");
    });

    // Create root element.
    tinyxml2::XMLElement* root = doc.NewElement("html");
    doc.InsertFirstChild(root);

    // Add head element.
    tinyxml2::XMLElement* head = doc.NewElement("head");

    // Create stylesheet element
    tinyxml2::XMLElement* style = doc.NewElement("style");
    setStyles(style);

    Bind(wxEVT_SYS_COLOUR_CHANGED, [this, style, webView](wxSysColourChangedEvent&) {
        setStyles(style);
        this->lastReloadReason = LastReloadReason::ThemeChanged;
        renderPage(webView);
    });

    // Add style element to the head element
    head->InsertEndChild(style);
    root->InsertEndChild(head);

    // Add body element.
    body = doc.NewElement("body");

    currentDiv = doc.NewElement("div");
    currentDiv->SetAttribute("id", "current");
    body->InsertEndChild(currentDiv);

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

    tinyxml2::XMLElement* queryDiv = doc.NewElement("div");
    queryDiv->SetAttribute("class", "query");

    tinyxml2::XMLElement* queryMathML = query->ToMathMLElement(doc);

    tinyxml2::XMLElement* queryMath = doc.NewElement("math");
    queryMath->InsertFirstChild(queryMathML);

    queryDiv->InsertEndChild(queryMath);

    if (currentDiv->PreviousSibling()) {
        body->InsertAfterChild(currentDiv->PreviousSibling(), queryDiv);
    } else {
        body->InsertFirstChild(queryDiv);
    }


    tinyxml2::XMLElement* responseDiv = doc.NewElement("div");
    responseDiv->SetAttribute("class", "response");

    if (response == nullptr) {
        tinyxml2::XMLText* errorText = doc.NewText("Error");
        responseDiv->InsertEndChild(errorText);
    } else {
        tinyxml2::XMLElement* responseMathML = response->ToMathMLElement(doc);
        tinyxml2::XMLElement* responseMath = doc.NewElement("math");
        responseMath->InsertFirstChild(responseMathML);
        responseDiv->InsertEndChild(responseMath);
    }

    body->InsertAfterChild(queryDiv, responseDiv);

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
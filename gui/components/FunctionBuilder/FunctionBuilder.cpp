//
// Created by Matthew McCall on 4/27/24.
//

#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/webview.h>

#include "FunctionBuilder.hpp"

#include "../../InputPreprocessor.hpp"
#include "Oasis/Expression.hpp"
#include "Oasis/FromString.hpp"

#include <fmt/format.h>
#include <tinyxml2.h>

std::string updatePreview(wxWebView* webView, wxTextCtrl* firstArgInput, wxTextCtrl* secondArgInput, const std::string& function)
{
    auto input = fmt::format("{}({},{})", function, firstArgInput->GetValue().ToStdString(), secondArgInput->GetValue().ToStdString());
    auto preprocessedInput = preprocessInput(input);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLDeclaration* decl = doc.NewDeclaration(R"(xml version="1.0" encoding="UTF-8")");
    doc.LinkEndChild(decl);

    tinyxml2::XMLElement* html = doc.NewElement("html");
    doc.InsertEndChild(html);

    tinyxml2::XMLElement* head = doc.NewElement("head");
    html->InsertEndChild(head);

    tinyxml2::XMLElement* body = doc.NewElement("body");
    html->InsertEndChild(body);

    tinyxml2::XMLElement* div = doc.NewElement("div");
    div->SetAttribute("style", "display: flex; justify-content: center; align-items: center; height: 100vh");
    body->InsertEndChild(div);

    try {
        auto expression = Oasis::FromInFix(preprocessedInput);
        tinyxml2::XMLElement* expressionElement = expression->ToMathMLElement(doc);

        tinyxml2::XMLElement* math = doc.NewElement("math");
        div->InsertEndChild(math);

        math->InsertEndChild(expressionElement);
    } catch (std::exception& e) {
        tinyxml2::XMLElement* err = doc.NewElement("p");
        err->SetText(e.what());
        div->InsertEndChild(err);
    }

    tinyxml2::XMLPrinter printer;
    doc.Print(&printer);

    const std::string html_document = printer.CStr();
    webView->SetPage(html_document, "");

    return input;
}

FunctionBuilder::FunctionBuilder(wxWindow* parent, wxWindowID id, const wxString& title, const std::string& function, const std::string& firstArgPrompt, const std::string& secondArgPrompt)
    : wxDialog(parent, id, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // wxWebView
    wxWebView* webView = wxWebView::New(this, wxID_ANY);
    sizer->Add(webView, wxSizerFlags(3).Expand().Border(wxDOWN));

    // First label and text control
    wxStaticText* firstArgLabel = new wxStaticText(this, wxID_ANY, firstArgPrompt);
    wxTextCtrl* firstArgInput = new wxTextCtrl(this, wxID_ANY);
    wxBoxSizer* firstInputSizer = new wxBoxSizer(wxHORIZONTAL);
    firstInputSizer->Add(firstArgLabel, wxSizerFlags(0).Center());
    firstInputSizer->AddSpacer(4);
    firstInputSizer->Add(firstArgInput, wxSizerFlags(1).Expand());
    sizer->Add(firstInputSizer, wxSizerFlags(0).Expand().Border(wxDOWN | wxLEFT | wxRIGHT));

    // Second label and text control
    wxStaticText* secondArgLabel = new wxStaticText(this, wxID_ANY, secondArgPrompt);
    wxTextCtrl* secondArgInput = new wxTextCtrl(this, wxID_ANY);
    wxBoxSizer* secondInputSizer = new wxBoxSizer(wxHORIZONTAL);
    secondInputSizer->Add(secondArgLabel, wxSizerFlags(0).Center());
    secondInputSizer->AddSpacer(4);
    secondInputSizer->Add(secondArgInput, wxSizerFlags(1).Expand());
    sizer->Add(secondInputSizer, wxSizerFlags(0).Expand().Border(wxDOWN | wxLEFT | wxRIGHT));

    // Create buttons
    wxButton* cancelButton = new wxButton(this, wxID_CANCEL, wxT("Cancel"));
    wxButton* insertButton = new wxButton(this, wxID_OK, wxT("Insert"));
    insertButton->SetDefault();

    // Create sizer for buttons
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->Add(cancelButton, wxSizerFlags(0));
    buttonSizer->AddSpacer(4);
    buttonSizer->Add(insertButton, wxSizerFlags(0));

    // Add buttons sizer to the main sizer
    sizer->Add(buttonSizer, wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxDOWN | wxLEFT | wxRIGHT));

    firstArgInput->Bind(wxEVT_TEXT, [=](wxCommandEvent&) {
        composedFunction = updatePreview(webView, firstArgInput, secondArgInput, function);
    });

    secondArgInput->Bind(wxEVT_TEXT, [=](wxCommandEvent&) {
        composedFunction = updatePreview(webView, firstArgInput, secondArgInput, function);
    });

    SetSizerAndFit(sizer);
}

std::string FunctionBuilder::getComposedFunction() const
{
    return composedFunction;
}

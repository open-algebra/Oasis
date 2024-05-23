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
#include "Oasis/MathMLSerializer.hpp"

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

        Oasis::MathMLSerializer mathmlSerializer { doc };
        expression->Serialize(mathmlSerializer);
        tinyxml2::XMLElement* expressionElement = mathmlSerializer.GetResult();

        tinyxml2::XMLElement* math = doc.NewElement("math");
        math->SetAttribute("display", "block");
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
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    // wxWebView
    wxWebView* webView = wxWebView::New(this, wxID_ANY);

    const auto flex_grid_sizer = new wxFlexGridSizer(2, 2, 4, 4);

    // First label and text control
    const auto firstArgLabel = new wxStaticText(this, wxID_ANY, firstArgPrompt);
    flex_grid_sizer->Add(firstArgLabel, wxSizerFlags().Right().CenterVertical());

    const auto firstArgInput = new wxTextCtrl(this, wxID_ANY);
    flex_grid_sizer->Add(firstArgInput, wxSizerFlags().Expand());

    // Second label and text control
    const auto secondArgLabel = new wxStaticText(this, wxID_ANY, secondArgPrompt);
    flex_grid_sizer->Add(secondArgLabel, wxSizerFlags().Right().CenterVertical());

    const auto secondArgInput = new wxTextCtrl(this, wxID_ANY);
    flex_grid_sizer->Add(secondArgInput, wxSizerFlags().Expand());

    flex_grid_sizer->AddGrowableCol(1);

    // Create buttons
    wxButton* cancelButton = new wxButton(this, wxID_CANCEL, wxT("Cancel"));
    wxButton* insertButton = new wxButton(this, wxID_OK, wxT("Insert"));
    insertButton->SetDefault();

    // Create sizer for buttons
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->Add(cancelButton);
    buttonSizer->AddSpacer(4);
    buttonSizer->Add(insertButton);

    sizer->Add(webView, wxSizerFlags(1).Expand());
    sizer->AddSpacer(4);
    sizer->Add(flex_grid_sizer, wxSizerFlags().Expand().HorzBorder());
    sizer->AddSpacer(4);
    sizer->Add(buttonSizer, wxSizerFlags().Right().HorzBorder());
    sizer->AddSpacer(4);

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

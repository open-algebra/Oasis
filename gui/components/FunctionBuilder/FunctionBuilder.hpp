//
// Created by Matthew McCall on 4/27/24.
//

#ifndef FUNCTIONBUILDER_HPP
#define FUNCTIONBUILDER_HPP
#include <wx/dialog.h>

class FunctionBuilder : public wxDialog {
public:
    FunctionBuilder(wxWindow* parent, wxWindowID id, const wxString& title, const std::string& function, const std::string& firstArgPrompt, const std::string& secondArgPrompt);
    [[nodiscard]] std::string getComposedFunction() const;

private:
    std::string composedFunction;
};



#endif //FUNCTIONBUILDER_HPP

//
// Created by Matthew McCall on 2/16/24.
//

#ifndef ARITHMETICVIEW_HPP
#define ARITHMETICVIEW_HPP

#include "wx/frame.h"

#include "Oasis/Expression.hpp"
#include "Oasis/MathMLSerializer.hpp"
#include "components/EquationViewer/EquationViewer.hpp"

class wxWebView;
class wxTextCtrl;

class DefaultView final : public wxFrame {
public:
    DefaultView();

private:
    std::string currentInput;
    std::unique_ptr<Oasis::Expression> currentExpression;

    std::vector<std::pair<std::unique_ptr<Oasis::Expression>, std::unique_ptr<Oasis::Expression>>> history;

    void onEnter(const EquationViewer& equationViewer, wxTextCtrl* textCtrl);

    enum class LastReloadReason {
        OnEnter,
        OnInputChanged,
        ThemeChanged
    };

    LastReloadReason lastReloadReason = LastReloadReason::OnEnter;
    int lastScrollHeight = 0;

    EquationViewer viewer_;
};



#endif //ARITHMETICVIEW_HPP

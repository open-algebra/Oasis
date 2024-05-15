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
    std::unique_ptr<Oasis::Expression> currentExpression;
    void onEnter(const EquationViewer& equationViewer, wxTextCtrl* textCtrl);

    EquationViewer viewer_;
};



#endif //ARITHMETICVIEW_HPP

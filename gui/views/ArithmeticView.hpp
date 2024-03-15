//
// Created by Matthew McCall on 2/16/24.
//

#ifndef ARITHMETICVIEW_HPP
#define ARITHMETICVIEW_HPP

#include "wx/frame.h"

#include "Oasis/Expression.hpp"

class ArithmeticView final : public wxFrame {
public:
    ArithmeticView();

private:
    std::vector<std::unique_ptr<Oasis::Expression>> history;
};



#endif //ARITHMETICVIEW_HPP

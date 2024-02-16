//
// Created by Matthew McCall on 2/16/24.
//

#ifndef ARITHMETICVIEW_HPP
#define ARITHMETICVIEW_HPP

#include "wx/frame.h"

class ArithmeticView final : public wxFrame {
public:
    ArithmeticView();

private:
    void OnAbout(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
};



#endif //ARITHMETICVIEW_HPP

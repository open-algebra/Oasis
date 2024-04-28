//
// Created by Matthew McCall on 4/27/24.
//

#include "InputPreprocessor.hpp"

std::string preprocessInput(const std::string& input)
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
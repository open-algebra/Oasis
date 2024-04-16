//
// Created by Matthew McCall on 4/9/24.
//

#include <cctype>
#include <sstream>
#include <stack>

#include "SExpression.hpp"

namespace Oasis::t3 {

Sexp::Sexp(const std::string& str)
{
    std::string strWithNoParens = str;

    // Check if it is surrounded by parenthesis
    if (str.starts_with("(") && str.ends_with(")")) {
        strWithNoParens = str.substr(1, str.length() - 2);
    } else if (str.starts_with("(") || str.ends_with(")")) {
        BAD_SEXP = true;
        return;
    }

    std::stack<std::reference_wrapper<Sexp>> opStack;
    opStack.emplace(*this);

    std::istringstream stream { strWithNoParens };
    stream >> label;

    std::string arg;
    while (stream >> arg) {
        Sexp& current = opStack.top().get();

        if (arg.starts_with("(")) {
            opStack.emplace(current.operands.emplace_back(arg.substr(1, arg.length())));
            continue;
        }

        // what if we had "4)2)"?
        auto closingParensPos = arg.find(')');
        while (closingParensPos != std::string::npos) {
            // get the "4"
            if (auto argBeforeClosingParens = arg.substr(0, closingParensPos); !argBeforeClosingParens.empty()) {
                current.operands.emplace_back(argBeforeClosingParens);
            }
            opStack.pop();
            // make sure this isn't the last parens
            if (closingParensPos + 1 >= arg.size()) {
                goto loop_end;
            }
            // get the "2)"
            arg = arg.substr(closingParensPos + 1, arg.size());
            closingParensPos = arg.find(')');
        }

        current.operands.emplace_back(arg);
        if (operands.back().BAD_SEXP) {
            operands.clear();
            BAD_SEXP = true;
            return;
        }
loop_end: {}
    }
}

}

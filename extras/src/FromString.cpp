//
// Created by Matthew McCall on 4/21/24.
//
#include <memory>
#include <regex>
#include <sstream>
#include <stack>

#include <../../include/Oasis/Add.hpp>
#include <../../include/Oasis/Divide.hpp>
#include <../../include/Oasis/Exponent.hpp>
#include <../../include/Oasis/Log.hpp>
#include <../../include/Oasis/Multiply.hpp>
#include <../../include/Oasis/Real.hpp>
#include <../../include/Oasis/Subtract.hpp>

#include "Oasis/FromString.hpp"

#include "../../include/Oasis/Derivative.hpp"

namespace {

int prec(const char c)
{
    if (c == '^')
        return 3;
    if (c == '*' || c == '/')
        return 2;
    if (c == '+' || c == '-')
        return 1;
    return -1;
}

template <typename T>
void setOps(T& exp, const std::unique_ptr<Oasis::Expression>& op1, const std::unique_ptr<Oasis::Expression>& op2)
{
    if (op1) {
        exp.SetMostSigOp(*op1);
    }
    if (op2) {
        exp.SetLeastSigOp(*op2);
    }
}

void processOp(std::stack<std::string>& ops, std::stack<std::unique_ptr<Oasis::Expression>>& st)
{
    const auto right = std::move(st.top());
    st.pop();
    const auto left = std::move(st.top());
    st.pop();

    // if (ops.top() == "log") {
    //     st.pop();
    //     st.push(std::make_unique<Oasis::Log<>>(*left, *right));
    // } else {
    const auto op = ops.top();
    assert(op.size() == 1);

    std::unique_ptr<Oasis::Expression> opExp;

    switch (op[0]) {
    case '+': {
        Oasis::Add<> add;
        setOps(add, left, right);
        opExp = add.Copy();
    } break;
    case '-': {
        Oasis::Subtract<> subtract;
        setOps(subtract, left, right);
        opExp = subtract.Copy();
    } break;
    case '*': {
        Oasis::Multiply<> multiply;
        setOps(multiply, left, right);
        opExp = multiply.Copy();
    } break;
    case '/': {
        Oasis::Divide<> divide;
        setOps(divide, left, right);
        opExp = divide.Copy();
    } break;
    case '^': {
        Oasis::Exponent<> exponent;
        setOps(exponent, left, right);
        opExp = exponent.Copy();
        break;
    }
    default:
        throw std::runtime_error("Unknown operator encountered: " + op);
    }

    st.emplace(std::move(opExp));
    // }
    ops.pop();
}

void processFunction(std::stack<std::unique_ptr<Oasis::Expression>>& st, const std::string& function_token)
{
    // If we have a function active, the second operand has just been pushed onto the stack
    auto second_operand = std::move(st.top());
    st.pop();
    auto first_operand = std::move(st.top());
    st.pop();

    std::unique_ptr<Oasis::Expression> func;

    if (function_token == "log") {
        Oasis::Log<> log;
        setOps(log, first_operand, second_operand);
        func = log.Copy();
    } else if (function_token == "dd") {
        Oasis::Derivative<> dd;
        setOps(dd, first_operand, second_operand);
        func = dd.Copy();
    } else {
        throw std::runtime_error("Unknown function encountered: " + function_token);
    }

    st.emplace(std::move(func));
}

std::vector<std::string> tokenizeMultiplicands(const std::string& expression)
{
    // Insert '*' between digits and letters, and letters and digits
    std::string explicit_multiplication;
    for (size_t i = 0; i < expression.size(); i++) {
        explicit_multiplication += expression[i];
        if (i < expression.size() - 1) {
            bool is_digit = isdigit(expression[i]);
            bool is_letter = isalpha(expression[i]);
            bool is_digit_next = isdigit(expression[i + 1]);
            bool is_letter_next = isalpha(expression[i + 1]);

            if ((is_digit && is_letter_next) || (is_letter && is_digit_next)) {
                explicit_multiplication += '*';
            }

            if (is_letter && is_letter_next) {
                explicit_multiplication += '*';
            }
        }
    }

    // Tokenize on '*'
    std::vector<std::string> tokens;
    std::string buffer;
    bool inside_braces = false;

    for (char c : explicit_multiplication) {
        if (c == '{')
            inside_braces = true;
        if (c == '}')
            inside_braces = false;

        if (c == '*' && !inside_braces) {
            tokens.push_back(buffer);
            buffer.clear();
        } else {
            buffer += c;
        }
    }

    tokens.push_back(buffer); // push back the last token

    return tokens;
}

template <typename First, typename... T>
bool is_in(First&& first, T&&... t)
{
    return ((first == t) || ...);
}

bool is_operator(const std::string& token) { return is_in(token, "+", "-", "*", "/", "^"); }

bool is_function(const std::string& token) { return is_in(token, "log", "dd"); }

bool is_number(const std::string& token) { return std::regex_match(token, std::regex(R"(^-?\d+(\.\d+)?$)")); }

std::unique_ptr<Oasis::Expression> multiplyFromVariables(const std::vector<std::string>& tokens)
{
    std::vector<std::unique_ptr<Oasis::Expression>> multiplicands;
    std::ranges::transform(tokens, std::back_inserter(multiplicands), [](auto token) -> std::unique_ptr<Oasis::Expression> {
        if (is_number(token)) {
            return std::make_unique<Oasis::Real>(std::stof(token));
        }

        return std::make_unique<Oasis::Variable>(token);
    });

    return Oasis::BuildFromVector<Oasis::Multiply>(multiplicands);
}

}

namespace Oasis {
class Expression;

auto FromInFix(const std::string& str) -> std::unique_ptr<Expression>
{
    // Based of Dijkstra's Shunting Yard

    std::stack<std::unique_ptr<Expression>> st;
    std::stack<std::string> ops;

    std::string token, function_token;
    std::stringstream ss(str);

    while (ss >> token) {
        // Operand
        if (is_number(token)) {
            st.push(std::make_unique<Real>(std::stof(token)));
        }
        // '(' or function
        else if (token == "(" || is_function(token)) {
            ops.push(token);
        }
        // ','
        else if (token == ",") {
            // function_active = true;
            while (!ops.empty() && ops.top() != "(") {
                processOp(ops, st);
            }
        }
        // ')'
        else if (token == ")") {
            while (!ops.empty() && ops.top() != "(") {
                processOp(ops, st);
            }
            ops.pop(); // pop '('
            if (!ops.empty() && is_function(ops.top())) {
                std::string func = ops.top();
                ops.pop();
                processFunction(st, func);
            }
        }
        // Operator
        else if (is_operator(token)) {
            while (!ops.empty() && prec(ops.top()[0]) >= prec(token[0])) {
                processOp(ops, st);
            }
            ops.push(token);
        } else if (ss.peek() != '(') {
            st.push(multiplyFromVariables(tokenizeMultiplicands(token)));
        }
    }

    // Process remaining ops
    while (!ops.empty()) {
        processOp(ops, st);
    }

    return st.top()->Copy(); // root of the expression tree
}

}
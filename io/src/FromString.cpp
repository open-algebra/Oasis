//
// Created by Matthew McCall on 4/21/24.
//
#include <format>
#include <memory>
#include <regex>
#include <sstream>
#include <stack>

#include "Oasis/Imaginary.hpp"
#include <Oasis/Add.hpp>
#include <Oasis/Derivative.hpp>
#include <Oasis/Divide.hpp>
#include <Oasis/EulerNumber.hpp>
#include <Oasis/Exponent.hpp>
#include <Oasis/Integral.hpp>
#include <Oasis/Log.hpp>
#include <Oasis/Multiply.hpp>
#include <Oasis/Real.hpp>
#include <Oasis/Subtract.hpp>

#include "Oasis/FromString.hpp"

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

auto processOp(std::stack<std::string>& ops, std::stack<std::unique_ptr<Oasis::Expression>>& st) -> Oasis::FromInFixResult
{
    if (st.size() < 2) {
        return std::unexpected { "Invalid number of operands" };
    }

    const std::unique_ptr<Oasis::Expression> right = std::move(st.top());
    st.pop();

    const std::unique_ptr<Oasis::Expression> left = std::move(st.top());
    st.pop();

    const auto op = ops.top().front();
    std::unique_ptr<Oasis::Expression> opExp;

    switch (op) {
    case '+': {
        Oasis::Add add { *left, *right };
        opExp = add.Copy();
    } break;
    case '-': {
        Oasis::Subtract subtract { *left, *right };
        opExp = subtract.Copy();
    } break;
    case '*': {
        Oasis::Multiply multiply { *left, *right };
        opExp = multiply.Copy();
    } break;
    case '/': {
        Oasis::Divide divide { *left, *right };
        opExp = divide.Copy();
    } break;
    case '^': {
        Oasis::Exponent exponent { *left, *right };
        opExp = exponent.Copy();
        break;
    }
    default:
        if (op == '(' || op == ')') return std::unexpected { "Mismatched parenthesis" };
        return std::unexpected { std::format(R"(Unknown operator: "{}")", op) };
    }

    ops.pop();
    return opExp;
}

auto processFunction(std::stack<std::unique_ptr<Oasis::Expression>>& st, const std::string& function_token) -> Oasis::FromInFixResult
{
    if (st.size() < 2) {
        return std::unexpected { "Invalid number of operands" };
    }

    // If we have a function active, the second operand has just been pushed onto the stack
    const auto second_operand = std::move(st.top());
    st.pop();
    const auto first_operand = std::move(st.top());
    st.pop();

    std::unique_ptr<Oasis::Expression> func;

    if (function_token == "log") {
        Oasis::Log<> log { *first_operand, *second_operand };
        func = log.Copy();
    } else if (function_token == "dd") {
        Oasis::Derivative<> dd { *first_operand, *second_operand };
        func = dd.Copy();
    } else if (function_token == "in") {
        Oasis::Integral<> in;
        setOps(in, first_operand, second_operand);
        func = in.Copy();
    } else {
        return std::unexpected { std::format(R"(Unknown function: "{}")", function_token) };
    }

    return func;
}

template <typename First, typename... T>
bool is_in(First&& first, T&&... t)
{
    return ((first == t) || ...);
}

bool is_operator(const std::string& token) { return is_in(token, "+", "-", "*", "/", "^"); }

bool is_function(const std::string& token) { return is_in(token, "log", "dd", "in"); }

bool is_number(const std::string& token) { return std::regex_match(token, std::regex(R"(^-?\d+(\.\d+)?$)")); }

std::unique_ptr<Oasis::Expression> parseToken(const std::string& token, const Oasis::ParseImaginaryOption option)
{
    if (is_number(token)) {
        return std::make_unique<Oasis::Real>(std::stof(token));
    }

    if (token == (option == Oasis::ParseImaginaryOption::UseI ? "i" : "j")) {
        return std::make_unique<Oasis::Imaginary>();
    }
    if (token == "e") {
        return std::make_unique<Oasis::EulerNumber>();
    }

    return std::make_unique<Oasis::Variable>(token);
}

}

auto operator|(const std::string& input, const std::function<std::string(const std::string&)>& func) -> std::string
{
    return func(input);
}

namespace Oasis {

auto SpaceAroundOperators(const std::string& str) -> std::string
{
    std::stringstream result;
    std::string operators = "+-*/^(),";

    for (char ch : str) {
        if (std::ranges::find(operators, ch) != operators.end()) {
            result << ' ' << ch << ' ';
        } else {
            result << ch;
        }
    }

    return result.str();
}

auto ImplicitMultiplication(const std::string& str) -> std::string
{
    std::stringstream secondPassResult, sstr { str };
    std::string lastToken = "", token;

    while (sstr >> token) {
        if (is_function(token) || is_operator(token)) {
            secondPassResult << token;
            lastToken = token;
            continue;
        }

        if (lastToken == ")" && token == "(") {
            secondPassResult << '*' << token;
            lastToken = token;
            continue;
        }

        std::string updatedToken;

        for (size_t i = 0; i < token.size(); i++) {
            updatedToken += token[i];

            if (i >= token.size() - 1) { continue; }

            bool is_digit = isdigit(token[i]);
            bool is_letter = isalpha(token[i]);
            bool is_digit_next = isdigit(token[i + 1]);
            bool is_letter_next = isalpha(token[i + 1]);

            if ((is_digit && is_letter_next) || (is_letter && is_digit_next)) {
                updatedToken += "*";
            }

            if (is_letter && is_letter_next) {
                updatedToken += "*";
            }
        }

        secondPassResult << updatedToken;
        lastToken = token;
    }

    return secondPassResult.str();
}

auto PreProcessInFix(const std::string& str) -> std::string
{
    return str | SpaceAroundOperators | ImplicitMultiplication | SpaceAroundOperators;
}

auto FromInFix(const std::string& str, ParseImaginaryOption option) -> std::expected<std::unique_ptr<Expression>, std::string>
{
    // Based off Dijkstra's Shunting Yard

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
                auto processOpResult = processOp(ops, st);
                if (!processOpResult) return std::unexpected { processOpResult.error() };
                st.emplace(std::move(processOpResult.value()));
            }
        }
        // ')'
        else if (token == ")") {
            while (!ops.empty() && ops.top() != "(") {
                auto processOpResult = processOp(ops, st);
                if (!processOpResult) return std::unexpected { processOpResult.error() };
                st.emplace(std::move(processOpResult.value()));
            }

            if (ops.empty() || ops.top() != "(") {
                return std::unexpected { "Mismatched parenthesis" };
            }

            ops.pop(); // pop '('
            if (!ops.empty() && is_function(ops.top())) {
                std::string func = ops.top();
                ops.pop();
                auto processFunctionResult = processFunction(st, func);
                if (!processFunctionResult) return std::unexpected { processFunctionResult.error() };
                st.emplace(std::move(processFunctionResult.value()));
            }
        }
        // Operator
        else if (is_operator(token)) {
            while (!ops.empty() && prec(ops.top()[0]) >= prec(token[0])) {
                auto processOpResult = processOp(ops, st);
                if (!processOpResult) return std::unexpected { processOpResult.error() };
                st.emplace(std::move(processOpResult.value()));
            }
            ops.push(token);
        } else if (ss.peek() != '(') {
            st.push(parseToken(token, option));
        }
    }

    // Process remaining ops
    while (!ops.empty()) {
        auto processOpResult = processOp(ops, st);
        if (!processOpResult) return std::unexpected { processOpResult.error() };
        st.emplace(std::move(processOpResult.value()));
    }

    if (st.empty()) return std::unexpected { "Parsing failed" };
    return st.top()->Copy(); // root of the expression tree
}

}
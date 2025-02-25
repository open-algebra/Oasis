//
// Created by Matthew McCall on 4/21/24.
//
#include <format>
#include <memory>
#include <regex>
#include <sstream>
#include <stack>
#include <variant>

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

enum class Operator {
    Add,
    Subtract,
    Multiply,
    Divide,
    Exponent
};

enum class Function {
    Log,
    Derivative,
    Integral,
};

struct OpenParens { };

int prec(std::variant<Operator, Function, OpenParens> c)
{
    if (!std::holds_alternative<Operator>(c))
        return -1;
    auto op = std::get<Operator>(c);

    if (op == Operator::Exponent)
        return 3;
    if (op == Operator::Multiply || op == Operator::Divide)
        return 2;
    if (op == Operator::Add || op == Operator::Subtract)
        return 1;

    return -1;
}

auto processOp(std::stack<std::variant<Operator, Function, OpenParens>>& ops, std::stack<std::unique_ptr<Oasis::Expression>>& st) -> Oasis::FromInFixResult
{
    if (st.size() < 2) {
        return std::unexpected { "Invalid number of operands" };
    }

    const std::unique_ptr<Oasis::Expression> right = std::move(st.top());
    st.pop();

    const std::unique_ptr<Oasis::Expression> left = std::move(st.top());
    st.pop();

    const auto topOp = ops.top();
    if (!std::holds_alternative<Operator>(topOp))
        return std::unexpected { "Invalid operator" };
    const auto op = std::get<Operator>(topOp);
    std::unique_ptr<Oasis::Expression> opExp;

    switch (op) {
    case Operator::Add:
        opExp = Oasis::Add { *left, *right }.Copy();
        break;
    case Operator::Subtract:
        opExp = Oasis::Subtract { *left, *right }.Copy();
        break;
    case Operator::Multiply:
        opExp = Oasis::Multiply { *left, *right }.Copy();
        break;
    case Operator::Divide:
        opExp = Oasis::Divide { *left, *right }.Copy();
        break;
    case Operator::Exponent:
        opExp = Oasis::Exponent { *left, *right }.Copy();
        break;
    default:
        return std::unexpected { "Invalid operator" };
    }

    ops.pop();
    return opExp;
}

auto processFunction(std::stack<std::unique_ptr<Oasis::Expression>>& st, const Function function_token) -> Oasis::FromInFixResult
{
    if (st.size() < 2) {
        return std::unexpected { "Invalid number of operands" };
    }

    // If we have a function active, the second operand has just been pushed onto the stack
    const auto second_operand = std::move(st.top());
    st.pop();
    const auto first_operand = std::move(st.top());
    st.pop();

    if (function_token == Function::Log)
        return Oasis::Log { *first_operand, *second_operand }.Copy();
    if (function_token == Function::Derivative)
        return Oasis::Derivative { *first_operand, *second_operand }.Copy();
    if (function_token == Function::Integral)
        return Oasis::Integral { *first_operand, *second_operand }.Copy();

    return std::unexpected { "Invalid function" };
}

auto is_operator(const std::string& token) -> std::optional<Operator>
{
    if (token == "+")
        return Operator::Add;
    if (token == "-")
        return Operator::Subtract;
    if (token == "*")
        return Operator::Multiply;
    if (token == "/")
        return Operator::Divide;
    if (token == "^")
        return Operator::Exponent;
    return {};
}

auto is_function(const std::string& token) -> std::optional<Function>
{
    if (token == "log")
        return Function::Log;
    if (token == "dd")
        return Function::Derivative;
    if (token == "in")
        return Function::Integral;
    return {};
}

auto is_number(const std::string& token) -> std::optional<float>
{
    try {
        return std::stof(token);
    } catch (const std::exception&) {
        return {};
    }
}

std::unique_ptr<Oasis::Expression> ParseOperand(const std::string& token, const Oasis::ParseImaginaryOption option)
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

auto operator|(const std::string& input, const std::function<std::stringstream(const std::string&)>& func) -> std::stringstream
{
    return func(input);
}

auto operator|(std::stringstream&& input, const std::function<std::string(std::stringstream&&)>& func) -> std::string
{
    return func(std::move(input));
}

namespace Oasis {

auto SpaceAroundOperators(const std::string& str) -> std::stringstream
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

    return result;
}

auto ImplicitMultiplication(std::stringstream&& sstr) -> std::string
{
    std::stringstream secondPassResult;
    std::string lastToken, token;

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

            if (i >= token.size() - 1) {
                continue;
            }

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
    return (str | SpaceAroundOperators | ImplicitMultiplication | SpaceAroundOperators).str();
}

auto FromInFix(const std::string& str, ParseImaginaryOption option) -> std::expected<std::unique_ptr<Expression>, std::string>
{
    // Based off Dijkstra's Shunting Yard

    std::stack<std::unique_ptr<Expression>> st;
    std::stack<std::variant<Operator, Function, OpenParens>> ops;

    std::string token, function_token;
    std::stringstream ss(str);

    while (ss >> token) {
        // Operand
        if (is_number(token)) {
            st.push(std::make_unique<Real>(std::stof(token)));
        }
        // '(' or function
        else if (token == "(") {
            ops.emplace(OpenParens());
        } else if (auto func = is_function(token); func) {
            ops.emplace(func.value());
        }
        // ','
        else if (token == ",") {
            auto topOp = ops.top();
            // function_active = true;
            while (!ops.empty() && !std::holds_alternative<OpenParens>(topOp)) {
                auto processOpResult = processOp(ops, st);
                if (!processOpResult)
                    return std::unexpected { processOpResult.error() };
                st.emplace(std::move(processOpResult.value()));
            }
        }
        // ')'
        else if (token == ")") {
            auto topOp = ops.top();
            while (!ops.empty() && !std::holds_alternative<OpenParens>(topOp)) {
                auto processOpResult = processOp(ops, st);
                if (!processOpResult)
                    return std::unexpected { processOpResult.error() };
                st.emplace(std::move(processOpResult.value()));
            }

            topOp = ops.top();
            if (ops.empty() || !std::holds_alternative<OpenParens>(topOp)) {
                return std::unexpected { "Mismatched parenthesis" };
            }

            ops.pop(); // pop '('
            topOp = ops.top();
            if (!ops.empty() && std::holds_alternative<Function>(topOp)) {
                auto func = std::get<Function>(topOp);
                ops.pop();
                auto processFunctionResult = processFunction(st, func);
                if (!processFunctionResult)
                    return std::unexpected { processFunctionResult.error() };
                st.emplace(std::move(processFunctionResult.value()));
            }
        }
        // Operator
        else if (auto newOp = is_operator(token); newOp) {
            while (!ops.empty() && prec(ops.top()) >= prec(newOp.value())) {
                auto processOpResult = processOp(ops, st);
                if (!processOpResult)
                    return std::unexpected { processOpResult.error() };
                st.emplace(std::move(processOpResult.value()));
            }
            ops.emplace(newOp.value());
        } else if (ss.peek() != '(') {
            st.push(ParseOperand(token, option));
        }
    }

    // Process remaining ops
    while (!ops.empty()) {
        auto processOpResult = processOp(ops, st);
        if (!processOpResult)
            return std::unexpected { processOpResult.error() };
        st.emplace(std::move(processOpResult.value()));
    }

    if (st.empty())
        return std::unexpected { "Parsing failed" };
    return st.top()->Copy(); // root of the expression tree
}

}
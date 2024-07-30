//
// Created by Matthew McCall on 4/21/24.
//
#include <memory>
#include <regex>
#include <sstream>
#include <stack>

#include <Oasis/Add.hpp>
#include <Oasis/Derivative.hpp>
#include <Oasis/Divide.hpp>
#include <Oasis/Exponent.hpp>
#include "Oasis/Imaginary.hpp"
#include <Oasis/Integral.hpp>
#include <Oasis/Log.hpp>
#include <Oasis/Multiply.hpp>
#include <Oasis/Real.hpp>
#include <Oasis/Subtract.hpp>

#include "Oasis/FromString.hpp"

#include "fmt/format.h"

#include <Oasis/EulerNumber.hpp>

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

bool processOp(std::stack<std::string>& ops, std::stack<std::unique_ptr<Oasis::Expression>>& st)
{
    if (st.size() < 2) {
        // TODO: Pass error message up
        // throw std::runtime_error("Invalid number of arguments");
        return false;
    }

    const std::unique_ptr<Oasis::Expression> right = std::move(st.top());
    st.pop();

    const std::unique_ptr<Oasis::Expression> left = std::move(st.top());
    st.pop();

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
        return false;
    }

    st.emplace(std::move(opExp));
    ops.pop();
    return true;
}

bool processFunction(std::stack<std::unique_ptr<Oasis::Expression>>& st, const std::string& function_token)
{
    if (st.size() < 2) {
        // TODO: Pass error message up
        // throw std::runtime_error("Invalid number of arguments");
        return false;
    }

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
    } else if (function_token == "in") {
        Oasis::Integral<> in;
        setOps(in, first_operand, second_operand);
        func = in.Copy();
    } else {
        return false;
    }

    st.emplace(std::move(func));
    return true;
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

bool is_function(const std::string& token) { return is_in(token, "log", "dd", "in"); }

bool is_number(const std::string& token) { return std::regex_match(token, std::regex(R"(^-?\d+(\.\d+)?$)")); }

std::unique_ptr<Oasis::Expression> parseToken(const std::string& token)
{
    if (is_number(token)) {
        return std::make_unique<Oasis::Real>(std::stof(token));
    }

    if (token == "i") {
        return std::make_unique<Oasis::Imaginary>();
    }

    return std::make_unique<Oasis::Variable>(token);
}

std::unique_ptr<Oasis::Expression> multiplyFromVariables(const std::vector<std::string>& tokens)
{
    std::vector<std::unique_ptr<Oasis::Expression>> multiplicands;
    std::ranges::transform(tokens, std::back_inserter(multiplicands), [](auto token) -> std::unique_ptr<Oasis::Expression> {
        if (is_number(token)) {
            return std::make_unique<Oasis::Real>(std::stof(token));
        }

        if (token == "i") {
            return std::make_unique<Oasis::Imaginary>();
        }
        if (token == "e") {
            return std::make_unique<Oasis::EulerNumber>();
        }

        return std::make_unique<Oasis::Variable>(token);
    });

    if (multiplicands.size() == 1) {
        return std::move(multiplicands.front());
    }

    return Oasis::BuildFromVector<Oasis::Multiply>(multiplicands);
}

}

namespace Oasis {

ParseResult::ParseResult(std::unique_ptr<Expression> expr) {
    result = std::move(expr);
}

ParseResult::ParseResult(std::string err) : result(err) {
}

bool ParseResult::Ok() const {
    return result.index() == 0;
}

auto ParseResult::GetResult() const -> const Expression & {
    return *std::get<std::unique_ptr<Expression>>(result);
}

std::string ParseResult::GetErrorMessage() const
{
    if (const auto message = std::get_if<std::string>(&result)) {
        return *message;
    }

    return "No Error";
}

auto PreProcessFirstPass(const std::string& str) -> std::stringstream
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

auto PreProcessSecondPass(std::stringstream str) -> std::stringstream
{
    std::stringstream secondPassResult;
    std::string token;

    while (str >> token) {
        std::string updatedToken;

        if (!is_function(token)) {
            for (size_t i = 0; i < token.size(); i++) {
                updatedToken += token[i];
                if (i < token.size() - 1) {
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
            }
        } else {
            updatedToken = token;
        }
        secondPassResult << updatedToken;
    }

    return secondPassResult;
}

auto PreProcessInFix(const std::string& str) -> std::string
{
    std::stringstream firstPassResult = PreProcessFirstPass(str);
    std::stringstream secondPassResult = PreProcessSecondPass(std::move(firstPassResult));

    return PreProcessFirstPass(secondPassResult.str()).str();
}

auto FromInFix(const std::string& str) -> ParseResult {
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
                if (!processOp(ops, st)) {
                    return ParseResult { fmt::format(R"(Unknown operator: "{}, or invalid number of operands")", token) };
                }
            }
        }
        // ')'
        else if (token == ")") {
            while (!ops.empty() && ops.top() != "(") {
                if (!processOp(ops, st)) {
                    return ParseResult { fmt::format(R"(Unknown operator: "{}, or invalid number of operands")", token) };
                }
            }

            if (ops.empty() || ops.top() != "(") {
                return ParseResult { "Mismatched parenthesis" };
            }

            ops.pop(); // pop '('
            if (!ops.empty() && is_function(ops.top())) {
                std::string func = ops.top();
                ops.pop();
                if (!processFunction(st, func)) {
                    return ParseResult { fmt::format(R"(Unknown function: "{}")", token) };
                }
            }
        }
        // Operator
        else if (is_operator(token)) {
            while (!ops.empty() && prec(ops.top()[0]) >= prec(token[0])) {
                if (!processOp(ops, st)) {
                    return ParseResult { fmt::format(R"(Unknown operator: "{}")", token) };
                }
            }
            ops.push(token);
        } else if (ss.peek() != '(') {
            st.push(parseToken(token));
        }
    }

    // Process remaining ops
    while (!ops.empty()) {
        if (!processOp(ops, st)) {
            return ParseResult { fmt::format(R"(Unknown operator: "{}")", token) };
        }
    }

    if (st.empty()) {
        return ParseResult { "Parsing failed" };
    }

    return ParseResult { st.top()->Copy() }; // root of the expression tree
}

}
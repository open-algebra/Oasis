//
// Created by codin on 10/31/25.
//

#ifndef OASIS_PALMCONST_HPP
#define OASIS_PALMCONST_HPP

#include <array>
#include <cstdint>
#include <string_view>

#include "Oasis/Expression.hpp"

namespace Oasis {
struct PALMOpts {
    enum class ImaginarySymbol {
        I,
        J
    } imaginarySymbol
        = ImaginarySymbol::I;

    uint8_t numPlaces = 5;

    enum class Separator {
        SPACE
    } separator
        = Separator::SPACE;
};

static auto PALMExpressionToToken(const ExpressionType type, const PALMOpts& options) -> std::string_view
{
    switch (type) {
    case Oasis::ExpressionType::Real:
        return "real";
    case Oasis::ExpressionType::Imaginary:
        switch (options.imaginarySymbol) {
        case PALMOpts::ImaginarySymbol::J:
            return "j";
        case PALMOpts::ImaginarySymbol::I:
        default:
            return "i";
        }
    case Oasis::ExpressionType::Variable:
        return "var";
    case Oasis::ExpressionType::Add:
        return "+";
    case Oasis::ExpressionType::Subtract:
        return "-";
    case Oasis::ExpressionType::Multiply:
        return "*";
    case Oasis::ExpressionType::Divide:
        return "/";
    case Oasis::ExpressionType::Exponent:
        return "^";
    case Oasis::ExpressionType::Log:
        return "log";
    case Oasis::ExpressionType::Integral:
        return "int";
    case Oasis::ExpressionType::Limit:
        return "limit";
    case Oasis::ExpressionType::Derivative:
        return "d";
    case Oasis::ExpressionType::Negate:
        return "neg";
    case Oasis::ExpressionType::Sqrt:
        return "sqrt";
    case Oasis::ExpressionType::Matrix:
        return "matrix";
    case Oasis::ExpressionType::Pi:
        return "pi";
    case Oasis::ExpressionType::EulerNumber:
        return "e";
    case Oasis::ExpressionType::Magnitude:
        return "magnitude";
    default:
        return "";
    }
}

enum PALMDelimiterType {
    START_EXPRESSION,
    END_EXPRESSION,
    SEPARATOR
};

static auto PALMDelimiterToToken(const PALMDelimiterType type, const PALMOpts& options) -> std::string_view
{
    switch (type) {
    case START_EXPRESSION:
        return "(";
    case END_EXPRESSION:
        return ")";
    case SEPARATOR:
        switch (options.separator) {
        case PALMOpts::Separator::SPACE:
        default:
            return " ";
        }
    default:
        return "";
    }
}

struct PALMDelimiters {
    constexpr static inline std::string_view START_EXPRESSION = "(";
    constexpr static inline std::string_view END_EXPRESSION = ")";
};

// Define PALM tokens
constexpr inline std::string_view PALM_OPEN_PARENS = "(";
constexpr inline std::string_view PALM_CLOSE_PARENS = ")";

constexpr inline std::string_view PALM_REAL = "real";
constexpr inline std::array<std::string_view, 2> PALM_IMAGINARY = { "i", "j" };
constexpr inline std::string_view PALM_VARIABLE = "var";
constexpr inline std::string_view PALM_ADD = "+";
constexpr inline std::string_view PALM_SUBTRACT = "-";
constexpr inline std::string_view PALM_MULTIPLY = "*";
constexpr inline std::string_view PALM_DIVIDE = "/";
constexpr inline std::string_view PALM_EXPONENT = "^";
constexpr inline std::string_view PALM_LOG = "log";
constexpr inline std::string_view PALM_INTEGRAL = "int";
constexpr inline std::string_view PALM_NEGATE = "neg";
constexpr inline std::string_view PALM_DERIVATIVE = "d";
constexpr std::string_view PALM_MATRIX = "matrix";
constexpr inline std::array<std::string_view, 1> PALM_PI = { "pi" };
constexpr inline std::string_view PALM_EULER = "e";
}
#endif // OASIS_PALMCONST_HPP
//
// Created by codin on 11/8/25.
//

#include "PALMHelper.hpp"

namespace Oasis {

auto PALMExpressionToToken(const ExpressionType type, const PALMOpts& options) -> std::string_view
{
    switch (type) {
    case Oasis::ExpressionType::Real:
        return "real";
    case Oasis::ExpressionType::Imaginary:
        switch (options.imaginarySymbol) {
        case PALMOpts::ImgSym::J:
            return "j";
        case PALMOpts::ImgSym::I:
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

auto PALMDelimiterToToken(const PALMDelimiterType type, const PALMOpts& options) -> std::string_view
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

}

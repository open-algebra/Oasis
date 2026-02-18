//
// Created by codin on 11/9/25.
//

#ifndef OASIS_PALMCONSTS_HPP
#define OASIS_PALMCONSTS_HPP

#include "Oasis/Expression.hpp"
#include "PALMTypes.hpp"

#include <boost/assign/list_of.hpp>

namespace Oasis {
/** A mapping from PALM operator tokens to their corresponding ExpressionType. */
inline const PALMOperatorBimap kPALMOperatorBimap = ::boost::assign::list_of<PALMOperatorBimap::relation>
    ("undefined", ExpressionType::None)
    ("real", ExpressionType::Real)
    ("i", ExpressionType::Imaginary)
    ("j", ExpressionType::Imaginary)
    ("var", ExpressionType::Variable)
    ("+", ExpressionType::Add)
    ("-", ExpressionType::Subtract)
    ("*", ExpressionType::Multiply)
    ("/", ExpressionType::Divide)
    ("^", ExpressionType::Exponent)
    ("log", ExpressionType::Log)
    ("int", ExpressionType::Integral)
    ("d", ExpressionType::Derivative)
    ("neg", ExpressionType::Negate)
    ("matrix", ExpressionType::Matrix)
    ("pi",  ExpressionType::Pi)
    ("e", ExpressionType::EulerNumber)
    ("magnitude", ExpressionType::Magnitude);

/** Regular expression for valid PALM identifiers. */
static constexpr std::string_view kPALMIdentifierRegex = R"(^[a-zA-Z_][a-zA-Z0-9_]*$)";

/** PALM representations for special numeric values. */
inline constexpr std::string_view kPALMNaN = "NaN";
inline constexpr std::string_view kPALMInfinity = "Infinity";
inline constexpr std::string_view kPALMNegativeInfinity = "-Infinity";

/** Regular expression for valid PALM numbers. */
// In `io/src/PALMConsts.hpp`
static constexpr std::string_view kPALMNumberRegex =
    R"(^([+-]?(\d+(\.\d*)?|\.\d+)([eE][+-]?\d+)?|[+-]?(NaN|Infinity))$)";

/** A mapping from PALM punctuator tokens to their corresponding PALMPunctuatorType. */
inline const PALMPunctuatorBimap kPALMPunctuatorBimap = ::boost::assign::list_of<PALMPunctuatorBimap::relation>
    ("(", PALMPunctuatorType::StartExpression)
    (")", PALMPunctuatorType::EndExpression);

}

#endif // OASIS_PALMCONSTS_HPP

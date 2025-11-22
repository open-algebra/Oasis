//
// Created by codin on 10/31/25.
//

#include <utility>

#include "Oasis/PALMSerializer.hpp"
#include "PALMConsts.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Derivative.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/EulerNumber.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Magnitude.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Negate.hpp"
#include "Oasis/Pi.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Undefined.hpp"
#include "Oasis/Variable.hpp"

namespace Oasis {

bool operator==(const PALMSerializationError& lhs, const PALMSerializationError& rhs)
{
    return lhs.type == rhs.type && lhs.message == rhs.message;
}

auto PALMSerializer::TypedVisit(const Real& real) -> RetT
{
    auto realString = std::format("{:.{}}", real.GetValue(), palmOptions.numPlaces + 1);
    return WrapExpression(ExpressionType::Real, { realString });
}

auto PALMSerializer::TypedVisit(const Imaginary& imaginary) -> RetT
{
    return SerializeExpression(imaginary);
}

auto PALMSerializer::TypedVisit(const Variable& variable) -> RetT
{
    return WrapExpression(ExpressionType::Variable, { variable.GetName() });
}

auto PALMSerializer::TypedVisit(const Undefined& undefined) -> RetT
{
    return SerializeExpression(undefined);
}

auto PALMSerializer::TypedVisit(const Add<>& add) -> RetT
{
    return SerializeExpression(add);
}

auto PALMSerializer::TypedVisit(const Subtract<>& subtract) -> RetT
{
    return SerializeExpression(subtract);
}

auto PALMSerializer::TypedVisit(const Multiply<>& multiply) -> RetT
{
    return SerializeExpression(multiply);
}

auto PALMSerializer::TypedVisit(const Divide<>& divide) -> RetT
{
    return SerializeExpression(divide);
}

auto PALMSerializer::TypedVisit(const Exponent<>& exponent) -> RetT
{
    return SerializeExpression(exponent);
}

auto PALMSerializer::TypedVisit(const Log<>& log) -> RetT
{
    return SerializeExpression(log);
}

auto PALMSerializer::TypedVisit(const Negate<>& negate) -> RetT
{
    return SerializeExpression(negate);
}

auto PALMSerializer::TypedVisit(const Derivative<>& derivative) -> RetT
{
    return SerializeExpression(derivative);
}

auto PALMSerializer::TypedVisit(const Integral<>& integral) -> RetT
{
    return SerializeExpression(integral);
}

auto PALMSerializer::TypedVisit(const Matrix& /*matrix*/) -> RetT
{
    // TODO: Implement Matrix serialization
    return std::unexpected<std::string> { "Matrix is not implemented yet" };
}

auto PALMSerializer::TypedVisit(const EulerNumber& euler_number) -> RetT
{
    return SerializeExpression(euler_number);
}

auto PALMSerializer::TypedVisit(const Pi& pi) -> RetT
{
    return SerializeExpression(pi);
}

auto PALMSerializer::TypedVisit(const Magnitude<Expression>& magnitude) -> RetT
{
    return SerializeExpression(magnitude);
}

/** Convert an ExpressionType (Operator) to its corresponding PALM token.
 *
 * @param op The ExpressionType (Operator) to convert.
 * @param options The PALM options to consider during conversion.
 * @return The corresponding PALM token as a string view.
 */
auto PALMSerializer::OperatorToToken(const ExpressionType op, const PALMSerializationOpts& options) -> std::string_view
{
    switch (op) {
    case ExpressionType::Imaginary:
        switch (options.imaginarySymbol) {
        case PALMSerializationOpts::ImgSymType::J:
            return "j";
        case PALMSerializationOpts::ImgSymType::I:
        default:
            return "i";
        }

    default:
        return kPALMOperatorBimap.right.at(op);
    }
}

/** Convert a PALMPunctuatorType to its corresponding PALM token.
 *
 * @param punctuator The PALMDelimiterType to convert.

 * @return The corresponding PALM token as a string view.
 */
auto PALMSerializer::PunctuatorToToken(const PALMPunctuatorType punctuator, const PALMSerializationOpts& /*options*/) -> std::string_view
{
    return kPALMPunctuatorBimap.right.at(punctuator);
}

/** Serialize a generic expression.
 *
 * @tparam RetT The return type of the serialization (usually std::expected<std::string, std::string>)
 * @param expressionType The type of the expression.
 * @param operands The serialized operands of the expression.
 * @return The serialized expression as a string, or an error if serialization fails.
 */
auto PALMSerializer::WrapExpression(const ExpressionType expressionType, const std::vector<RetT>& operands) const -> RetT
{
    // Build the serialized string
    std::ostringstream serialized;

    // Start Expression
    serialized
        << PunctuatorToToken(PALMPunctuatorType::StartExpression, palmOptions)
        << palmOptions.expressionPadding

        // Add Operator
        << OperatorToToken(expressionType, palmOptions);

    // Add Operands
    for (const auto& operand : operands) {
        // Check for errors in operand serialization
        auto value = operand;

        if (!value) {
            return std::unexpected<std::string> { value.error() };
        }

        // Add Separator and Operand Value
        serialized
            << palmOptions.tokenSeparator
            << value.value();
    }

    // End Expression
    serialized
        << palmOptions.expressionPadding
        << PunctuatorToToken(PALMPunctuatorType::EndExpression, palmOptions);

    // Return the serialized string
    return serialized.str();
}

/** Serialize a leaf expression.
 *
 * @tparam RetT The return type of the serialization (usually std::expected<std::string, std::string>)
 * @param expr The leaf expression to serialize.
 * @return The serialized expression as a string, or an error if serialization fails.
 */
auto PALMSerializer::SerializeExpression(const DerivedFromLeafExpression auto& expr) -> RetT
{
    return WrapExpression(expr.GetType());
}

/** Serialize a unary expression.
 *
 * @tparam RetT The return type of the serialization (usually std::expected<std::string, std::string>)
 * @param expr The unary expression to serialize.
 * @return The serialized expression as a string, or an error if serialization fails.
 */
auto PALMSerializer::SerializeExpression(const DerivedFromUnaryExpression auto& expr) -> RetT
{
    // Ensure the operand exists
    if (!expr.HasOperand()) {
        return std::unexpected<std::string> { std::string(PALM_UNEXPECTED_NO_MOST_SIG_OP) };
    }

    // Serialize the operand
    auto operandResult = expr.GetOperand().Accept(*this);
    if (!operandResult) {
        return std::unexpected<std::string> { operandResult.error() };
    }

    // Serialize the expression
    return WrapExpression(expr.GetType(), { operandResult });
}

/** Serialize a binary expression.
 *
 * @tparam RetT The return type of the serialization (usually std::expected<std::string, std::string>)
 * @param expr The binary expression to serialize.
 * @return The serialized expression as a string, or an error if serialization fails.
 */
auto PALMSerializer::SerializeExpression(const DerivedFromBinaryExpression auto& expr) -> RetT
{
    // Ensure both operands exist
    if (!expr.HasMostSigOp()) {
        return std::unexpected<std::string> { std::string(PALM_UNEXPECTED_NO_MOST_SIG_OP) };
    }
    if (!expr.HasLeastSigOp()) {
        return std::unexpected<std::string> { std::string(PALM_UNEXPECTED_NO_LEAST_SIG_OP) };
    }

    // Serialize both operands
    auto mostSigOpResult = expr.GetMostSigOp().Accept(*this);
    if (!mostSigOpResult) {
        return std::unexpected<std::string> { mostSigOpResult.error() };
    }

    auto leastSigOpResult = expr.GetLeastSigOp().Accept(*this);
    if (!leastSigOpResult) {
        return std::unexpected<std::string> { leastSigOpResult.error() };
    }

    // Serialize the expression
    return WrapExpression(expr.GetType(), { mostSigOpResult, leastSigOpResult });
}

}
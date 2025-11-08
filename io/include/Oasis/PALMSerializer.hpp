//
// Created by codin on 10/31/25.
//

#ifndef OASIS_PALMSERIALIZER_HPP
#define OASIS_PALMSERIALIZER_HPP

#include "../../src/PALMHelper.hpp"
#include "Oasis/Add.hpp"

#include <format>
#include <sstream>
#include <string>
#include <vector>

#include "Oasis/Visit.hpp"

namespace Oasis {

inline std::string_view PALM_UNEXPECTED_NO_MOST_SIG_OP = "Expression missing most significant operand";
inline std::string_view PALM_UNEXPECTED_NO_LEAST_SIG_OP = "Expression missing least significant operand";

class PALMSerializer final : public TypedVisitor<std::expected<std::string, std::string>> {
public:
    PALMSerializer()
        : PALMSerializer(PALMOpts {})
    {
    }
    explicit PALMSerializer(PALMOpts options)
        : palmOptions(std::move(options))
    {
    }

    auto TypedVisit(const Real& real) -> RetT override;
    auto TypedVisit(const Imaginary& imaginary) -> RetT override;
    auto TypedVisit(const Variable& variable) -> RetT override;
    auto TypedVisit(const Undefined& undefined) -> RetT override;
    auto TypedVisit(const Add<Expression, Expression>& add) -> RetT override;
    auto TypedVisit(const Subtract<Expression, Expression>& subtract) -> RetT override;
    auto TypedVisit(const Multiply<Expression, Expression>& multiply) -> RetT override;
    auto TypedVisit(const Divide<Expression, Expression>& divide) -> RetT override;
    auto TypedVisit(const Exponent<Expression, Expression>& exponent) -> RetT override;
    auto TypedVisit(const Log<Expression, Expression>& log) -> RetT override;
    auto TypedVisit(const Negate<Expression>& negate) -> RetT override;
    auto TypedVisit(const Derivative<Expression, Expression>& derivative) -> RetT override;
    auto TypedVisit(const Integral<Expression, Expression>& integral) -> RetT override;
    auto TypedVisit(const Matrix& matrix) -> RetT override;
    auto TypedVisit(const EulerNumber&) -> RetT override;
    auto TypedVisit(const Pi&) -> RetT override;
    auto TypedVisit(const Magnitude<Expression>& magnitude) -> RetT override;
    ~PALMSerializer() override = default;

private:
    PALMOpts palmOptions {};

    /** Serialize a generic expression.
     *
     * @tparam RetT The return type of the serialization (usually std::expected<std::string, std::string>)
     * @param expressionType The type of the expression.
     * @param operands The serialized operands of the expression.
     * @return The serialized expression as a string, or an error if serialization fails.
     */
    auto SerializeExpression(ExpressionType expressionType, const std::vector<RetT>& operands = {}) const -> RetT
    {
        // Build the serialized string
        std::ostringstream serialized;

        // Start Expression
        serialized << PALMDelimiterToToken(START_EXPRESSION, palmOptions) << PALMDelimiterToToken(SEPARATOR, palmOptions);

        // Add Operator
        serialized << PALMExpressionToToken(expressionType, palmOptions);

        // Add Operands
        for (const auto& operand : operands) {
            // Check for errors in operand serialization
            auto value = operand;

            if (!value) {
                return std::unexpected<std::string> { value.error() };
            }

            // Add Separator and Operand Value
            serialized << PALMDelimiterToToken(SEPARATOR, palmOptions) << value.value();
        }

        // End Expression
        serialized << PALMDelimiterToToken(SEPARATOR, palmOptions) << PALMDelimiterToToken(END_EXPRESSION, palmOptions);

        // Return the serialized string
        return serialized.str();
    }

    /** Serialize a unary expression.
     *
     * @tparam RetT The return type of the serialization (usually std::expected<std::string, std::string>)
     * @param expr The unary expression to serialize.
     * @return The serialized expression as a string, or an error if serialization fails.
     */
    auto SerializeUnaryExpression(const DerivedFromUnaryExpression auto& expr) -> RetT
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
        return SerializeExpression(expr.GetType(), { operandResult });
    }

    /** Serialize a binary expression.
     *
     * @tparam RetT The return type of the serialization (usually std::expected<std::string, std::string>)
     * @param expr The binary expression to serialize.
     * @return The serialized expression as a string, or an error if serialization fails.
     */
    auto SerializeBinaryExpression(const DerivedFromBinaryExpression auto& expr) -> RetT
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
        return SerializeExpression(expr.GetType(), { mostSigOpResult, leastSigOpResult });
    }
};

}

#endif // OASIS_PALMSERIALIZER_HPP

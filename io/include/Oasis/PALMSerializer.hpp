//
// Created by codin on 10/31/25.
//

#ifndef OASIS_PALMSERIALIZER_HPP
#define OASIS_PALMSERIALIZER_HPP

#include <format>
#include <string>
#include <vector>

#include "../../src/PALMTypes.hpp"
#include "Oasis/Visit.hpp"
#include "Oasis/Expression.hpp"

namespace Oasis {

/** Options for PALM serialization. */
struct PALMSerializationOpts {
    enum class ImgSymType {
        I,
        J
    } imaginarySymbol
        = ImgSymType::I;

    uint8_t numPlaces = 5;

    std::string tokenSeparator = " ";

    std::string expressionPadding = " ";
};

struct PALMSerializationError {
    enum class PALMSerializationErrorType {
        None,
        InvalidIdentifier,
        MissingOperand,
        Other
    } type;

    const Expression* expression;
    std::string message;
};

/** Equality operator for PALMSerializationError.
 *
 * @param lhs The left-hand side PALMSerializationError.
 * @param rhs The right-hand side PALMSerializationError.
 * @return True if the two errors are equal, false otherwise.
 */
bool operator==(const PALMSerializationError& lhs, const PALMSerializationError& rhs);

inline std::string_view PALM_UNEXPECTED_NO_MOST_SIG_OP = "Expression missing most significant operand";
inline std::string_view PALM_UNEXPECTED_NO_LEAST_SIG_OP = "Expression missing least significant operand";

class PALMSerializer final : public TypedVisitor<std::expected<std::string, PALMSerializationError>> {
public:
    PALMSerializer()
        : PALMSerializer(PALMSerializationOpts {})
    {
    }
    explicit PALMSerializer(PALMSerializationOpts options)
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
    PALMSerializationOpts palmOptions {};

    static inline auto OperatorToToken(ExpressionType op, const PALMSerializationOpts& options) -> std::string_view;
    static inline auto PunctuatorToToken(PALMPunctuatorType punctuator, const PALMSerializationOpts& options) -> std::string_view;

    auto WrapExpression(ExpressionType expressionType, const std::initializer_list<std::expected<std::string, PALMSerializationError>>& operands = {}) const -> RetT;

    auto SerializeExpression(const DerivedFromLeafExpression auto& expr) -> RetT;
    auto SerializeExpression(const DerivedFromUnaryExpression auto& expr) -> RetT;
    auto SerializeExpression(const DerivedFromBinaryExpression auto& expr) -> RetT;
};

}

#endif // OASIS_PALMSERIALIZER_HPP

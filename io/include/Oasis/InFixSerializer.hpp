//
// Created by Matthew McCall on 4/28/24.
//

#ifndef INFIXSERIALIZER_HPP
#define INFIXSERIALIZER_HPP

#include <format>
#include <string>

#include "Oasis/Visit.hpp"

namespace Oasis {

class InFixSerializer final : public Visitor {
public:
    using RetT = std::string;

    any Visit(const Real& real) override;
    any Visit(const Imaginary& imaginary) override;
    any Visit(const Variable& variable) override;
    any Visit(const Undefined& undefined) override;
    any Visit(const Add<Expression, Expression>& add) override;
    any Visit(const Subtract<Expression, Expression>& subtract) override;
    any Visit(const Multiply<Expression, Expression>& multiply) override;
    any Visit(const Divide<Expression, Expression>& divide) override;
    any Visit(const Exponent<Expression, Expression>& exponent) override;
    any Visit(const Log<Expression, Expression>& log) override;
    any Visit(const Negate<Expression>& negate) override;
    any Visit(const Derivative<Expression, Expression>& derivative) override;
    any Visit(const Integral<Expression, Expression>& integral) override;
    any Visit(const Matrix& matrix) override;
    any Visit(const EulerNumber&) override;
    any Visit(const Pi&) override;
    any Visit(const Magnitude<Expression>& magnitude) override;

private:
    auto GetOpsOfBinExp(const DerivedFromBinaryExpression auto& visited) -> std::optional<std::pair<std::string, std::string>>;
    auto SerializeArithBinExp(const DerivedFromBinaryExpression auto& visited, const std::string& op) -> std::optional<std::string>;
    auto SerializeFuncBinExp(const DerivedFromBinaryExpression auto& visited, const std::string& func) -> std::optional<std::string>;

    std::string result;
};

auto InFixSerializer::GetOpsOfBinExp(const DerivedFromBinaryExpression auto& visited) -> std::optional<std::pair<std::string, std::string>>
{
    const auto mostSigOpStr = visited.GetMostSigOp().Accept(*this);
    const auto leastSigOpStr = visited.GetLeastSigOp().Accept(*this);
    if (!mostSigOpStr || !leastSigOpStr)
        return {};

    return std::pair { mostSigOpStr.value(), leastSigOpStr.value() };
}

auto InFixSerializer::SerializeArithBinExp(const DerivedFromBinaryExpression auto& visited, const std::string& op) -> std::optional<std::string>
{
    auto ops = GetOpsOfBinExp(visited);
    if (!ops)
        return {};

    const auto& [mostSigOpStr, leastSigOpStr] = ops.value();
    return std::format("({}{}{})", mostSigOpStr, op, leastSigOpStr);
}

auto InFixSerializer::SerializeFuncBinExp(const DerivedFromBinaryExpression auto& visited, const std::string& func) -> std::optional<std::string>
{
    auto ops = GetOpsOfBinExp(visited);
    if (!ops)
        return {};

    const auto& [mostSigOpStr, leastSigOpStr] = ops.value();
    return std::format("{}({},{})", func, mostSigOpStr, leastSigOpStr);
}

} // Oasis

#endif // INFIXSERIALIZER_HPP

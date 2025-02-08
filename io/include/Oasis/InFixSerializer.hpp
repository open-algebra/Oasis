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

    std::any Visit(const Real& real) override;
    std::any Visit(const Imaginary& imaginary) override;
    std::any Visit(const Variable& variable) override;
    std::any Visit(const Undefined& undefined) override;
    std::any Visit(const Add<Expression, Expression>& add) override;
    std::any Visit(const Subtract<Expression, Expression>& subtract) override;
    std::any Visit(const Multiply<Expression, Expression>& multiply) override;
    std::any Visit(const Divide<Expression, Expression>& divide) override;
    std::any Visit(const Exponent<Expression, Expression>& exponent) override;
    std::any Visit(const Log<Expression, Expression>& log) override;
    std::any Visit(const Negate<Expression>& negate) override;
    std::any Visit(const Derivative<Expression, Expression>& derivative) override;
    std::any Visit(const Integral<Expression, Expression>& integral) override;
    std::any Visit(const Matrix& matrix) override;
    std::any Visit(const EulerNumber&) override;
    std::any Visit(const Pi&) override;
    std::any Visit(const Magnitude<Expression>& magnitude) override;

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
    if (!mostSigOpStr || !leastSigOpStr) return {};

    return std::pair { mostSigOpStr.value(), leastSigOpStr.value() };
}

auto InFixSerializer::SerializeArithBinExp(const DerivedFromBinaryExpression auto& visited, const std::string& op) -> std::optional<std::string>
{
    auto ops = GetOpsOfBinExp(visited);
    if (!ops) return {};

    const auto& [mostSigOpStr, leastSigOpStr] = ops.value();
    return std::format("({}{}{})", mostSigOpStr, op, leastSigOpStr);
}

auto InFixSerializer::SerializeFuncBinExp(const DerivedFromBinaryExpression auto& visited, const std::string& func) -> std::optional<std::string>
{
    auto ops = GetOpsOfBinExp(visited);
    if (!ops) return {};

    const auto& [mostSigOpStr, leastSigOpStr] = ops.value();
    return std::format("{}({},{})", func, mostSigOpStr, leastSigOpStr);
}

} // Oasis

#endif // INFIXSERIALIZER_HPP

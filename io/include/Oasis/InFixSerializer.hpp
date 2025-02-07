//
// Created by Matthew McCall on 4/28/24.
//

#ifndef INFIXSERIALIZER_HPP
#define INFIXSERIALIZER_HPP

#include <string>

#include "Oasis/Visit.hpp"

namespace Oasis {

class InFixSerializer final : public Visitor {
public:
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

    [[nodiscard]] std::string getResult() const;

private:
    auto GetOpsOfBinExp(const DerivedFromBinaryExpression auto& visited) -> std::pair<std::string, std::string>;
    auto SerializeArithBinExp(const DerivedFromBinaryExpression auto& visited, const std::string& op) -> std::any;
    auto SerializeFuncBinExp(const DerivedFromBinaryExpression auto& visited, const std::string& func) -> std::any;

    std::string result;
};

auto InFixSerializer::GetOpsOfBinExp(const DerivedFromBinaryExpression auto& visited) -> std::pair<std::string, std::string>
{
    const auto mostSigOpStr = std::any_cast<std::string>(visited.GetMostSigOp().Accept(*this));
    const auto leastSigOpStr = std::any_cast<std::string>(visited.GetLeastSigOp().Accept(*this));
    return { mostSigOpStr, leastSigOpStr };
}

auto InFixSerializer::SerializeArithBinExp(const DerivedFromBinaryExpression auto& visited, const std::string& op) -> std::any
{
    try {
        const auto& [mostSigOpStr, leastSigOpStr] = GetOpsOfBinExp(visited);
        return fmt::format("({}{}{})", mostSigOpStr, op, leastSigOpStr);
    } catch (std::bad_any_cast) {
        return {};
    }
}

auto InFixSerializer::SerializeFuncBinExp(const DerivedFromBinaryExpression auto& visited, const std::string& func) -> std::any
{
    try {
        const auto& [mostSigOpStr, leastSigOpStr] = GetOpsOfBinExp(visited);
        return fmt::format("{}({},{})", func, mostSigOpStr, leastSigOpStr);
    } catch (std::bad_any_cast) {
        return {};
    }
}

} // Oasis

#endif // INFIXSERIALIZER_HPP

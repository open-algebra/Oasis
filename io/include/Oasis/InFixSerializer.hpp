//
// Created by Matthew McCall on 4/28/24.
//

#ifndef INFIXSERIALIZER_HPP
#define INFIXSERIALIZER_HPP

#include <format>
#include <string>

#include "Oasis/Visit.hpp"

namespace Oasis {

class InFixSerializer final : public TypedVisitor<std::expected<std::string, std::string_view>> {
public:
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

private:
    auto GetOpsOfBinExp(const DerivedFromBinaryExpression auto& visited) -> std::expected<std::pair<std::string, std::string>, std::string_view>;
    auto SerializeArithBinExp(const DerivedFromBinaryExpression auto& visited, const std::string& op) -> std::expected<std::string, std::string_view>;
    auto SerializeFuncBinExp(const DerivedFromBinaryExpression auto& visited, const std::string& func) -> std::expected<std::string, std::string_view>;
};

auto InFixSerializer::GetOpsOfBinExp(const DerivedFromBinaryExpression auto& visited) -> std::expected<std::pair<std::string, std::string>, std::string_view>
{
    InFixSerializer& thisSerializer = *this;
    return visited.GetMostSigOp().Accept(thisSerializer).and_then([&thisSerializer, &visited](const auto& mostSigOpStr) {
        return visited.GetLeastSigOp().Accept(thisSerializer).transform([&mostSigOpStr](const auto& leastSigOpStr) {
            return std::pair { mostSigOpStr, leastSigOpStr };
        });
    });
}

auto InFixSerializer::SerializeArithBinExp(const DerivedFromBinaryExpression auto& visited, const std::string& op) -> std::expected<std::string, std::string_view>
{
    return GetOpsOfBinExp(visited).transform([&op](const auto& ops) {
        return std::format("({}{}{})", ops.first, op, ops.second);
    });
}

auto InFixSerializer::SerializeFuncBinExp(const DerivedFromBinaryExpression auto& visited, const std::string& func) -> std::expected<std::string, std::string_view>
{
    return GetOpsOfBinExp(visited).transform([&func](const auto& ops) {
        return std::format("{}({},{})", func, ops.first, ops.second);
    });
}

} // Oasis

#endif // INFIXSERIALIZER_HPP

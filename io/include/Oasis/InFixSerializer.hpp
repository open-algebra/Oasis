//
// Created by Matthew McCall on 4/28/24.
//

#ifndef INFIXSERIALIZER_HPP
#define INFIXSERIALIZER_HPP

#include <format>
#include <string>

#include "Oasis/Visit.hpp"

namespace Oasis {

/**
 * The InFixSerializer class is a class for end users to evaluate their results and display
 * the final evaluated expression after computation in a string format.
 *
 * @section Parameters
 * None
 *
 * @section exam Example Usage:
 * @code
std::string exp = {"x+5"};

const auto preproc = Oasis::PreProcessInFix(exp);
auto midResult = Oasis::FromInFix(preproc);

const std::unique_ptr<Oasis::Expression> expression = std::move(midResult).value();

Oasis::Integral in {
    *expression,
    Oasis::Variable{"x"}
};
Oasis::InFixSerializer result; // Instantiate an InFixSerializer object.

auto resultant= in.Simplify(); // Simplify the integral to evaluate it.

std::println{"Result: {}", resultant->Accept(result).value()};
// Extract the string version of the evaluated expression
 * @endcode
 *
 */
class InFixSerializer final : public TypedVisitor<std::expected<std::string, std::string>> {
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
    auto TypedVisit(const Sine<Expression>& sine) -> RetT override;
    auto TypedVisit(const Derivative<Expression, Expression>& derivative) -> RetT override;
    auto TypedVisit(const Integral<Expression, Expression>& integral) -> RetT override;
    auto TypedVisit(const Matrix& matrix) -> RetT override;
    auto TypedVisit(const EulerNumber&) -> RetT override;
    auto TypedVisit(const Pi&) -> RetT override;
    auto TypedVisit(const Magnitude<Expression>& magnitude) -> RetT override;

private:
    auto GetOpsOfBinExp(const DerivedFromBinaryExpression auto& visited) -> std::expected<std::pair<std::string, std::string>, std::string>;
    auto SerializeArithBinExp(const DerivedFromBinaryExpression auto& visited, const std::string& op) -> std::expected<std::string, std::string>;
    auto SerializeFuncBinExp(const DerivedFromBinaryExpression auto& visited, const std::string& func) -> std::expected<std::string, std::string>;
};

auto InFixSerializer::GetOpsOfBinExp(const DerivedFromBinaryExpression auto& visited) -> std::expected<std::pair<std::string, std::string>, std::string>
{
    InFixSerializer& thisSerializer = *this;
    return visited.GetMostSigOp().Accept(thisSerializer).and_then([&thisSerializer, &visited](const std::string& mostSigOpStr) {
        return visited.GetLeastSigOp().Accept(thisSerializer).transform([&mostSigOpStr](const std::string& leastSigOpStr) {
            return std::pair { mostSigOpStr, leastSigOpStr };
        });
    });
}

auto InFixSerializer::SerializeArithBinExp(const DerivedFromBinaryExpression auto& visited, const std::string& op) -> RetT
{
    return GetOpsOfBinExp(visited).transform([&op](const auto& ops) {
        return std::format("({}{}{})", ops.first, op, ops.second);
    });
}

auto InFixSerializer::SerializeFuncBinExp(const DerivedFromBinaryExpression auto& visited, const std::string& func) -> RetT
{
    return GetOpsOfBinExp(visited).transform([&func](const auto& ops) {
        return std::format("{}({},{})", func, ops.first, ops.second);
    });
}

} // Oasis

#endif // INFIXSERIALIZER_HPP

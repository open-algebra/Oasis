//
// Created by Matthew McCall on 8/10/23.
//

#include <map>
#include <vector>

#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/RecursiveCast.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"

namespace Oasis {

Divide<Expression>::Divide(const Expression& dividend, const Expression& divisor)
    : BinaryExpression(dividend, divisor)
{
}

auto Divide<Expression>::Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression>
{
    SimplifyVisitor simplifyVisitor {};
    // Single integration variable
    if (auto variable = RecursiveCast<Variable>(integrationVariable); variable != nullptr) {
        auto simplifiedDiv = this->Accept(simplifyVisitor).value();

        // Constant case - Integrand over a divisor
        if (auto constant = RecursiveCast<Multiply<Expression, Real>>(*simplifiedDiv); constant != nullptr) {
            return constant->Integrate(integrationVariable)->Accept(simplifyVisitor).value();
        }
    }

    Integral<Expression, Expression> integral { *(this->Copy()), *(integrationVariable.Copy()) };

    return integral.Copy();
}

auto Divide<Expression>::Differentiate(const Oasis::Expression& differentiationVariable) const -> std::unique_ptr<Expression>
{
    SimplifyVisitor simplifyVisitor {};
    // Single differentiation variable
    if (auto variable = RecursiveCast<Variable>(differentiationVariable); variable != nullptr) {
        auto simplifiedDiv = this->Accept(simplifyVisitor).value();

        // Constant case - differentiation over a divisor
        if (auto constant = RecursiveCast<Divide<Expression, Real>>(*simplifiedDiv); constant != nullptr) {
            auto exp = constant->GetMostSigOp().Copy();
            auto num = constant->GetLeastSigOp();
            auto differentiate = (*exp).Differentiate(differentiationVariable);
            if (auto add = RecursiveCast<Expression>(*differentiate); add != nullptr) {
                auto simplifiedAdd = add->Accept(simplifyVisitor).value();
                return std::make_unique<Divide<Expression, Real>>(Divide<Expression, Real> { *simplifiedAdd, Real { num.GetValue() } })->Accept(simplifyVisitor).value();
            }
        }
        // In case of simplify turning divide into mult
        if (auto constant = RecursiveCast<Multiply<Expression, Real>>(*simplifiedDiv); constant != nullptr) {
            auto exp = constant->GetMostSigOp().Copy();
            auto num = constant->GetLeastSigOp();
            auto differentiate = (*exp).Differentiate(differentiationVariable);
            if (auto add = RecursiveCast<Expression>(*differentiate); add != nullptr) {
                auto simplifiedAdd = (add->Accept(simplifyVisitor).value());
                return std::make_unique<Multiply<Expression, Real>>(Multiply<Expression, Real> { *simplifiedAdd, Real { num.GetValue() } })->Accept(simplifyVisitor).value();
            }
        }
        // Quotient Rule: d/dx (f(x)/g(x)) = (g(x)f'(x)-f(x)g'(x))/(g(x)^2)
        if (auto quotient = RecursiveCast<Divide<Expression, Expression>>(*simplifiedDiv); quotient != nullptr) {
            auto leftexp = quotient->GetMostSigOp().Copy();
            auto rightexp = quotient->GetLeastSigOp().Copy();
            auto leftDiff = leftexp->Differentiate(differentiationVariable);
            auto rightDiff = rightexp->Differentiate(differentiationVariable);
            auto mult1 = Multiply<Expression, Expression>(Multiply<Expression, Expression> { *(rightexp->Accept(simplifyVisitor).value()), *(leftDiff->Accept(simplifyVisitor).value()) }).Accept(simplifyVisitor).value()->Accept(simplifyVisitor).value();
            auto mult2 = Multiply<Expression, Expression>(Multiply<Expression, Expression> { *(leftexp->Accept(simplifyVisitor).value()), *(rightDiff->Accept(simplifyVisitor).value()) }).Accept(simplifyVisitor).value()->Accept(simplifyVisitor).value();
            auto numerator = Subtract<Expression, Expression>(Subtract<Expression, Expression> { *mult1, *mult2 }).Accept(simplifyVisitor).value();
            auto denominator = Multiply<Expression, Expression>(Multiply<Expression, Expression> { *(rightexp->Accept(simplifyVisitor).value()), *(rightexp->Accept(simplifyVisitor).value()) }).Accept(simplifyVisitor).value();
            return Divide<Expression, Expression>({ *(numerator->Accept(simplifyVisitor).value()), *(denominator->Accept(simplifyVisitor).value()) }).Accept(simplifyVisitor).value();
        }
    }

    return Copy();
}

} // Oasis
//
// Created by Matthew McCall on 8/10/23.
//

#include "Oasis/Subtract.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Matrix.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/RecursiveCast.hpp"
#include "Oasis/Variable.hpp"

namespace Oasis {

Subtract<Expression>::Subtract(const Expression& minuend, const Expression& subtrahend)
    : BinaryExpression(minuend, subtrahend)
{
}

auto Subtract<Expression>::Differentiate(const Expression& differentiationVariable) const -> std::unique_ptr<Expression>
{
    SimplifyVisitor simplifyVisitor {};
    // Single diff variable
    if (auto variable = RecursiveCast<Variable>(differentiationVariable); variable != nullptr) {
        auto s = this->Accept(simplifyVisitor);
        if (!s) {
            return this->Generalize();
        }
        auto simplifiedSub = std::move(s).value();

        // Make sure we're still subtracting
        if (auto adder = RecursiveCast<Subtract<Expression>>(*simplifiedSub); adder != nullptr) {
            auto rightRef = adder->GetLeastSigOp().Copy();
            auto rightDiff = rightRef->Differentiate(differentiationVariable);

            auto specializedRight = RecursiveCast<Expression>(*rightDiff);

            auto leftRef = adder->GetMostSigOp().Copy();
            auto leftDiff = leftRef->Differentiate(differentiationVariable);

            auto specializedLeft = RecursiveCast<Expression>(*leftDiff);

            if (specializedLeft == nullptr || specializedRight == nullptr) {
                return Copy();
            }

            auto us = std::make_unique<Subtract<Expression, Expression>>(Subtract<Expression, Expression> { *(specializedLeft->Copy()), *(specializedRight->Copy()) });
            auto s1 = us->Accept(simplifyVisitor);
            if (!s1) {
                return us;
            }
            return std::move(s1).value();

        }
        // If not, use other differentiation technique
        else {
            return simplifiedSub->Differentiate(differentiationVariable);
        }
    }
    return Copy();
}

auto Subtract<Expression>::Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression>
{
    SimplifyVisitor simplifyVisitor {};
    // Single integration variable
    if (auto variable = RecursiveCast<Variable>(integrationVariable); variable != nullptr) {
        auto s = this->Accept(simplifyVisitor);
        if (!s) {
            return this->Generalize();
        }
        auto simplifiedSub = std::move(s).value();

        // Make sure we're still subtracting
        if (auto adder = RecursiveCast<Subtract<Expression>>(*simplifiedSub); adder != nullptr) {
            auto leftRef = adder->GetLeastSigOp().Copy();
            auto leftIntegral = leftRef->Integrate(integrationVariable);

            auto specializedLeft = RecursiveCast<Add<Expression>>(*leftIntegral);

            auto rightRef = adder->GetMostSigOp().Copy();
            auto rightIntegral = rightRef->Integrate(integrationVariable);

            auto specializedRight = RecursiveCast<Add<Expression>>(*rightIntegral);

            if (specializedLeft == nullptr || specializedRight == nullptr) {
                return Copy();
            }
            Add add { Subtract<Expression> {
                          *(specializedRight->GetMostSigOp().Copy()),
                          *(specializedLeft->GetMostSigOp().Copy()) },
                Variable { "C" } };

            return add.Accept(simplifyVisitor).value();
        }
        // If not, use other integration technique
        else {
            return simplifiedSub->Integrate(integrationVariable);
        }
    }
    Integral<Expression, Expression> integral { *(this->Copy()), *(integrationVariable.Copy()) };

    return integral.Copy();
}

} // Oasis
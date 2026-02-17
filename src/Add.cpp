//
// Created by Matthew McCall on 7/2/23.
//
#include <unordered_map>

#include "Oasis/Add.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/MatchCast.hpp"
#include "Oasis/Matrix.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/RecursiveCast.hpp"

#define EPSILON 10E-6

namespace Oasis {
auto Add<Expression>::Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression>
{
    // Single integration variable
    SimplifyVisitor simplifyVisitor;
    if (auto variable = RecursiveCast<Variable>(integrationVariable); variable != nullptr) {
        auto simplified = this->Accept(simplifyVisitor);

        if (!simplified) {
            return this->Generalize();
        }

        auto simplifiedAdd = std::move(simplified).value();

        // Make sure we're still adder
        if (auto adder = RecursiveCast<Add<Expression>>(*simplifiedAdd); adder != nullptr) {
            auto leftRef = adder->GetLeastSigOp().Copy();
            auto leftIntegral = leftRef->Integrate(integrationVariable);

            auto specializedLeft = RecursiveCast<Add<Expression>>(*leftIntegral);

            auto rightRef = adder->GetMostSigOp().Copy();
            auto rightIntegral = rightRef->Integrate(integrationVariable);

            auto specializedRight = RecursiveCast<Add<Expression>>(*rightIntegral);
            if (specializedLeft == nullptr || specializedRight == nullptr) {
                return Copy();
            }
            Add<Expression> add {
                Add<Expression, Expression> {
                    *(specializedLeft->GetMostSigOp().Copy()), *(specializedRight->GetMostSigOp().Copy()) },
                Variable { "C" }
            };
            auto postSimplify = add.Accept(simplifyVisitor);
            if (!postSimplify) {
                return add.Generalize();
            }
            return std::move(postSimplify).value();
        }
        // If not, use other integration technique
        return simplifiedAdd->Integrate(integrationVariable)->Accept(simplifyVisitor).value();
    }
    Integral<Expression, Expression> integral { *(this->Copy()), *(integrationVariable.Copy()) };

    return integral.Copy();
}

auto Add<Expression>::Differentiate(const Expression& differentiationVariable) const -> std::unique_ptr<Expression>
{
    if (auto variable = RecursiveCast<Variable>(differentiationVariable); variable != nullptr) {
        auto left = mostSigOp->Differentiate(differentiationVariable);
        auto right = leastSigOp->Differentiate(differentiationVariable);
        SimplifyVisitor simplifyVisitor;
        auto simplified = Add<Expression> { *left, *right }.Accept(simplifyVisitor);
        if (!simplified) {
            Add<Expression> { *left, *right };
        }
        return std::move(simplified).value();
    }
    return Copy();
}

} // Oasis
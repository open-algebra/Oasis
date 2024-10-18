//
// Created by bachia on 4/12/2024.
//

#include "Oasis/Derivative.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Expression.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Undefined.hpp"

#include <cmath>
#include <string>

namespace Oasis {
Derivative<Expression>::Derivative(const Expression& exp, const Expression& var)
    : BinaryExpression(exp, var)
{
}

auto Derivative<Expression>::Simplify() const -> std::unique_ptr<Expression>
{
    auto simplifiedExpression = mostSigOp ? mostSigOp->Simplify() : nullptr;
    auto simplifiedVar = leastSigOp ? leastSigOp->Simplify() : nullptr;
    return simplifiedExpression->Differentiate(*simplifiedVar);
}

std::unique_ptr<Expression> Derivative<Expression, Expression>::Differentiate(const Expression&) const
{
    return mostSigOp->Differentiate(*leastSigOp);
}

auto Derivative<Expression>::Specialize(const Expression& other) -> std::unique_ptr<Derivative<Expression, Expression>>
{
    if (!other.Is<Oasis::Derivative>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize();
    return std::make_unique<Derivative>(dynamic_cast<const Derivative&>(*otherGeneralized));
}

}

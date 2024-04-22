//
// Created by bachia on 4/12/2024.
//

#include "../include/Oasis/Derivative.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Expression.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Undefined.hpp"
#include "string"
#include <cmath>

namespace Oasis {
Derivative<Expression>::Derivative(const Expression& exp, const Expression& var)
    : BinaryExpression(exp, var)
{
}

auto Derivative<Expression>::Simplify() const -> std::unique_ptr<Expression>
{
    auto simplifiedExpression = mostSigOp ? mostSigOp->Simplify() : nullptr;
    auto simplifiedVar = leastSigOp ? leastSigOp->Simplify() : nullptr;

    if (auto realCase = Real::Specialize(*simplifiedExpression); realCase != nullptr) {
        return realCase->Differentiate(*simplifiedVar);
    }
    if (auto addCase = Add<Expression>::Specialize(*simplifiedExpression); addCase != nullptr) {
        return addCase->Differentiate(*simplifiedVar);
    }
    if (auto subCase = Subtract<Expression>::Specialize(*simplifiedExpression); subCase != nullptr) {
        return subCase->Differentiate(*simplifiedVar);
    }
    if (auto multCase = Multiply<Expression>::Specialize(*simplifiedExpression); multCase != nullptr) {
        return multCase->Differentiate(*simplifiedVar);
    }
    if (auto divCase = Divide<Expression>::Specialize(*simplifiedExpression); divCase != nullptr) {
        return divCase->Differentiate(*simplifiedVar);
    }
    if (auto varCase = Variable::Specialize(*simplifiedExpression); varCase != nullptr) {
        return varCase->Differentiate(*simplifiedVar);
    }
    if (auto expCase = Exponent<Expression, Expression>::Specialize(*simplifiedExpression); expCase != nullptr) {
        return expCase->Differentiate(*simplifiedVar);
    }
    return simplifiedExpression->Differentiate(*simplifiedVar);
}
auto Derivative<Expression>::ToString() const -> std::string
{
    return fmt::format("(d({})/d{})", mostSigOp->ToString(), leastSigOp->ToString());
}
auto Derivative<Expression>::Specialize(const Expression& other) -> std::unique_ptr<Derivative<Expression, Expression>>
{
    if (!other.Is<Oasis::Derivative>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize();
    return std::make_unique<Derivative>(dynamic_cast<const Derivative&>(*otherGeneralized));
}

auto Derivative<Expression>::Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Derivative>
{
    if (!other.Is<Oasis::Derivative>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize(subflow);
    return std::make_unique<Derivative>(dynamic_cast<const Derivative&>(*otherGeneralized));
}
}

//
// Created by linle on 2/24/2024.
//

#include "Oasis/Integrate.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Log.hpp"

namespace Oasis{

Integrate<Expression>::Integrate(const Expression& integrandend, const Expression& differentialend)
    : BinaryExpression(integrandend, differentialend)
{
}

auto Integrate<Expression>::Simplify() const -> std::unique_ptr<Expression>{
    // To be Implemented

}

auto Integrate<Expression>::ToString() const -> std::string
{
    return fmt::format("({} + {})", mostSigOp->ToString(), leastSigOp->ToString());
}

auto Integrate<Expression>::Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression>
{
    // To be Implemented
}

auto Integrate<Expression>::Specialize(const Expression& other) -> std::unique_ptr<Integrate<Expression, Expression>>
{
    if (!other.Is<Oasis::Integrate>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize();
    return std::make_unique<Integrate>(dynamic_cast<const Integrate&>(*otherGeneralized));
}

auto Integrate<Expression>::Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Integrate>
{
    if (!other.Is<Oasis::Integrate>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize(subflow);
    return std::make_unique<Integrate>(dynamic_cast<const Integrate&>(*otherGeneralized));
}

} // Oasis

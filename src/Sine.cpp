//
// Created by Andrew Nazareth on 10/8/24.
//

#include "Oasis/Sine.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Pi.hpp"
#include "Oasis/Real.hpp"

namespace Oasis {
Sine<Expression>::Sine(const Expression& operand)
    : UnaryExpression(operand)
{
}

auto Sine<Expression>::Simplify() const -> std::unique_ptr<Expression>
{
    //    std::cout<<"Sine Simplify"<<std::endl;
    auto simplifiedOperand = op ? op->Simplify() : nullptr;

    if (auto PiCase = Pi::Specialize(*simplifiedOperand); PiCase != nullptr) {
        return std::make_unique<Real>(0);
    }

    return std::make_unique<Real>(-128);
}

auto Sine<Expression>::Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression>
{
    // TODO: Implement
    return Expression::Integrate(integrationVariable);
}

auto Sine<Expression>::Differentiate(const Expression& differentiationVariable) const -> std::unique_ptr<Expression>
{
    // TODO: Implement
    return Expression::Differentiate(differentiationVariable);
}

}
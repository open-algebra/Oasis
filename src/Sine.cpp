//
// Created by Andrew Nazareth on 9/19/25.
//

#include "Oasis/Sine.hpp"
#include "Oasis/Expression.hpp"
#include "Oasis/UnaryExpression.hpp"

namespace Oasis {
auto Sine<Expression>::Simplify() const -> std::unique_ptr<Expression>
{
    return nullptr;
}

auto Sine<Expression>::Differentiate(const Expression&) const -> std::unique_ptr<Expression>
{
    return nullptr;
}

auto Sine<Expression>::Integrate(const Expression&) const -> std::unique_ptr<Expression>
{
    return nullptr;
}
} // Oasis
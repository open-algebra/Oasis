//
// Created by Matthew McCall on 7/2/23.
//

#include <string>

#include "Oasis/Add.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Variable.hpp"

namespace Oasis {

Real::Real(double value)
    : value(value)
{
}

auto Real::Equals(const Expression& other) const -> bool
{
    return other.Is<Real>() && value == dynamic_cast<const Real&>(other).value;
}

auto Real::GetValue() const -> double
{
    return value;
}

auto Real::ToString() const -> std::string
{
    return std::to_string(value);
}

auto Real::Specialize(const Expression& other) -> std::unique_ptr<Real>
{
    return other.Is<Real>() ? std::make_unique<Real>(dynamic_cast<const Real&>(other)) : nullptr;
}

auto Real::Specialize(const Expression& other, tf::Subflow&) -> std::unique_ptr<Real>
{
    return other.Is<Real>() ? std::make_unique<Real>(dynamic_cast<const Real&>(other)) : nullptr;
}

auto Real::Integrate(const Variable& integrationVariable) -> std::unique_ptr<Expression>
{
    if (value != 0) {
        return std::make_unique<Add<Multiply<Real, Variable>, Variable>>(Add {
            Multiply<Real, Variable> { Real { value }, Variable { integrationVariable.GetName() } },
            Variable { "C" } });
    }

    return std::make_unique<Variable>(Variable { "C" });
}

} // namespace Oasis
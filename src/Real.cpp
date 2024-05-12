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

auto Real::Differentiate(const Expression&) const -> std::unique_ptr<Expression>
{
    return std::make_unique<Real>(0);
}

auto Real::Equals(const Expression& other) const -> bool
{
    return other.Is<Real>() && value == dynamic_cast<const Real&>(other).value;
}

auto Real::GetValue() const -> double
{
    return value;
}

auto Real::Specialize(const Expression& other) -> std::unique_ptr<Real>
{
    return other.Is<Real>() ? std::make_unique<Real>(dynamic_cast<const Real&>(other)) : nullptr;
}

auto Real::Specialize(const Expression& other, tf::Subflow&) -> std::unique_ptr<Real>
{
    return other.Is<Real>() ? std::make_unique<Real>(dynamic_cast<const Real&>(other)) : nullptr;
}

auto Real::Integrate(const Expression& integrationVariable) -> std::unique_ptr<Expression>
{
    if (auto variable = Variable::Specialize(integrationVariable); variable != nullptr) {
        if (value != 0) {
            return std::make_unique<Add<Multiply<Real, Variable>, Variable>>(Add {
                Multiply<Real, Variable> { Real { value }, Variable { (*variable).GetName() } },
                Variable { "C" } });
        }

        return std::make_unique<Variable>(Variable { "C" });
    }
    return Copy();
}

} // namespace Oasis
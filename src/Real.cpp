//
// Created by Matthew McCall on 7/2/23.
//

#include <string>

#include "Oasis/Add.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/RecursiveCast.hpp"
#include "Oasis/Variable.hpp"

namespace Oasis {

Real::Real(double value, Unit unit)
    : value(value), unit(unit)
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

auto Real::GetFactor(Unit from, Unit to) const -> double
{
    using namespace boost::units;
    using namespace boost::units::si;

    quantity<si::length> one_meter(1.0 * meter);
    quantity<si::length> one_kilometer(1000 * meter);
    if (from == Unit::Meter && to == Unit::Kilogram) {
        return one_meter.value() / one_kilometer.value();
    }
    if (from == Unit::Kilogram && to == Unit::Meter) {
        return one_kilometer.value() / one_meter.value();
    }

    //Other Cases
    return 1.0;
}

auto Real::ConvertTo(Unit targetUnit) const -> Real
{   //template of the ConvertTo AS OF NOW
    if (unit == targetUnit) {
        return *this;
    }

    double factor = GetFactor(unit, targetUnit);
    return Real(value * factor, targetUnit);
}

auto Real::GetValue() const -> double
{
    return value;
}

auto Real::GetUnit() const -> Unit
{
    return unit;
}

auto Real::Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression>
{
    if (auto variable = RecursiveCast<Variable>(integrationVariable); variable != nullptr) {
        // Constant rule
        if (value != 0) {

            Add adder {
                Multiply<Real, Variable> { Real { value }, Variable { (*variable).GetName() } },
                Variable { "C" }
            };
            return adder.Simplify();
        }

        // Zero rule
        return std::make_unique<Variable>(Variable { "C" })->Simplify();
    }

    Integral<Expression, Expression> integral { *(this->Copy()), *(integrationVariable.Copy()) };

    return integral.Copy();
}

} // namespace Oasis
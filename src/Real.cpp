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

auto Real::Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression>
{
    SimplifyVisitor simplifyVisitor {};
    if (auto variable = RecursiveCast<Variable>(integrationVariable); variable != nullptr) {
        // Constant rule
        if (value != 0) {

            Add adder {
                Multiply<Real, Variable> { Real { value }, Variable { (*variable).GetName() } },
                Variable { "C" }
            };

            return std::move(adder.Accept(simplifyVisitor)).value();
        }

        // Zero rule
        return std::make_unique<Variable>(Variable { "C" });
    }

    Integral<Expression, Expression> integral { *(this->Copy()), *(integrationVariable.Copy()) };

    return integral.Copy();
}

} // namespace Oasis
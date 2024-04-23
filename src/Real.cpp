//
// Created by Matthew McCall on 7/2/23.
//

#include <string>

#include "Oasis/Add.hpp"
#include "Oasis/Integrate.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Variable.hpp"

namespace Oasis {

Real::Real(double value)
    : value(value)
{
}

auto Real::Differentiate(const Expression&) -> std::unique_ptr<Expression>
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

auto Real::ToString() const -> std::string
{
    return std::to_string(value);
}

auto Real::ToMathMLElement(tinyxml2::XMLDocument& doc) const -> tinyxml2::XMLElement*
{
    tinyxml2::XMLElement* const element = doc.NewElement("mn");
    element->SetText(ToString().c_str());
    return element;
}

auto Real::Specialize(const Expression& other) -> std::unique_ptr<Real>
{
    return other.Is<Real>() ? std::make_unique<Real>(dynamic_cast<const Real&>(other)) : nullptr;
}

auto Real::Specialize(const Expression& other, tf::Subflow&) -> std::unique_ptr<Real>
{
    return other.Is<Real>() ? std::make_unique<Real>(dynamic_cast<const Real&>(other)) : nullptr;
}

auto Real::IntegrateExp(const Expression& integrationVariable) -> std::unique_ptr<Expression>
{
    if (auto variable = Variable::Specialize(integrationVariable); variable != nullptr) {
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

    Integrate<Expression, Expression> integral { *(this->Copy()), *(integrationVariable.Copy()) };

    return integral.Copy();
}

} // namespace Oasis
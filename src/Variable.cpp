//
// Created by Matthew McCall on 8/15/23.
//

#include "Oasis/Variable.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Multiply.hpp"

namespace Oasis {

Variable::Variable(std::string name)
    : name(std::move(name))
{
}

auto Variable::Equals(const Expression& other) const -> bool
{
    return other.Is<Variable>() && name == dynamic_cast<const Variable&>(other).name;
}

auto Variable::GetName() const -> std::string
{
    return name;
}

auto Variable::ToString() const -> std::string
{
    return name;
}

auto Variable::Specialize(const Expression& other) -> std::unique_ptr<Variable>
{
    return other.Is<Variable>() ? std::make_unique<Variable>(dynamic_cast<const Variable&>(other)) : nullptr;
}

auto Variable::Specialize(const Expression& other, tf::Subflow&) -> std::unique_ptr<Variable>
{
    return other.Is<Variable>() ? std::make_unique<Variable>(dynamic_cast<const Variable&>(other)) : nullptr;
}

auto Variable::Integrate(const Variable& integrationVariable) -> std::unique_ptr<Expression>
{
    // Power rule
    if (name == integrationVariable.GetName()) {
        return std::make_unique<Add<Exponent<Variable, Real>, Variable>>(Add {
            Exponent { Variable { integrationVariable.GetName() }, Real { 2.0f } },
            Variable { "C" } });
    }

    // Different variable, treat as constant
    return std::make_unique<Add<Multiply<Variable, Variable>, Variable>>(Add {
        Multiply { Variable { name }, Variable { integrationVariable.GetName() } },
        Variable { "C" } });
}

} // Oasis
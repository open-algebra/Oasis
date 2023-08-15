//
// Created by Matthew McCall on 8/15/23.
//

#include "Oasis/Variable.hpp"

namespace Oasis {

Variable::Variable(std::string name) : name(std::move(name))
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

auto Variable::Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Variable>
{
    return other.Is<Variable>() ? std::make_unique<Variable>(dynamic_cast<const Variable&>(other)) : nullptr;
}

} // Oasis
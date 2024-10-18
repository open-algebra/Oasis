//
// Created by Matthew McCall on 8/15/23.
//

#include "Oasis/Variable.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"

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

auto Variable::Specialize(const Expression& other) -> std::unique_ptr<Variable>
{
    return other.Is<Variable>() ? std::make_unique<Variable>(dynamic_cast<const Variable&>(other)) : nullptr;
}

auto Variable::Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression>
{
    if (auto variable = Variable::Specialize(integrationVariable); variable != nullptr) {

        // Power rule
        if (name == (*variable).GetName()) {
            Add adder {
                Divide {
                    Exponent { Variable { (*variable).GetName() }, Real { 2.0f } },
                    Real { 2.0f } },
                Variable { "C" }
            };
            return adder.Simplify();
        }

        // Different variable, treat as constant
        Add adder {
            Multiply { Variable { name }, Variable { (*variable).GetName() } },
            Variable { "C" }
        };
        return adder.Simplify();
    }

    Integral<Expression, Expression> integral { *(this->Copy()), *(integrationVariable.Copy()) };

    return integral.Copy();
}

auto Variable::Substitute(const Expression& var, const Expression& val) -> std::unique_ptr<Expression>
{
    auto varclone = Variable::Specialize(var);
    if (varclone == nullptr) {
        throw std::invalid_argument("Variable was not a variable.");
    }
    if (varclone->GetName() == GetName()) {
        return val.Copy();
    }
    return Copy();
}

auto Variable::Differentiate(const Expression& differentiationVariable) const -> std::unique_ptr<Expression>
{
    if (auto variable = Variable::Specialize(differentiationVariable); variable != nullptr) {

        // Power rule
        if (name == (*variable).GetName()) {
            return std::make_unique<Real>(Real { 1.0f })
                ->Simplify();
        }

        // Different variable, treat as constant
        return std::make_unique<Real>(Real { 0 })
            ->Simplify();
    }

    return Copy();
}

} // Oasis
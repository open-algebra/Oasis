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
#include "Oasis/RecursiveCast.hpp"

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

auto Variable::Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression>
{
    SimplifyVisitor simplifyVisitor {};

    if (auto variable = RecursiveCast<Variable>(integrationVariable); variable != nullptr) {

        // Power rule
        if (name == (*variable).GetName()) {
            Add adder {
                Divide {
                    Exponent { Variable { (*variable).GetName() }, Real { 2.0f } },
                    Real { 2.0f } },
                Variable { "C" }
            };
            return adder.Accept(simplifyVisitor).value();
        }

        // Different variable, treat as constant
        Add adder {
            Multiply { Variable { name }, Variable { (*variable).GetName() } },
            Variable { "C" }
        };
        return adder.Accept(simplifyVisitor).value();
    }

    Integral<Expression, Expression> integral { *(this->Copy()), *(integrationVariable.Copy()) };

    return integral.Copy();
}

auto Variable::Substitute(const Expression& var, const Expression& val) -> std::unique_ptr<Expression>
{
    auto varclone = RecursiveCast<Variable>(var);
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
    SimplifyVisitor simplifyVisitor {};

    if (auto variable = RecursiveCast<Variable>(differentiationVariable); variable != nullptr) {

        // Power rule
        if (name == (*variable).GetName()) {
            return std::make_unique<Real>(Real { 1.0f })
                ->Accept(simplifyVisitor)
                .value();
        }

        // Different variable, treat as constant
        return std::make_unique<Real>(Real { 0 })
            ->Accept(simplifyVisitor)
            .value();
    }

    return Copy();
}

} // Oasis
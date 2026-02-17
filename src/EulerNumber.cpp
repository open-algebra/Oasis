//
// Created by Andrew Nazareth on 6/25/24.
//

#include "Oasis/EulerNumber.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Variable.hpp"
#include "numbers"
#include "string"
#include <cmath>

namespace Oasis {

auto EulerNumber::Equals(const Expression& other) const -> bool
{
    return other.Is<EulerNumber>();
}

auto EulerNumber::GetValue() -> double
{
    return std::numbers::e;
}

auto EulerNumber::Differentiate(const Expression&) const -> std::unique_ptr<Expression>
{
    return std::make_unique<Real>(0);
}

auto EulerNumber::Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression>
{
    SimplifyVisitor simplifyVisitor {};

    if (const auto variable = RecursiveCast<Variable>(integrationVariable); variable != nullptr) {
        // Constant rule
        Add adder {
            Multiply<EulerNumber, Variable> { EulerNumber {}, Variable { (*variable).GetName() } },
            Variable { "C" }
        };

        return std::move(adder.Accept(simplifyVisitor)).value();
    }

    Integral<Expression, Expression> integral { *(this->Copy()), *(integrationVariable.Copy()) };

    return integral.Copy();
}

}
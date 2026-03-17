//
// Created by Matthew McCall on 8/10/23.
//

#include "Oasis/Multiply.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Matrix.hpp"
#include "Oasis/RecursiveCast.hpp"

#define EPSILON 10E-6

namespace Oasis {
auto Multiply<Expression>::Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression>
{
    SimplifyVisitor simplifyVisitor {};
    // Single integration variable
    if (auto variable = RecursiveCast<Variable>(integrationVariable); variable != nullptr) {
        auto s = this->Accept(simplifyVisitor);
        if (!s) {
            return this->Generalize();
        }
        auto simplifiedMult = std::move(s).value();

        // Constant case - Constant number multiplied by integrand
        if (auto constant = RecursiveCast<Multiply<Real, Expression>>(*simplifiedMult); constant != nullptr) {
            auto exp = constant->GetLeastSigOp().Copy();
            auto num = constant->GetMostSigOp();
            auto integrated = exp->Integrate(integrationVariable);

            if (auto add = RecursiveCast<Add<Expression, Variable>>(*integrated); add != nullptr) {
                Add<Multiply<Real, Expression>, Variable> adder {
                    Multiply<Real, Expression> {
                        Real { num.GetValue() }, add->GetMostSigOp() },
                    Variable { "C" }
                };

                return adder.Accept(simplifyVisitor).value();
            }
        }

        // TODO: Implement integration by parts
    }
    Integral<Expression, Expression> integral { *(this->Copy()), *(integrationVariable.Copy()) };

    return integral.Copy();
}

} // Oasis
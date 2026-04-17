//
// Created by Matthew McCall on 8/10/23.
//

#include "Oasis/Multiply.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/EulerNumber.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Matrix.hpp"
#include "Oasis/RecursiveCast.hpp"
#include "Oasis/Subtract.hpp"

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

        // Apply Integration By Parts
        else {
            // Start by assuming u is MostSigOp and dv is LeastSigOp
            auto multiplyCopy = std::make_unique<Multiply<Expression, Expression>>(Multiply { this->GetMostSigOp(), this->GetLeastSigOp() });

            // Check by LIPET if it's necessary to swap operands so u is LeastSigOp and dv is MostSigOp
            // LIPET: Logarithm case
            if (!(multiplyCopy->GetMostSigOp().Is<Log<Expression, Expression>>())
                && multiplyCopy->GetLeastSigOp().Is<Log<Expression, Expression>>()) {
                multiplyCopy = std::make_unique<Multiply<Expression, Expression>>(multiplyCopy->SwapOperands());
            }

            // LIPET: Inverse trigonometry is not implemented yet in Oasis

            // LIPET: Swap polynomial (linear variable) and EulerNumber
            if ((multiplyCopy->GetLeastSigOp().Is<Variable>()
                    && multiplyCopy->GetMostSigOp().Is<Exponent<EulerNumber, Expression>>())) {
                multiplyCopy = std::make_unique<Multiply<Expression, Expression>>(multiplyCopy->SwapOperands());
            }

            // LIPET: Swap polynomial (exponential) and EulerNumber
            if (auto polynomial = RecursiveCast<Exponent<Variable, Real>>(multiplyCopy->GetLeastSigOp()); polynomial != nullptr) {
                if (auto euler = RecursiveCast<Exponent<EulerNumber, Variable>>(multiplyCopy->GetMostSigOp()); euler != nullptr) {
                    multiplyCopy = std::make_unique<Multiply<Expression, Expression>>(multiplyCopy->SwapOperands());
                }
            }

            // LIPET: Trigonometry is not implemented yet in Oasis

            // Integrate dv and differentiate u to attain v and du
            auto v = multiplyCopy->GetLeastSigOp().Integrate(integrationVariable);
            auto du = multiplyCopy->GetMostSigOp().Differentiate(integrationVariable);

            // Remove the +C from v
            auto vPlusC = RecursiveCast<Add<Expression, Variable>>(*v);
            v = vPlusC->GetMostSigOp().Copy();

            // Initialize the coefficients of v and du to 1
            auto vCoefficient = std::make_unique<Real>(Real { 1 });
            auto duCoefficient = std::make_unique<Real>(Real { 1 });

            // Make a copy of v so that the coefficient can be factored out when computing vdu,
            // while preserving the original v for the multiplication of u*v
            auto v_VDU = v->Copy();

            // Factor out the coefficient for v; v_VDU now has a coefficient of one
            if (auto coefficientMultiply = RecursiveCast<Multiply<Real, Expression>>(*v); coefficientMultiply != nullptr) {
                vCoefficient = RecursiveCast<Real>(coefficientMultiply->GetMostSigOp());
                v_VDU = coefficientMultiply->GetLeastSigOp().Accept(simplifyVisitor).value();
            }

            // Factor out the coefficient for du; du now has a coefficient of one
            if (auto coefficientMultiply = RecursiveCast<Multiply<Real, Expression>>(*du); coefficientMultiply != nullptr) {
                duCoefficient = RecursiveCast<Real>(coefficientMultiply->GetMostSigOp());
                du = coefficientMultiply->GetLeastSigOp().Accept(simplifyVisitor).value();
            }

            // Attain a constant for the coefficient of vdu
            auto vduCoefficient = std::make_unique<Multiply<Expression, Expression>>(Multiply { *vCoefficient, *duCoefficient })->Accept(simplifyVisitor).value();

            // Multiply v and du to attain partVDU, which is vdu with a coefficient of one
            auto partVDU = std::make_unique<Multiply<Expression, Expression>>(Multiply { *v_VDU, *du })->Accept(simplifyVisitor).value();

            // Prevent infinite recursion
            // If fullVDU is equal to the original Multiply instance,
            // then IBP cannot reduce the integrand, so return an integral of the Multiply instance
            auto fullVDU = std::make_unique<Multiply<Expression, Expression>>(Multiply { *vduCoefficient, *partVDU });
            if (multiplyCopy->Equals(*fullVDU)) {
                Integral<Expression, Expression> integral { *(multiplyCopy->Copy()), *(integrationVariable.Copy()) };
                return integral.Copy();
            }

            // If partVDU is a Multiply instance it will use IBP again,
            // otherwise it will find another integration method to use.
            // Implicit base case: IBP will eventually reduce the integrand such that it is not a Multiply instance.
            auto integratedPartVDU = partVDU->Integrate(integrationVariable);

            // Remove the +C from the integration result
            if (auto removeC = RecursiveCast<Add<Expression, Expression>>(*integratedPartVDU); removeC != nullptr) {
                integratedPartVDU = removeC->GetMostSigOp().Copy();
            }

            // Apply coefficient correction for integratedPartVDU, thus building integratedFullVDU
            auto integratedFullVDU = std::make_unique<Multiply<Expression, Expression>>(Multiply { *vduCoefficient, *integratedPartVDU });

            // Apply the Integration By Parts formula, and add the +C at the end
            Add<Subtract<Multiply<Expression, Expression>, Expression>, Variable> adder {
                Subtract<Multiply<Expression, Expression>, Expression> {
                    Multiply<Expression, Expression> { multiplyCopy->GetMostSigOp(), *v },
                    *integratedFullVDU },
                Variable { "C" }
            };

            return adder.Accept(simplifyVisitor).value();
        }
    }
    Integral<Expression, Expression> integral { *(this->Copy()), *(integrationVariable.Copy()) };

    return integral.Copy();
}

} // Oasis
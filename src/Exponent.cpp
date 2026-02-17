//
// Created by Andrew Nazareth on 9/19/23.
//

#include <cmath>

#include "Oasis/Add.hpp"
#include "Oasis/Derivative.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/EulerNumber.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/MatchCast.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Negate.hpp"
#include "Oasis/RecursiveCast.hpp"

namespace Oasis {
Exponent<Expression>::Exponent(const Expression& base, const Expression& power)
    : BinaryExpression(base, power)
{
}

auto Exponent<Expression>::Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression>
{
    SimplifyVisitor simplifyVisitor {};
    // variable integration
    if (auto variable = RecursiveCast<Variable>(integrationVariable); variable != nullptr) {
        auto simplified = this->Accept(simplifyVisitor);
        if (!simplified) {
            return this->Generalize();
        }
        auto simplifiedExponent = std::move(simplified).value();

        std::unique_ptr<Expression> integral;
        // Variable with a constant power
        if (auto realExponent = RecursiveCast<Exponent<Variable, Real>>(*simplifiedExponent); realExponent != nullptr) {
            const Variable& expBase = realExponent->GetMostSigOp();
            const Real& expPow = realExponent->GetLeastSigOp();

            if ((*variable).GetName() == expBase.GetName()) {
                Add adder {
                    Divide {
                        Exponent<Variable, Real> { Variable { (*variable).GetName() }, Real { expPow.GetValue() + 1 } },
                        Real { expPow.GetValue() + 1 } },
                    Variable { "C" }
                };

                return adder.Accept(simplifyVisitor).value();
            }
        }
    }

    Integral<Expression, Expression> integral { *(this->Copy()), *(integrationVariable.Copy()) };

    return integral.Copy();
}

auto Exponent<Expression>::Differentiate(
    const Expression& differentiationVariable) const -> std::unique_ptr<Expression>
{
    // variable diff
    SimplifyVisitor simplifyVisitor {};
    if (auto variable = RecursiveCast<Variable>(differentiationVariable); variable != nullptr) {
        auto simplified = this->Accept(simplifyVisitor);
        if (!simplified) {
            return this->Generalize();
        }
        auto simplifiedExponent = std::move(simplified).value();

        std::unique_ptr<Expression> diff;
        // Variable with a constant power
        if (auto realExponent = RecursiveCast<Exponent<Variable, Real>>(*simplifiedExponent); realExponent != nullptr) {
            const Variable& expBase = realExponent->GetMostSigOp();
            const Real& expPow = realExponent->GetLeastSigOp();

            if (variable->GetName() == expBase.GetName()) {
                return Multiply {
                    Real { expPow.GetValue() },
                    Exponent {
                        Variable { variable->GetName() },
                        Real { expPow.GetValue() - 1 } }
                }
                    .Accept(simplifyVisitor)
                    .value();
            }
        }

        if (auto natBase = RecursiveCast<Exponent<EulerNumber, Expression>>(*simplifiedExponent); natBase != nullptr) {
            Multiply derivative { Derivative { natBase->GetLeastSigOp(), differentiationVariable }, *simplifiedExponent };
            return derivative.Accept(simplifyVisitor).value();
        }

        if (auto realBase = RecursiveCast<Exponent<Real, Expression>>(*simplifiedExponent); realBase != nullptr) {
            Multiply derivative {
                Multiply { Derivative { realBase->GetLeastSigOp(), differentiationVariable }, *simplifiedExponent },
                Log { EulerNumber {}, realBase->GetMostSigOp() }
            };
            return derivative.Accept(simplifyVisitor).value();
        }

        if (auto varBase = RecursiveCast<Exponent<Variable, Expression>>(*simplifiedExponent); varBase != nullptr) {
            const Variable& expBase = varBase->GetMostSigOp();
            if (expBase.GetName() != variable->GetName()) {
                Multiply derivative {
                    Multiply { Derivative { varBase->GetLeastSigOp(), differentiationVariable }, *simplifiedExponent },
                    Log { EulerNumber {}, varBase->GetMostSigOp() }
                };
                return derivative.Accept(simplifyVisitor).value();
            }
        }

        if (auto generalCase = RecursiveCast<Exponent<Expression, Expression>>(*simplifiedExponent); generalCase != nullptr) {
            Multiply derivative {
                Multiply {
                    *simplifiedExponent,
                    Add {
                        Divide {
                            Multiply {
                                generalCase->GetLeastSigOp(),
                                Derivative { generalCase->GetMostSigOp(), differentiationVariable } },
                            generalCase->GetMostSigOp() },
                        Multiply {
                            Derivative { generalCase->GetLeastSigOp(), differentiationVariable },
                            Log { EulerNumber {}, generalCase->GetMostSigOp() } } } }
            };
            return derivative.Accept(simplifyVisitor).value();
        }
    }

    return Derivative { *this, differentiationVariable }.Copy();
}
} // Oasis
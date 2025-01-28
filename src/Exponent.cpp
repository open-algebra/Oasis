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
#include "Oasis/RecursiveCast.hpp"

namespace Oasis {

Exponent<Expression>::Exponent(const Expression& base, const Expression& power)
    : BinaryExpression(base, power)
{
}

auto Exponent<Expression>::Simplify() const -> std::unique_ptr<Expression>
{
    static auto match_cast = MatchCast<Expression>()
                                 .Case([](const Exponent<Expression, Real>& zeroCase) -> std::unique_ptr<Expression> {
                                     if (const Real& power = zeroCase.GetLeastSigOp(); power.GetValue() == 0.0)
                                         return std::make_unique<Real>(1.0);
                                     return {};
                                 })
                                 .Case([](const Exponent<Real, Expression>& zeroCase) -> std::unique_ptr<Expression> {
                                     if (const Real& base = zeroCase.GetMostSigOp(); base.GetValue() == 0.0)
                                         return std::make_unique<Real>(0.0);
                                     return {};
                                 })
                                 .Case([](const Exponent<Real>& realCase) -> std::unique_ptr<Expression> {
                                     const Real& base = realCase.GetMostSigOp();
                                     const Real& power = realCase.GetLeastSigOp();
                                     return std::make_unique<Real>(pow(base.GetValue(), power.GetValue()));
                                 })
                                 .Case([](const Exponent<Expression, Real>& oneCase) -> std::unique_ptr<Expression> {
                                     if (const Real& power = oneCase.GetLeastSigOp(); power.GetValue() == 1.0)
                                         return oneCase.GetMostSigOp().Copy();
                                     return {};
                                 })
                                 .Case([](const Exponent<Real, Expression>& oneCase) -> std::unique_ptr<Expression> {
                                     if (const Real& base = oneCase.GetMostSigOp(); base.GetValue() == 1.0)
                                         return std::make_unique<Real>(1.0);
                                     return {};
                                 })
                                 .Case([](const Exponent<Imaginary, Real>& ImgCase) -> std::unique_ptr<Expression> {
                                     const auto power = std::fmod((ImgCase.GetLeastSigOp()).GetValue(), 4);
                                     switch (static_cast<int>(power)) {
                                     case 0:
                                         return std::make_unique<Real>(1);
                                     case 1:
                                         return std::make_unique<Imaginary>();
                                     case 2:
                                         return std::make_unique<Real>(-1);
                                     case 3:
                                         return std::make_unique<Multiply<Real, Imaginary>>(Real { -1 }, Imaginary {});
                                     default:
                                         return {};
                                     }
                                 })
                                 .Case([](const Exponent<Multiply<Real, Expression>, Real>& ImgCase) -> std::unique_ptr<Expression> {
                                     if (ImgCase.GetMostSigOp().GetMostSigOp().GetValue() < 0 && ImgCase.GetLeastSigOp().GetValue() == 0.5) {
                                         return std::make_unique<Multiply<>>(
                                             Multiply { Real { pow(std::abs(ImgCase.GetMostSigOp().GetMostSigOp().GetValue()), 0.5) },
                                                 Exponent { ImgCase.GetMostSigOp().GetLeastSigOp(), Real { 0.5 } } },
                                             Imaginary {});
                                     }
                                     return {};
                                 })
                                 .Case([](const Exponent<Exponent, Expression>& expExpCase) -> std::unique_ptr<Expression> {
                                     return std::make_unique<Exponent>(expExpCase.GetMostSigOp().GetMostSigOp(),
                                         *(Multiply { expExpCase.GetMostSigOp().GetLeastSigOp(), expExpCase.GetLeastSigOp() }.Simplify()));
                                 })
                                 .Case([](const Exponent<Expression, Log<>>& logCase) -> std::unique_ptr<Expression> {
                                     if (logCase.GetMostSigOp().Equals(logCase.GetLeastSigOp().GetMostSigOp())) {
                                         return logCase.GetLeastSigOp().GetLeastSigOp().Copy();
                                     }
                                     return {};
                                 });

    const auto simplifiedBase = mostSigOp->Simplify();
    const auto simplifiedPower = leastSigOp->Simplify();

    const Exponent simplifiedExponent { *simplifiedBase, *simplifiedPower };

    return match_cast.Execute(simplifiedExponent, simplifiedExponent.Copy());
}

auto Exponent<Expression>::Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression>
{
    // variable integration
    if (auto variable = RecursiveCast<Variable>(integrationVariable); variable != nullptr) {
        auto simplifiedExponent = this->Simplify();

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

                return adder.Simplify();
            }
        }
    }

    Integral<Expression, Expression> integral { *(this->Copy()), *(integrationVariable.Copy()) };

    return integral.Copy();
}

auto Exponent<Expression>::Differentiate(const Expression& differentiationVariable) const -> std::unique_ptr<Expression>
{
    // variable diff
    if (auto variable = RecursiveCast<Variable>(differentiationVariable); variable != nullptr) {
        auto simplifiedExponent = this->Simplify();

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
                    .Simplify();
            }
        }

        if (auto natBase = RecursiveCast<Exponent<EulerNumber, Expression>>(*simplifiedExponent); natBase != nullptr) {
            Multiply derivative { Derivative { natBase->GetLeastSigOp(), differentiationVariable }, *simplifiedExponent };
            return derivative.Simplify();
        }

        if (auto realBase = RecursiveCast<Exponent<Real, Expression>>(*simplifiedExponent); realBase != nullptr) {
            Multiply derivative { Multiply { Derivative { realBase->GetLeastSigOp(), differentiationVariable }, *simplifiedExponent }, Log { EulerNumber {}, realBase->GetMostSigOp() } };
            return derivative.Simplify();
        }

        if (auto samevarBase = RecursiveCast<Exponent<Variable, Expression>>(*simplifiedExponent); samevarBase != nullptr) {
            auto expBase = RecursiveCast<Variable>(samevarBase->GetMostSigOp());
            if(expBase->GetName() == variable->GetName()) {
                Multiply derivative { Multiply { Derivative { samevarBase->GetLeastSigOp(), differentiationVariable }, *simplifiedExponent }, Add {Log { EulerNumber {}, samevarBase->GetMostSigOp() }, Real{1} } };
                return derivative.Simplify();
            }
        }

        if (auto varBase = RecursiveCast<Exponent<Variable, Expression>>(*simplifiedExponent); varBase != nullptr) {
            Multiply derivative { Multiply { Derivative { varBase->GetLeastSigOp(), differentiationVariable }, *simplifiedExponent }, Log { EulerNumber {}, varBase->GetMostSigOp() } };
            return derivative.Simplify();
        }

        if (auto generalBase = RecursiveCast<Exponent<Expression, Expression>>(*simplifiedExponent); generalBase != nullptr) {
            Multiply derivative { Multiply { *simplifiedExponent ,
                                    Add { Divide { Multiply { generalBase->GetLeastSigOp() , Derivative {generalBase->GetMostSigOp()  , differentiationVariable }  }, generalBase->GetMostSigOp() }  ,
                                        Multiply { Derivative { generalBase->GetLeastSigOp(), differentiationVariable } , Log { EulerNumber {} , generalBase->GetMostSigOp() } } } } };
            return derivative.Simplify();
        }
    }

    return Derivative { *this, differentiationVariable }.Copy();
}

} // Oasis

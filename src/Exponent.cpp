//
// Created by Andrew Nazareth on 9/19/23.
//

#include <cmath>

#include "Oasis/Add.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"

#include "Oasis/Derivative.hpp"
#include "Oasis/EulerNumber.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Multiply.hpp"

namespace Oasis {

Exponent<Expression>::Exponent(const Expression& base, const Expression& power)
    : BinaryExpression(base, power)
{
}

auto Exponent<Expression>::Simplify() const -> std::unique_ptr<Expression>
{
    auto simplifiedBase = mostSigOp->Simplify();
    auto simplifiedPower = leastSigOp->Simplify();

    Exponent simplifiedExponent { *simplifiedBase, *simplifiedPower };

    if (auto zeroCase = Exponent<Expression, Real>::Specialize(simplifiedExponent); zeroCase != nullptr) {
        const Real& power = zeroCase->GetLeastSigOp();

        if (power.GetValue() == 0.0) {
            return std::make_unique<Real>(1.0);
        }
    }

    if (auto zeroCase = Exponent<Real, Expression>::Specialize(simplifiedExponent); zeroCase != nullptr) {
        const Real& base = zeroCase->GetMostSigOp();

        if (base.GetValue() == 0.0) {
            return std::make_unique<Real>(0.0);
        }
    }

    if (auto realCase = Exponent<Real>::Specialize(simplifiedExponent); realCase != nullptr) {
        const Real& base = realCase->GetMostSigOp();
        const Real& power = realCase->GetLeastSigOp();

        return std::make_unique<Real>(pow(base.GetValue(), power.GetValue()));
    }

    if (auto oneCase = Exponent<Expression, Real>::Specialize(simplifiedExponent); oneCase != nullptr) {
        const Real& power = oneCase->GetLeastSigOp();
        if (power.GetValue() == 1.0) {
            return oneCase->GetMostSigOp().Copy();
        }
    }

    if (auto oneCase = Exponent<Real, Expression>::Specialize(simplifiedExponent); oneCase != nullptr) {
        const Real& base = oneCase->GetMostSigOp();
        if (base.GetValue() == 1.0) {
            return std::make_unique<Real>(1.0);
        }
    }

    if (auto ImgCase = Exponent<Imaginary, Real>::Specialize(simplifiedExponent); ImgCase != nullptr) {
        const auto power = std::fmod((ImgCase->GetLeastSigOp()).GetValue(), 4);
        if (power == 1) {
            return std::make_unique<Imaginary>();
        } else if (power == 2) {
            return std::make_unique<Real>(-1);
        } else if (power == 0) {
            return std::make_unique<Real>(1);
        } else if (power == 3) {
            return std::make_unique<Multiply<Real, Imaginary>>(Real { -1 }, Imaginary {});
        }
    }

    if (auto ImgCase = Exponent<Multiply<Real, Expression>, Real>::Specialize(simplifiedExponent); ImgCase != nullptr) {
        if (ImgCase->GetMostSigOp().GetMostSigOp().GetValue() < 0 && ImgCase->GetLeastSigOp().GetValue() == 0.5) {
            return std::make_unique<Multiply<Expression>>(
                Multiply<Expression> { Real { pow(std::abs(ImgCase->GetMostSigOp().GetMostSigOp().GetValue()), 0.5) },
                    Exponent<Expression> { ImgCase->GetMostSigOp().GetLeastSigOp(), Real { 0.5 } } },
                Imaginary {});
        }
    }

    if (auto expExpCase = Exponent<Exponent<Expression, Expression>, Expression>::Specialize(simplifiedExponent); expExpCase != nullptr) {
        return std::make_unique<Exponent<Expression>>(expExpCase->GetMostSigOp().GetMostSigOp(),
            *(Multiply { expExpCase->GetMostSigOp().GetLeastSigOp(), expExpCase->GetLeastSigOp() }.Simplify()));
    }

    // a^log[a](x) = x - maybe add domain stuff (should only be defined for x >= 0)
    if (auto logCase = Exponent<Expression, Log<Expression, Expression>>::Specialize(simplifiedExponent); logCase != nullptr) {
        if (logCase->GetMostSigOp().Equals(logCase->GetLeastSigOp().GetMostSigOp())) {
            return Expression::Specialize(logCase->GetLeastSigOp().GetLeastSigOp());
        }
    }

    return simplifiedExponent.Copy();
}

auto Exponent<Expression>::Specialize(const Oasis::Expression& other) -> std::unique_ptr<Exponent<Expression, Expression>>
{
    if (!other.Is<Oasis::Exponent>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize();
    return std::make_unique<Exponent>(dynamic_cast<const Exponent&>(*otherGeneralized));
}

auto Exponent<Expression>::Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression>
{
    // variable integration
    if (auto variable = Variable::Specialize(integrationVariable); variable != nullptr) {
        auto simplifiedExponent = this->Simplify();

        std::unique_ptr<Expression> integral;
        // Variable with a constant power
        if (auto realExponent = Exponent<Variable, Real>::Specialize(*simplifiedExponent); realExponent != nullptr) {
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
    if (auto variable = Variable::Specialize(differentiationVariable); variable != nullptr) {
        auto simplifiedExponent = this->Simplify();

        std::unique_ptr<Expression> diff;
        // Variable with a constant power
        if (auto realExponent = Exponent<Variable, Real>::Specialize(*simplifiedExponent); realExponent != nullptr) {
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

        if (auto natBase = Exponent<EulerNumber, Expression>::Specialize(*simplifiedExponent); natBase != nullptr) {
            Multiply derivative { Derivative { natBase->GetLeastSigOp(), differentiationVariable }, *simplifiedExponent };
            return derivative.Simplify();
        }

        if (auto realBase = Exponent<Real, Expression>::Specialize(*simplifiedExponent); realBase != nullptr) {
            Multiply derivative { Multiply { Derivative { realBase->GetLeastSigOp(), differentiationVariable }, *simplifiedExponent }, Log { EulerNumber {}, realBase->GetMostSigOp() } };
            return derivative.Simplify();
        }

        if (auto varBase = Exponent<Variable, Expression>::Specialize(*simplifiedExponent); varBase != nullptr) {
            Multiply derivative { Multiply { Derivative { varBase->GetLeastSigOp(), differentiationVariable }, *simplifiedExponent }, Log { EulerNumber {}, varBase->GetMostSigOp() } };
            return derivative.Simplify();
        }
    }

    return Derivative { *this, differentiationVariable }.Copy();
}

} // Oasis

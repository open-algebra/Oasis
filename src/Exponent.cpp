//
// Created by Andrew Nazareth on 9/19/23.
//

#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Multiply.hpp"
#include <cmath>

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

auto Exponent<Expression>::ToString() const -> std::string
{
    return fmt::format("({}^{})", mostSigOp->ToString(), leastSigOp->ToString());
}

auto Exponent<Expression>::Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression>
{
    std::unique_ptr<Expression> simplifiedBase, simplifiedPower;

    tf::Task baseSimplifyTask = subflow.emplace([this, &simplifiedBase](tf::Subflow& sbf) {
        if (!mostSigOp) {
            return;
        }
        simplifiedBase = mostSigOp->Simplify(sbf);
    });

    tf::Task powerSimplifyTask = subflow.emplace([this, &simplifiedPower](tf::Subflow& sbf) {
        if (!leastSigOp) {
            return;
        }

        simplifiedPower = leastSigOp->Simplify(sbf);
    });

    Exponent simplifiedExponent;

    tf::Task simplifyTask = subflow.emplace([&simplifiedExponent, &simplifiedBase, &simplifiedPower](tf::Subflow&) {
        if (simplifiedPower) {
            simplifiedExponent.SetMostSigOp(*simplifiedBase);
        }

        if (simplifiedPower) {
            simplifiedExponent.SetLeastSigOp(*simplifiedPower);
        }
    });

    simplifyTask.succeed(baseSimplifyTask, powerSimplifyTask);

    std::unique_ptr<Exponent<Real>> realCase;

    tf::Task realCaseTask = subflow.emplace([&simplifiedExponent, &realCase](tf::Subflow& sbf) {
        realCase = Exponent<Real>::Specialize(simplifiedExponent, sbf);
    });

    simplifyTask.precede(realCaseTask);

    subflow.join();

    if (realCase) {
        const Real& base = realCase->GetMostSigOp();
        const Real& power = realCase->GetLeastSigOp();

        return std::make_unique<Real>(pow(base.GetValue(), power.GetValue()));
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

auto Exponent<Expression>::Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Exponent>
{
    if (!other.Is<Oasis::Exponent>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize(subflow);
    return std::make_unique<Exponent>(dynamic_cast<const Exponent&>(*otherGeneralized));
}

} // Oasis

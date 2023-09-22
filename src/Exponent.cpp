//
// Created by Andrew Nazareth on 9/19/23.
//

#include "Oasis/Exponent.hpp"
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

    if (auto realCase = Exponent<Real>::Specialize(simplifiedExponent); realCase != nullptr) {
        const Real& base = realCase->GetMostSigOp();
        const Real& power = realCase->GetLeastSigOp();

        return std::make_unique<Real>(pow(base.GetValue(), power.GetValue()));
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

    tf::Task simplifyTask = subflow.emplace([&simplifiedExponent, &simplifiedBase, &simplifiedPower](tf::Subflow& sbf) {
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
    if (!other.Is<Exponent>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize();
    return std::make_unique<Exponent>(dynamic_cast<const Exponent&>(*otherGeneralized));
}

auto Exponent<Expression>::Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Exponent>
{
    if (!other.Is<Exponent>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize(subflow);
    return std::make_unique<Exponent>(dynamic_cast<const Exponent&>(*otherGeneralized));
}

} // Oasis

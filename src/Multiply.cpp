//
// Created by Matthew McCall on 8/10/23.
//

#include "Oasis/Multiply.hpp"

namespace Oasis {

Multiply<Expression>::Multiply(const Expression& minuend, const Expression& subtrahend)
    : BinaryExpression(minuend, subtrahend)
{
}

auto Multiply<Expression>::Simplify() const -> std::unique_ptr<Expression>
{
    auto simplifiedMultiplicand = mostSigOp->Simplify();
    auto simplifiedMultiplier = leastSigOp->Simplify();

    Multiply simplifiedMultiply { *simplifiedMultiplicand, *simplifiedMultiplier };

    if (auto realCase = Multiply<Real>::Specialize(simplifiedMultiply); realCase != nullptr) {
        const Real& multiplicand = realCase->GetMostSigOp();
        const Real& multiplier = realCase->GetLeastSigOp();

        return std::make_unique<Real>(multiplicand.GetValue() * multiplier.GetValue());
    }

    return simplifiedMultiply.Copy();
}

auto Multiply<Expression>::ToString() const -> std::string
{
    return fmt::format("({} * {})", mostSigOp->ToString(), leastSigOp->ToString());
}

auto Multiply<Expression>::Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression>
{
    std::unique_ptr<Expression> simplifiedMultiplicand, simplifiedMultiplier;

    tf::Task leftSimplifyTask = subflow.emplace([this, &simplifiedMultiplicand](tf::Subflow& sbf) {
        if (!mostSigOp) {
            return;
        }

        simplifiedMultiplicand = mostSigOp->Simplify(sbf);
    });

    tf::Task rightSimplifyTask = subflow.emplace([this, &simplifiedMultiplier](tf::Subflow& sbf) {
        if (!leastSigOp) {
            return;
        }

        simplifiedMultiplier = leastSigOp->Simplify(sbf);
    });

    Multiply simplifiedMultiply;

    // While this task isn't actually parallelized, it exists as a prerequisite for check possible cases in parallel
    tf::Task simplifyTask = subflow.emplace([&simplifiedMultiply, &simplifiedMultiplicand, &simplifiedMultiplier](tf::Subflow& sbf) {
        if (simplifiedMultiplicand) {
            simplifiedMultiply.SetMostSigOp(*simplifiedMultiplicand);
        }

        if (simplifiedMultiplier) {
            simplifiedMultiply.SetLeastSigOp(*simplifiedMultiplier);
        }
    });

    simplifyTask.succeed(leftSimplifyTask, rightSimplifyTask);

    std::unique_ptr<Multiply<Real>> realCase;

    tf::Task realCaseTask = subflow.emplace([&simplifiedMultiply, &realCase](tf::Subflow& sbf) {
        realCase = Multiply<Real>::Specialize(simplifiedMultiply, sbf);
    });

    simplifyTask.precede(realCaseTask);

    subflow.join();

    if (realCase) {
        const Real& multiplicand = realCase->GetMostSigOp();
        const Real& multiplier = realCase->GetLeastSigOp();

        return std::make_unique<Real>(multiplicand.GetValue() * multiplier.GetValue());
    }

    return simplifiedMultiply.Copy();
}

auto Multiply<Expression>::Specialize(const Expression& other) -> std::unique_ptr<Multiply<Expression, Expression>>
{
    if (!other.Is<Multiply>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize();
    return std::make_unique<Multiply>(dynamic_cast<const Multiply&>(*otherGeneralized));
}

auto Multiply<Expression>::Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Multiply>
{
    if (!other.Is<Multiply>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize(subflow);
    return std::make_unique<Multiply>(dynamic_cast<const Multiply&>(*otherGeneralized));
}

} // Oasis
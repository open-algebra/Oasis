//
// Created by Matthew McCall on 7/2/23.
//
#include <unordered_map>

#include "Oasis/Add.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Multiply.hpp"

namespace Oasis {

Add<Expression>::Add(const Expression& addend1, const Expression& addend2)
    : BinaryExpression(addend1, addend2)
{
}

auto Add<Expression>::Simplify() const -> std::unique_ptr<Expression>
{
    auto simplifiedAugend = mostSigOp ? mostSigOp->Simplify() : nullptr;
    auto simplifiedAddend = leastSigOp ? leastSigOp->Simplify() : nullptr;

    Add simplifiedAdd { *simplifiedAugend, *simplifiedAddend };

    if (auto realCase = Add<Real>::Specialize(simplifiedAdd); realCase != nullptr) {
        const Real& firstReal = realCase->GetMostSigOp();
        const Real& secondReal = realCase->GetLeastSigOp();

        return std::make_unique<Real>(firstReal.GetValue() + secondReal.GetValue());
    }

    if (auto likeTermsCase = Add<Multiply<Real, Expression>>::Specialize(simplifiedAdd); likeTermsCase != nullptr) {
        const Oasis::IExpression auto& leftTerm = likeTermsCase->GetMostSigOp().GetLeastSigOp();
        const Oasis::IExpression auto& rightTerm = likeTermsCase->GetLeastSigOp().GetLeastSigOp();

        if (leftTerm.Equals(rightTerm)) {
            const Real& coefficient1 = likeTermsCase->GetMostSigOp().GetMostSigOp();
            const Real& coefficient2 = likeTermsCase->GetLeastSigOp().GetMostSigOp();

            return std::make_unique<Multiply<Expression>>(Real(coefficient1.GetValue() + coefficient2.GetValue()), leftTerm);
        }
    }

    // 2i + i
    if (auto ImgCase = Add<Multiply<Expression, Imaginary>, Imaginary>::Specialize(simplifiedAdd); ImgCase != nullptr) {
        return std::make_unique<Multiply<Expression>>(
            *(Add { Real { 1.0 }, ImgCase->GetMostSigOp().GetMostSigOp() }.Simplify()), Imaginary {});
    }

    if (auto ImgCase = Add<Multiply<Expression, Imaginary>, Multiply<Expression, Imaginary>>::Specialize(simplifiedAdd); ImgCase != nullptr) {
        return std::make_unique<Multiply<Expression>>(
            *(Add { ImgCase->GetLeastSigOp().GetMostSigOp(), ImgCase->GetMostSigOp().GetMostSigOp() }.Simplify()), Imaginary {});
    }

    // log(a) + log(b) = log(ab)
    if (auto logCase = Add<Log<Expression, Expression>, Log<Expression, Expression>>::Specialize(simplifiedAdd); logCase != nullptr) {
        if (logCase->GetMostSigOp().GetMostSigOp().Equals(logCase->GetLeastSigOp().GetMostSigOp())) {
            const IExpression auto& base = logCase->GetMostSigOp().GetMostSigOp();
            const IExpression auto& argument = Multiply<Expression>({ logCase->GetMostSigOp().GetLeastSigOp(), logCase->GetLeastSigOp().GetLeastSigOp() });
            return std::make_unique<Log<Expression>>(base, argument);
        }
    }

    // x + x = 2x
    if (simplifiedAugend->Equals(*simplifiedAddend)) {
        return std::make_unique<Multiply<Real, Expression>>(Real { 2.0 }, *simplifiedAugend);
    }

    // 2x + x = 3x
    if (const auto likeTermsCase2 = Add<Multiply<Real, Expression>, Expression>::Specialize(simplifiedAdd); likeTermsCase2 != nullptr) {
        const Real& coeffiecent = likeTermsCase2->GetMostSigOp().GetMostSigOp();
        return std::make_unique<Multiply<Real, Expression>>(Real { coeffiecent.GetValue() + 1 }, likeTermsCase2->GetMostSigOp());
    }

    return simplifiedAdd.Copy();
}

auto Add<Expression>::ToString() const -> std::string
{
    return fmt::format("({} + {})", mostSigOp->ToString(), leastSigOp->ToString());
}

auto Add<Expression>::Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression>
{
    std::unique_ptr<Expression> simplifiedAugend, simplifiedAddend;

    tf::Task leftSimplifyTask = subflow.emplace([this, &simplifiedAugend](tf::Subflow& sbf) {
        if (!mostSigOp) {
            return;
        }

        simplifiedAugend = mostSigOp->Simplify(sbf);
    });

    tf::Task rightSimplifyTask = subflow.emplace([this, &simplifiedAddend](tf::Subflow& sbf) {
        if (!leastSigOp) {
            return;
        }

        simplifiedAddend = leastSigOp->Simplify(sbf);
    });

    Add simplifiedAdd;

    // While this task isn't actually parallelized, it exists as a prerequisite for check possible cases in parallel
    tf::Task simplifyTask = subflow.emplace([&simplifiedAdd, &simplifiedAugend, &simplifiedAddend](tf::Subflow&) {
        if (simplifiedAugend) {
            simplifiedAdd.SetMostSigOp(*simplifiedAugend);
        }

        if (simplifiedAddend) {
            simplifiedAdd.SetLeastSigOp(*simplifiedAddend);
        }
    });

    simplifyTask.succeed(leftSimplifyTask, rightSimplifyTask);

    std::unique_ptr<Add<Real>> realCase;

    tf::Task realCaseTask = subflow.emplace([&simplifiedAdd, &realCase](tf::Subflow& sbf) {
        realCase = Add<Real>::Specialize(simplifiedAdd, sbf);
    });

    simplifyTask.precede(realCaseTask);

    subflow.join();

    if (realCase) {
        const Real& firstReal = realCase->GetMostSigOp();
        const Real& secondReal = realCase->GetLeastSigOp();

        return std::make_unique<Real>(firstReal.GetValue() + secondReal.GetValue());
    }

    return simplifiedAdd.Copy();
}

auto Add<Expression>::Specialize(const Expression& other) -> std::unique_ptr<Add<Expression, Expression>>
{
    if (!other.Is<Oasis::Add>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize();
    return std::make_unique<Add>(dynamic_cast<const Add&>(*otherGeneralized));
}

auto Add<Expression>::Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Add>
{
    if (!other.Is<Oasis::Add>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize(subflow);
    return std::make_unique<Add>(dynamic_cast<const Add&>(*otherGeneralized));
}

} // Oasis
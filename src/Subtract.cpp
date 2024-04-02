//
// Created by Matthew McCall on 8/10/23.
//

#include "Oasis/Subtract.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Variable.hpp"
#include "Oasis/Add.hpp"

namespace Oasis {

Subtract<Expression>::Subtract(const Expression& minuend, const Expression& subtrahend)
    : BinaryExpression(minuend, subtrahend)
{
}

auto Subtract<Expression>::Simplify() const -> std::unique_ptr<Expression>
{
    auto simplifiedMinuend = mostSigOp ? mostSigOp->Simplify() : nullptr;
    auto simplifiedSubtrahend = leastSigOp ? leastSigOp->Simplify() : nullptr;

    Subtract simplifiedSubtract { *simplifiedMinuend, *simplifiedSubtrahend };

    if (auto realCase = Subtract<Real>::Specialize(simplifiedSubtract); realCase != nullptr) {
        const Real& minuend = realCase->GetMostSigOp();
        const Real& subtrahend = realCase->GetLeastSigOp();

        return std::make_unique<Real>(minuend.GetValue() - subtrahend.GetValue());
    }

    if (auto ImgCase = Subtract<Imaginary>::Specialize(simplifiedSubtract); ImgCase != nullptr) {
        return std::make_unique<Multiply<Real, Imaginary>>(Real { 2.0 }, Imaginary {});
    }

    if (auto ImgCase = Subtract<Multiply<Expression, Imaginary>, Imaginary>::Specialize(simplifiedSubtract); ImgCase != nullptr) {
        return std::make_unique<Multiply<Expression>>(
            *(Subtract { ImgCase->GetMostSigOp().GetMostSigOp(), Real { 1.0 } }.Simplify()), Imaginary {});
    }

    if (auto ImgCase = Subtract<Imaginary, Multiply<Expression, Imaginary>>::Specialize(simplifiedSubtract); ImgCase != nullptr) {
        return std::make_unique<Multiply<Expression>>(
            *(Subtract { Real { 1.0 }, ImgCase->GetLeastSigOp().GetMostSigOp() }.Simplify()), Imaginary {});
    }

    if (auto ImgCase = Subtract<Multiply<Expression, Imaginary>, Multiply<Expression, Imaginary>>::Specialize(simplifiedSubtract); ImgCase != nullptr) {
        return std::make_unique<Multiply<Expression>>(
            *(Subtract { ImgCase->GetLeastSigOp().GetMostSigOp(), ImgCase->GetMostSigOp().GetMostSigOp() }.Simplify()), Imaginary {});
    }

    // exponent - exponent
    if (auto exponentCase = Subtract<Exponent<Expression>, Exponent<Expression>>::Specialize(simplifiedSubtract); exponentCase != nullptr) {
        if (exponentCase->GetMostSigOp().GetMostSigOp().Equals(exponentCase->GetLeastSigOp().GetMostSigOp()) && exponentCase->GetMostSigOp().GetLeastSigOp().Equals(exponentCase->GetLeastSigOp().GetLeastSigOp())) {
            return std::make_unique<Real>(Real { 0.0 });
        }
    }

    // a*exponent - exponent
    if (auto exponentCase = Subtract<Multiply<Expression, Exponent<Expression>>, Exponent<Expression>>::Specialize(simplifiedSubtract); exponentCase != nullptr) {
        if (exponentCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exponentCase->GetLeastSigOp().GetMostSigOp()) && exponentCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp().Equals(exponentCase->GetLeastSigOp().GetLeastSigOp())) {
            if (Real { 1.0 }.Equals(exponentCase->GetMostSigOp().GetMostSigOp()))
                return std::make_unique<Real>(Real { 0.0 });
            return std::make_unique<Multiply<Expression>>(*(Subtract { exponentCase->GetMostSigOp().GetMostSigOp(), Real { 1.0 } }.Simplify()),
                exponentCase->GetLeastSigOp());
        }
    }

    // exponent - a*exponent
    if (auto exponentCase = Subtract<Exponent<Expression>, Multiply<Expression, Exponent<Expression>>>::Specialize(simplifiedSubtract); exponentCase != nullptr) {
        if (exponentCase->GetLeastSigOp().GetLeastSigOp().GetMostSigOp().Equals(exponentCase->GetMostSigOp().GetMostSigOp()) && exponentCase->GetLeastSigOp().GetLeastSigOp().GetLeastSigOp().Equals(exponentCase->GetMostSigOp().GetLeastSigOp())) {
            if (Real { 1.0 }.Equals(exponentCase->GetLeastSigOp().GetMostSigOp()))
                return std::make_unique<Real>(Real { 0.0 });
            return std::make_unique<Multiply<Expression>>(*(Subtract { Real { 1.0 }, exponentCase->GetLeastSigOp().GetMostSigOp() }.Simplify()),
                exponentCase->GetMostSigOp());
        }
    }

    // a*exponent - b*exponent
    if (auto exponentCase = Subtract<Multiply<Expression, Exponent<Expression>>, Multiply<Expression, Exponent<Expression>>>::Specialize(simplifiedSubtract); exponentCase != nullptr) {
        if (exponentCase->GetLeastSigOp().GetLeastSigOp().GetMostSigOp().Equals(exponentCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp()) && exponentCase->GetLeastSigOp().GetLeastSigOp().GetLeastSigOp().Equals(exponentCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp())) {
            if (Real { 1.0 }.Equals(exponentCase->GetLeastSigOp().GetMostSigOp()))
                return std::make_unique<Real>(Real { 0.0 });
            return std::make_unique<Multiply<Expression>>(
                *(Subtract { exponentCase->GetMostSigOp().GetMostSigOp(), exponentCase->GetLeastSigOp().GetMostSigOp() }.Simplify()),
                exponentCase->GetMostSigOp().GetLeastSigOp());
        }
    }

    // log(a) - log(b) = log(a / b)
    if (auto logCase = Subtract<Log<Expression, Expression>, Log<Expression, Expression>>::Specialize(simplifiedSubtract); logCase != nullptr) {
        if (logCase->GetMostSigOp().GetMostSigOp().Equals(logCase->GetLeastSigOp().GetMostSigOp())) {
            const IExpression auto& base = logCase->GetMostSigOp().GetMostSigOp();
            const IExpression auto& argument = Divide<Expression>({ logCase->GetMostSigOp().GetLeastSigOp(), logCase->GetLeastSigOp().GetLeastSigOp() });
            return std::make_unique<Log<Expression>>(base, argument);
        }
    }

    return simplifiedSubtract.Copy();
}

auto Subtract<Expression>::ToString() const -> std::string
{
    return fmt::format("({} - {})", mostSigOp->ToString(), leastSigOp->ToString());
}

auto Subtract<Expression>::Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression>
{
    std::unique_ptr<Expression> simplifiedMinuend, simplifiedSubtrahend;

    tf::Task leftSimplifyTask = subflow.emplace([this, &simplifiedMinuend](tf::Subflow& sbf) {
        if (!mostSigOp) {
            return;
        }

        simplifiedMinuend = mostSigOp->Simplify(sbf);
    });

    tf::Task rightSimplifyTask = subflow.emplace([this, &simplifiedSubtrahend](tf::Subflow& sbf) {
        if (!leastSigOp) {
            return;
        }

        simplifiedSubtrahend = leastSigOp->Simplify(sbf);
    });

    Subtract simplifiedSubtract;

    // While this task isn't actually parallelized, it exists as a prerequisite for check possible cases in parallel
    tf::Task simplifyTask = subflow.emplace([&simplifiedSubtract, &simplifiedMinuend, &simplifiedSubtrahend](tf::Subflow&) {
        if (simplifiedMinuend) {
            simplifiedSubtract.SetMostSigOp(*simplifiedMinuend);
        }

        if (simplifiedSubtrahend) {
            simplifiedSubtract.SetLeastSigOp(*simplifiedSubtrahend);
        }
    });

    simplifyTask.succeed(leftSimplifyTask, rightSimplifyTask);

    std::unique_ptr<Subtract<Real>> realCase;

    tf::Task realCaseTask = subflow.emplace([&simplifiedSubtract, &realCase](tf::Subflow& sbf) {
        realCase = Subtract<Real>::Specialize(simplifiedSubtract, sbf);
    });

    simplifyTask.precede(realCaseTask);

    subflow.join();

    if (realCase) {
        const Real& minuend = realCase->GetMostSigOp();
        const Real& subtrahend = realCase->GetLeastSigOp();

        return std::make_unique<Real>(minuend.GetValue() - subtrahend.GetValue());
    }

    return simplifiedSubtract.Copy();
}

auto Subtract<Expression>::Specialize(const Expression& other) -> std::unique_ptr<Subtract<Expression, Expression>>
{
    if (!other.Is<Oasis::Subtract>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize();
    return std::make_unique<Subtract>(dynamic_cast<const Subtract&>(*otherGeneralized));
}

auto Subtract<Expression>::Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Subtract>
{
    if (!other.Is<Oasis::Subtract>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize(subflow);
    return std::make_unique<Subtract>(dynamic_cast<const Subtract&>(*otherGeneralized));
}
auto Subtract<Expression>::Differentiate(const Expression& differentiationVariable) -> std::unique_ptr<Expression>
{
    // Single integration variable
    if (auto variable = Variable::Specialize(differentiationVariable); variable != nullptr) {
        auto simplifiedSub = this->Simplify();

        // Make sure we're still subtracting
        if (auto adder = Subtract<Expression>::Specialize(*simplifiedSub); adder != nullptr) {
            auto leftRef = adder->GetLeastSigOp().Copy();
            auto leftDiff = leftRef->Differentiate(differentiationVariable);

            auto specializedLeft = Add<Expression>::Specialize(*leftDiff);

            auto rightRef = adder->GetMostSigOp().Copy();
            auto rightDiff = rightRef->Differentiate(differentiationVariable);

            auto specializedRight = Add<Expression>::Specialize(*rightDiff);

            if (specializedLeft == nullptr || specializedRight == nullptr) {
                return Copy();
            }

            return std::make_unique<Subtract<Expression, Expression>>(Subtract<Expression> { *(specializedLeft->GetMostSigOp().Copy()), *(specializedRight->GetMostSigOp().Copy())})->Simplify();
        }
        // If not, use other integration technique
        else {
            return simplifiedSub->Differentiate(differentiationVariable);
        }
    }
    return Copy();
}

} // Oasis
//
// Created by Matthew McCall on 7/2/23.
//
#include <unordered_map>

#include "Oasis/Add.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
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

    if (auto ImgCase = Add<Imaginary>::Specialize(simplifiedAdd); ImgCase != nullptr) {
        return std::make_unique<Multiply<Real, Imaginary>>(Real { 2.0 }, Imaginary {});
    }

    if (auto ImgCase = Add<Multiply<Expression, Imaginary>, Imaginary>::Specialize(simplifiedAdd); ImgCase != nullptr) {
        return std::make_unique<Multiply<Expression>>(
            *(Add { Real { 1.0 }, ImgCase->GetMostSigOp().GetMostSigOp() }.Simplify()), Imaginary {});
    }

    if (auto ImgCase = Add<Multiply<Expression, Imaginary>, Multiply<Expression, Imaginary>>::Specialize(simplifiedAdd); ImgCase != nullptr) {
        return std::make_unique<Multiply<Expression>>(
            *(Add { ImgCase->GetLeastSigOp().GetMostSigOp(), ImgCase->GetMostSigOp().GetMostSigOp() }.Simplify()), Imaginary {});
    }

    // exponent + exponent
    if (auto exponentCase = Add<Exponent<Expression>, Exponent<Expression>>::Specialize(simplifiedAdd); exponentCase != nullptr) {
        if (exponentCase->GetMostSigOp().GetMostSigOp().Equals(exponentCase->GetLeastSigOp().GetMostSigOp()) && exponentCase->GetMostSigOp().GetLeastSigOp().Equals(exponentCase->GetLeastSigOp().GetLeastSigOp())) {
            return std::make_unique<Multiply<Expression>>(Real { 2.0 }, exponentCase->GetMostSigOp());
        }
    }

    // a*exponent + exponent
    if (auto exponentCase = Add<Multiply<Expression, Exponent<Expression>>, Exponent<Expression>>::Specialize(simplifiedAdd); exponentCase != nullptr) {
        if (exponentCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exponentCase->GetLeastSigOp().GetMostSigOp()) && exponentCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp().Equals(exponentCase->GetLeastSigOp().GetLeastSigOp())) {
            return std::make_unique<Multiply<Expression>>(*(Add<Expression> { exponentCase->GetMostSigOp().GetMostSigOp(), Real { 1.0 } }.Simplify()),
                exponentCase->GetLeastSigOp());
        }
    }

    if (auto exponentCase = Add<Exponent<Expression>, Multiply<Expression, Exponent<Expression>>>::Specialize(simplifiedAdd); exponentCase != nullptr) {
        if (exponentCase->GetLeastSigOp().GetLeastSigOp().GetMostSigOp().Equals(exponentCase->GetMostSigOp().GetMostSigOp()) && exponentCase->GetLeastSigOp().GetLeastSigOp().GetLeastSigOp().Equals(exponentCase->GetMostSigOp().GetLeastSigOp())) {
            return std::make_unique<Multiply<Expression>>(*(Add<Expression> { exponentCase->GetMostSigOp().GetMostSigOp(), Real { 1.0 } }.Simplify()),
                exponentCase->GetLeastSigOp());
        }
    }

    // a*exponent+b*exponent
    if (auto exponentCase = Add<Multiply<Expression, Exponent<Expression>>, Multiply<Expression, Exponent<Expression>>>::Specialize(simplifiedAdd); exponentCase != nullptr) {
        if (exponentCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exponentCase->GetLeastSigOp().GetLeastSigOp().GetMostSigOp()) && exponentCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp().Equals(exponentCase->GetLeastSigOp().GetLeastSigOp().GetLeastSigOp())) {
            return std::make_unique<Multiply<Expression>>(*(Add<Expression> { exponentCase->GetMostSigOp().GetMostSigOp(), exponentCase->GetLeastSigOp().GetMostSigOp() }.Simplify()),
                exponentCase->GetLeastSigOp());
        }
    }

    std::map<std::unique_ptr<Expression>, unsigned> terms;

    std::vector<std::unique_ptr<Expression>> simplifiedTerms;
    this->Flatten(simplifiedTerms);

    //    for (const auto& term: simplifiedTerms) {
    //        if (auto multiply = Multiply<Real, Expression>::Specialize(*term); multiply != nullptr) {
    //            auto leastSigOp = multiply->GetLeastSigOp().Copy();
    //            if (terms.find(leastSigOp) == terms.end()) {
    //                terms[leastSigOp] = 0;
    //            }
    //
    //            terms[leastSigOp] += static_cast<int>(multiply->GetMostSigOp().GetValue());
    //        } else {
    //            if (terms.find(term) == terms.end()) {
    //                terms[term] = 0;
    //            }
    //
    //            terms[term] += 1;
    //        }
    //
    //    }

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
    tf::Task simplifyTask = subflow.emplace([&simplifiedAdd, &simplifiedAugend, &simplifiedAddend](tf::Subflow& sbf) {
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
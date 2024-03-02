//
// Created by bachia on 2/27/2024.
//

#include "../include/Oasis/Differentiate.hpp"
#include <unordered_map>

#include "Oasis/Add.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Multiply.hpp"

namespace Oasis {

    Differentiate<Expression>::Differentiate(const Expression& exp, const Expression& var)
            : BinaryExpression(exp, var)
    {
    }

    auto Differentiate<Expression>::Simplify() const -> std::unique_ptr<Expression>
    {
        auto simplifiedExpression = mostSigOp ? mostSigOp->Simplify() : nullptr;
        auto simplifiedVar = leastSigOp ? leastSigOp->Simplify() : nullptr;

        Differentiate simplifiedDifferentiate { *simplifiedExpression, *simplifiedVar };

        if (auto realCase = Differentiate<Real>::Specialize(simplifiedDifferentiate); realCase != nullptr) {

            return std::make_unique<Real>(0);
        }

        if (auto likeTermsCase = Add<Multiply<Real, Expression>>::Specialize(simplifiedDifferentiate); likeTermsCase != nullptr) {
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

        // log(a) + log(b) = log(ab)
        if (auto logCase = Add<Log<Expression, Expression>, Log<Expression, Expression>>::Specialize(simplifiedAdd); logCase != nullptr) {
            if (logCase->GetMostSigOp().GetMostSigOp().Equals(logCase->GetLeastSigOp().GetMostSigOp())) {
                const IExpression auto& base = logCase->GetMostSigOp().GetMostSigOp();
                const IExpression auto& argument = Multiply<Expression>({ logCase->GetMostSigOp().GetLeastSigOp(), logCase->GetLeastSigOp().GetLeastSigOp() });
                return std::make_unique<Log<Expression>>(base, argument);
            }
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
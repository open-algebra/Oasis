//
// Created by Matthew McCall on 8/10/23.
//

#include "Oasis/Multiply.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Exponent.hpp"

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

    if (auto exprCase = Multiply<Expression>::Specialize(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().Equals(exprCase->GetLeastSigOp())) {
            return std::make_unique<Exponent<Expression, Expression>>(exprCase->GetMostSigOp(), Real { 2.0 });
        }
    }

    if (auto exprCase = Multiply<Expression, Exponent<Expression, Expression>>::Specialize(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetMostSigOp())) {
            return std::make_unique<Exponent<Expression>>(exprCase->GetMostSigOp(),
                *(Add<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } }.Simplify()));
        }
    }

    // x*x^n
    if (auto exprCase = Multiply<Expression, Exponent<Expression>>::Specialize(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetMostSigOp())) {
            return std::make_unique<Exponent<Expression>>(exprCase->GetMostSigOp(),
                *(Add<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } }.Simplify()));
        }
    }

    if (auto exprCase = Multiply<Exponent<Expression>, Expression>::Specialize(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetLeastSigOp().Equals(exprCase->GetMostSigOp().GetMostSigOp())) {
            return std::make_unique<Exponent<Expression>>(exprCase->GetLeastSigOp(),
                *(Add<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(), Real { 1.0 } }.Simplify()));
        }
    }

    // x^n*x^m
    if (auto exprCase = Multiply<Exponent<Expression>, Exponent<Expression>>::Specialize(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetMostSigOp())) {
            return std::make_unique<Exponent<Expression>>(exprCase->GetMostSigOp().GetMostSigOp(),
                *(Add<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(), exprCase->GetLeastSigOp().GetLeastSigOp() }.Simplify()));
        }
    }

    // a*x*x
    if (auto exprCase = Multiply<Multiply<Expression>, Expression>::Specialize(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().Equals(exprCase->GetLeastSigOp())) {
            return std::make_unique<Multiply<Expression, Expression>>(exprCase->GetMostSigOp().GetMostSigOp(),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(), Real { 2.0 } });
        }
    }

    // a*x*b*x
    if (auto exprCase = Multiply<Multiply<Expression>, Multiply<Expression>>::Specialize(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().Equals(exprCase->GetLeastSigOp().GetLeastSigOp())) {
            return std::make_unique<Multiply<Expression>>(
                *(Multiply<Expression> { exprCase->GetMostSigOp().GetMostSigOp(), exprCase->GetLeastSigOp().GetMostSigOp() }.Simplify()),
                *(Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(), Real { 2.0 } }.Simplify()));
        }
    }

    // a*x^n*x
    if (auto exprCase = Multiply<Multiply<Expression, Exponent<Expression>>, Expression>::Specialize(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exprCase->GetLeastSigOp())) {
            return std::make_unique<Multiply<Expression>>(exprCase->GetMostSigOp().GetMostSigOp(),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp(),
                    *(Add<Expression> { exprCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } }.Simplify()) });
        }
    }

    // a*x*x^n
    if (auto exprCase = Multiply<Multiply<Expression>, Exponent<Expression>>::Specialize(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().Equals(exprCase->GetLeastSigOp().GetMostSigOp())) {
            return std::make_unique<Multiply<Expression>>(exprCase->GetMostSigOp().GetMostSigOp(),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(),
                    *(Add<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } }.Simplify()) });
        }
        if (exprCase->GetMostSigOp().GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetMostSigOp())) {
            return std::make_unique<Multiply<Expression>>(exprCase->GetMostSigOp().GetLeastSigOp(),
                Exponent<Expression> { exprCase->GetMostSigOp().GetMostSigOp(),
                    *(Add<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } }.Simplify()) });
        }
    }

    // a*x^n*b*x
    if (auto exprCase = Multiply<Multiply<Expression>, Multiply<Expression, Exponent<Expression>>>::Specialize(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().Equals(exprCase->GetLeastSigOp().GetLeastSigOp().GetMostSigOp())) {
            return std::make_unique<Multiply<Expression>>(
                *(Multiply<Expression> { exprCase->GetMostSigOp().GetMostSigOp(), exprCase->GetLeastSigOp().GetMostSigOp() }.Simplify()),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(),
                    *(Add<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } }.Simplify()) });
        }
    }

    if (auto exprCase = Multiply<Multiply<Expression>, Multiply<Exponent<Expression>, Expression>>::Specialize(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().Equals(exprCase->GetLeastSigOp().GetMostSigOp().GetMostSigOp())) {
            return std::make_unique<Multiply<Expression>>(
                *(Multiply<Expression> { exprCase->GetMostSigOp().GetMostSigOp(), exprCase->GetLeastSigOp().GetLeastSigOp() }.Simplify()),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(),
                    *(Add<Expression> { exprCase->GetLeastSigOp().GetMostSigOp().GetLeastSigOp(), Real { 1.0 } }.Simplify()) });
        }
    }

    if (auto exprCase = Multiply<Multiply<Expression, Exponent<Expression>>, Multiply<Expression>>::Specialize(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetLeastSigOp())) {
            return std::make_unique<Multiply<Expression>>(
                *(Multiply<Expression> { exprCase->GetMostSigOp().GetMostSigOp(), exprCase->GetLeastSigOp().GetLeastSigOp() }.Simplify()),
                Exponent<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp(),
                    *(Add<Expression> { exprCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } }.Simplify()) });
        }
    }

    // a*x^n*x^m
    if (auto exprCase = Multiply<Multiply<Expression, Exponent<Expression>>, Exponent<Expression>>::Specialize(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetLeastSigOp())) {
            return std::make_unique<Multiply<Expression>>(
                exprCase->GetMostSigOp().GetMostSigOp(),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp(),
                    *(Add<Expression> { exprCase->GetLeastSigOp().GetMostSigOp(), exprCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp() }.Simplify()) });
        }
    }

    // a*x^n*b*x^m
    if (auto exprCase = Multiply<Multiply<Expression, Exponent<Expression>>, Multiply<Expression, Exponent<Expression>>>::Specialize(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetLeastSigOp().GetMostSigOp())) {
            return std::make_unique<Multiply<Expression>>(
                *(Multiply<Expression> { exprCase->GetMostSigOp().GetMostSigOp(), exprCase->GetLeastSigOp().GetMostSigOp() }.Simplify()),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp(),
                    *(Add<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp().GetLeastSigOp(), exprCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp() }.Simplify()) });
        }
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
//
// Created by Matthew McCall on 8/10/23.
//

#include "Oasis/Multiply.hpp"
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

#pragma region replace_variable_with_expression
    // x*x
    if (auto variableCase = Multiply<Variable>::Specialize(simplifiedMultiply); variableCase != nullptr) {
        if (variableCase->GetMostSigOp().Equals(variableCase->GetLeastSigOp())) {
            return std::make_unique<Oasis::Exponent<Variable, Real>>(variableCase->GetMostSigOp(), Oasis::Real { 2.0 });
            // return std::make_unique<Oasis::Exponent<Expression>>(variableCase->GetMostSigOp(), Oasis::Real{ 2.0 } );
        }
    }

    // x*x^n
    if (auto variableCase = Multiply<Variable, Exponent<Variable, Real>>::Specialize(simplifiedMultiply); variableCase != nullptr) {
        if (variableCase->GetMostSigOp().Equals(variableCase->GetLeastSigOp().GetMostSigOp())) {
            return std::make_unique<Oasis::Exponent<Variable, Real>>(variableCase->GetMostSigOp(),
                Oasis::Real { variableCase->GetLeastSigOp().GetLeastSigOp().GetValue() + 1.0 });
        }
    }

    // x^n*x^m
    if (auto variableCase = Multiply<Exponent<Variable, Real>, Exponent<Variable, Real>>::Specialize(simplifiedMultiply); variableCase != nullptr) {
        if (variableCase->GetMostSigOp().GetMostSigOp().Equals(variableCase->GetLeastSigOp().GetMostSigOp())) {
            return std::make_unique<Oasis::Exponent<Variable, Real>>(variableCase->GetMostSigOp().GetMostSigOp(),
                Oasis::Real { variableCase->GetMostSigOp().GetLeastSigOp().GetValue() + variableCase->GetLeastSigOp().GetLeastSigOp().GetValue() });
        }
    }

    // a*x*x
    if (auto variableCase = Multiply<Multiply<Real, Variable>, Variable>::Specialize(simplifiedMultiply); variableCase != nullptr) {
        if (variableCase->GetMostSigOp().GetLeastSigOp().Equals(variableCase->GetLeastSigOp())) {
            return std::make_unique<Oasis::Multiply<Oasis::Real, Oasis::Exponent<Variable, Real>>>(
                Oasis::Real { variableCase->GetMostSigOp().GetMostSigOp().GetValue() },
                Oasis::Exponent { variableCase->GetMostSigOp().GetLeastSigOp(), Oasis::Real { 2.0 } });
        }
    }

    // a*x*b*x
    if (auto variableCase = Multiply<Multiply<Real, Variable>, Multiply<Real, Variable>>::Specialize(simplifiedMultiply); variableCase != nullptr) {
        if (variableCase->GetMostSigOp().GetLeastSigOp().Equals(variableCase->GetLeastSigOp().GetLeastSigOp())) {
            return std::make_unique<Oasis::Multiply<Oasis::Real, Oasis::Exponent<Variable, Real>>>(
                Oasis::Real { variableCase->GetMostSigOp().GetMostSigOp().GetValue() * variableCase->GetLeastSigOp().GetMostSigOp().GetValue() },
                Oasis::Exponent { variableCase->GetMostSigOp().GetLeastSigOp(), Oasis::Real { 2.0 } });
        }
    }

    // a*x^n*x
    if (auto variableCase = Multiply<Multiply<Real, Exponent<Variable, Real>>, Variable>::Specialize(simplifiedMultiply); variableCase != nullptr) {
        if (variableCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(variableCase->GetLeastSigOp())) {
            return std::make_unique<Oasis::Multiply<Oasis::Real, Oasis::Exponent<Variable, Real>>>(
                Oasis::Real { variableCase->GetMostSigOp().GetMostSigOp().GetValue() },
                Oasis::Exponent { variableCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp(),
                    Oasis::Real { variableCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp().GetValue() + 1.0 } });
        }
    }

    // x^n*a*x
    if (auto variableCase = Multiply<Multiply<Real, Variable>, Exponent<Variable, Real>>::Specialize(simplifiedMultiply); variableCase != nullptr) {
        if (variableCase->GetMostSigOp().GetLeastSigOp().Equals(variableCase->GetLeastSigOp().GetMostSigOp())) {
            return std::make_unique<Oasis::Multiply<Oasis::Real, Oasis::Exponent<Variable, Real>>>(
                Oasis::Real { variableCase->GetMostSigOp().GetMostSigOp().GetValue() },
                Oasis::Exponent { variableCase->GetMostSigOp().GetLeastSigOp(),
                    Oasis::Real { variableCase->GetLeastSigOp().GetLeastSigOp().GetValue() + 1.0 } });
        }
    }

    // a*x^n*b*x
    if (auto variableCase = Multiply<Multiply<Real, Variable>, Multiply<Real, Exponent<Variable, Real>>>::Specialize(simplifiedMultiply); variableCase != nullptr) {
        if (variableCase->GetMostSigOp().GetLeastSigOp().Equals(variableCase->GetLeastSigOp().GetLeastSigOp().GetMostSigOp())) {
            return std::make_unique<Oasis::Multiply<Oasis::Real, Oasis::Exponent<Variable, Real>>>(
                Oasis::Real { variableCase->GetMostSigOp().GetMostSigOp().GetValue() * variableCase->GetLeastSigOp().GetMostSigOp().GetValue() },
                Oasis::Exponent { variableCase->GetMostSigOp().GetLeastSigOp(),
                    Oasis::Real { variableCase->GetLeastSigOp().GetLeastSigOp().GetLeastSigOp().GetValue() + 1.0 } });
        }
    }

    // a*x^n*x^m
    if (auto variableCase = Multiply<Multiply<Real, Exponent<Variable, Real>>, Exponent<Variable, Real>>::Specialize(simplifiedMultiply); variableCase != nullptr) {
        if (variableCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(variableCase->GetLeastSigOp().GetMostSigOp())) {
            return std::make_unique<Oasis::Multiply<Oasis::Real, Oasis::Exponent<Variable, Real>>>(
                variableCase->GetMostSigOp().GetMostSigOp(),
                Oasis::Exponent { variableCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp(),
                    Oasis::Real { variableCase->GetLeastSigOp().GetLeastSigOp().GetValue() + variableCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp().GetValue() } });
        }
    }

    // a*x^n*b*x^m
    if (auto variableCase = Multiply<Multiply<Real, Exponent<Variable, Real>>, Multiply<Real, Exponent<Variable, Real>>>::Specialize(simplifiedMultiply); variableCase != nullptr) {
        if (variableCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp().Equals(variableCase->GetLeastSigOp().GetMostSigOp())) {
            return std::make_unique<Oasis::Multiply<Oasis::Real, Oasis::Exponent<Variable, Real>>>(
                Oasis::Real { variableCase->GetMostSigOp().GetMostSigOp().GetValue() * variableCase->GetLeastSigOp().GetMostSigOp().GetValue() },
                Oasis::Exponent { variableCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp(),
                    Oasis::Real { variableCase->GetLeastSigOp().GetLeastSigOp().GetLeastSigOp().GetValue() + variableCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp().GetValue() } });
        }
    }

#pragma endregion

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
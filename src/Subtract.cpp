//
// Created by Matthew McCall on 8/10/23.
//

#include "Oasis/Subtract.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Matrix.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Negate.hpp"
#include "Oasis/Variable.hpp"

namespace Oasis {

Subtract<Expression>::Subtract(const Expression& minuend, const Expression& subtrahend)
    : BinaryExpression(minuend, subtrahend)
{
}

auto Subtract<Expression>::Simplify() const -> std::unique_ptr<Expression>
{
    const auto simplifiedMinuend = mostSigOp ? mostSigOp->Simplify() : nullptr;
    const auto simplifiedSubtrahend = leastSigOp ? leastSigOp->Simplify() : nullptr;

    const Subtract simplifiedSubtract { *simplifiedMinuend, *simplifiedSubtrahend };

    // 2 - 1 = 1
    if (auto realCase = Subtract<Real>::Specialize(simplifiedSubtract); realCase != nullptr) {
        const Real& minuend = realCase->GetMostSigOp();
        const Real& subtrahend = realCase->GetLeastSigOp();

        return std::make_unique<Real>(minuend.GetValue() - subtrahend.GetValue());
    }

    // x - x = 0
    if (simplifiedMinuend->Equals(*simplifiedSubtrahend)) {
        return std::make_unique<Real>(Real { 0.0 });
    }

    if (auto matrixCase = Subtract<Matrix, Matrix>::Specialize(simplifiedSubtract); matrixCase != nullptr) {
        const Oasis::IExpression auto& leftTerm = matrixCase->GetMostSigOp();
        const Oasis::IExpression auto& rightTerm = matrixCase->GetLeastSigOp();

        if ((leftTerm.GetRows() == rightTerm.GetRows()) && (leftTerm.GetCols() == leftTerm.GetCols())) {
            return std::make_unique<Matrix>(leftTerm.GetMatrix() - rightTerm.GetMatrix());
        } else {
            return std::make_unique<Subtract<Expression>>(leftTerm, rightTerm);
        }
    }

    // ax - x = (a-1)x
    if (const auto minusOneCase = Subtract<Multiply<>, Expression>::Specialize(simplifiedSubtract); minusOneCase != nullptr) {
        if (minusOneCase->GetMostSigOp().GetLeastSigOp().Equals(minusOneCase->GetLeastSigOp())) {
            const Subtract newCoefficient { minusOneCase->GetMostSigOp().GetMostSigOp(), Real { 1.0 } };
            return Multiply { newCoefficient, minusOneCase->GetLeastSigOp() }.Simplify();
        }
    }

    // x-ax = (1-a)x
    if (const auto oneMinusCase = Subtract<Expression, Multiply<>>::Specialize(simplifiedSubtract); oneMinusCase != nullptr) {
        if (oneMinusCase->GetMostSigOp().Equals(oneMinusCase->GetLeastSigOp().GetLeastSigOp())) {
            const Subtract newCoefficient { Real { 1.0 }, oneMinusCase->GetLeastSigOp().GetMostSigOp() };
            return Multiply { newCoefficient, oneMinusCase->GetMostSigOp() }.Simplify();
        }
    }

    // ax-bx= (a-b)x
    if (const auto coefficientCase = Subtract<Multiply<>>::Specialize(simplifiedSubtract); coefficientCase != nullptr) {
        if (coefficientCase->GetMostSigOp().GetLeastSigOp().Equals(coefficientCase->GetLeastSigOp().GetLeastSigOp())) {
            const Subtract newCoefficient { coefficientCase->GetMostSigOp().GetMostSigOp(), coefficientCase->GetLeastSigOp().GetMostSigOp() };
            return Multiply { newCoefficient, coefficientCase->GetLeastSigOp().GetLeastSigOp() }.Simplify();
        }
    }

    // log(a) - log(b) = log(a / b)
    if (const auto logCase = Subtract<Log<>>::Specialize(simplifiedSubtract); logCase != nullptr) {
        if (logCase->GetMostSigOp().GetMostSigOp().Equals(logCase->GetLeastSigOp().GetMostSigOp())) {
            const IExpression auto& base = logCase->GetMostSigOp().GetMostSigOp();
            const IExpression auto& argument = Divide({ logCase->GetMostSigOp().GetLeastSigOp(), logCase->GetLeastSigOp().GetLeastSigOp() });
            return std::make_unique<Log<>>(base, argument);
        }
    }

    // makes subtraction into addition because it is easier to deal with
    auto negated = Multiply<Expression> { Real { -1 }, *simplifiedSubtrahend };
    if (auto added = Add<Expression>::Specialize(negated.GetLeastSigOp()); added != nullptr) {
        auto RHS = Add { *(Multiply<Expression> { Real { -1.0 }, added->GetMostSigOp() }.Simplify()),
            *(Multiply<Expression> { Real { -1.0 }, added->GetLeastSigOp() }.Simplify()) }
                       .Simplify();
        return Add { *simplifiedMinuend, *RHS }.Simplify();
    }
    if (auto subtracted = Subtract<Expression>::Specialize(negated.GetLeastSigOp()); subtracted != nullptr) {
        auto RHS = Add { *(Multiply<Expression> { Real { -1.0 }, subtracted->GetMostSigOp() }.Simplify()),
            *(subtracted->GetLeastSigOp().Simplify()) }
                       .Simplify();
        return Add { *simplifiedMinuend, *RHS }.Simplify();
    }
    return Add { *simplifiedMinuend, negated }.Simplify();
}

auto Subtract<Expression>::Specialize(const Expression& other) -> std::unique_ptr<Subtract<Expression, Expression>>
{
    if (!other.Is<Oasis::Subtract>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize();
    return std::make_unique<Subtract>(dynamic_cast<const Subtract&>(*otherGeneralized));
}

auto Subtract<Expression>::Differentiate(const Expression& differentiationVariable) const -> std::unique_ptr<Expression>
{
    // Single diff variable
    if (auto variable = Variable::Specialize(differentiationVariable); variable != nullptr) {
        auto simplifiedSub = this->Simplify();

        // Make sure we're still subtracting
        if (auto adder = Subtract<Expression>::Specialize(*simplifiedSub); adder != nullptr) {
            auto rightRef = adder->GetLeastSigOp().Copy();
            auto rightDiff = rightRef->Differentiate(differentiationVariable);

            auto specializedRight = Expression::Specialize(*rightDiff);

            auto leftRef = adder->GetMostSigOp().Copy();
            auto leftDiff = leftRef->Differentiate(differentiationVariable);

            auto specializedLeft = Expression::Specialize(*leftDiff);

            if (specializedLeft == nullptr || specializedRight == nullptr) {
                return Copy();
            }

            return std::make_unique<Subtract<Expression, Expression>>(Subtract<Expression, Expression> { *(specializedLeft->Copy()), *(specializedRight->Copy()) })->Simplify();
        }
        // If not, use other differentiation technique
        else {
            return simplifiedSub->Differentiate(differentiationVariable);
        }
    }
    return Copy();
}

auto Subtract<Expression>::Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression>
{
    // Single integration variable
    if (auto variable = Variable::Specialize(integrationVariable); variable != nullptr) {
        auto simplifiedSub = this->Simplify();

        // Make sure we're still subtracting
        if (auto adder = Subtract<Expression>::Specialize(*simplifiedSub); adder != nullptr) {
            auto leftRef = adder->GetLeastSigOp().Copy();
            auto leftIntegral = leftRef->Integrate(integrationVariable);

            auto specializedLeft = Add<Expression>::Specialize(*leftIntegral);

            auto rightRef = adder->GetMostSigOp().Copy();
            auto rightIntegral = rightRef->Integrate(integrationVariable);

            auto specializedRight = Add<Expression>::Specialize(*rightIntegral);

            if (specializedLeft == nullptr || specializedRight == nullptr) {
                return Copy();
            }
            Add add { Subtract<Expression> {
                          *(specializedRight->GetMostSigOp().Copy()),
                          *(specializedLeft->GetMostSigOp().Copy()) },
                Variable { "C" } };

            return add.Simplify();
        }
        // If not, use other integration technique
        else {
            return simplifiedSub->Integrate(integrationVariable);
        }
    }
    Integral<Expression, Expression> integral { *(this->Copy()), *(integrationVariable.Copy()) };

    return integral.Copy();
}

} // Oasis
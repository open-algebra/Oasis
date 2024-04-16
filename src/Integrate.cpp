//
// Created by Levy Lin on 2/09/2024.
//

#include "Oasis/Integrate.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Log.hpp"

namespace Oasis {

Integrate<Expression>::Integrate(const Expression& integrand, const Expression& differential)
    : BinaryExpression(integrand, differential)
{
}

auto Integrate<Expression>::Simplify() const -> std::unique_ptr<Expression>
{
    // Returns simplified Integral

    auto simplifiedIntegrand = mostSigOp ? mostSigOp->Simplify() : nullptr;
    auto simplifiedDifferential = leastSigOp ? leastSigOp->Simplify() : nullptr;

    Integrate simplifiedIntegrate { *simplifiedIntegrand, *simplifiedDifferential };

    // Integration Rules

    // Constant Rule
    if (auto constCase = Integrate<Real, Variable>::Specialize(simplifiedIntegrate); constCase != nullptr) {
        const Variable& differential = constCase->GetLeastSigOp();
        Oasis::Real constant{constCase->GetMostSigOp()};

        return constant.Integrate(differential);
    }

    if (auto constCase = Integrate<Divide<Real>, Variable>::Specialize(simplifiedIntegrate); constCase != nullptr) {
        const Variable& differential = constCase->GetLeastSigOp();
        Oasis::Divide constant{constCase->GetMostSigOp()};
        auto constReal = constant.Simplify();

        return constReal->Integrate(differential);
    }

    // Power Rule
    if (auto powerCase = Integrate<Exponent<Expression>, Variable>::Specialize(simplifiedIntegrate); powerCase != nullptr) {
        const Variable& differential = powerCase->GetLeastSigOp();
        Oasis::Exponent exponent{powerCase->GetMostSigOp()};

        return exponent.Integrate(differential);
    }

    // Constant + Power Rule
    if (auto powerCase = Integrate<Multiply<Expression, Exponent<Expression>>, Variable>::Specialize(simplifiedIntegrate); powerCase != nullptr) {
        const Variable& differential = powerCase->GetLeastSigOp();
        Oasis::Exponent exponent{powerCase->GetMostSigOp().GetLeastSigOp()};
        auto integration = exponent.Integrate(differential);

        return std::make_unique<Multiply<Expression>>(powerCase->GetMostSigOp().GetMostSigOp(), (*integration));
    }

    if (auto powerCase = Integrate<Multiply<Exponent<Expression>, Expression>, Variable>::Specialize(simplifiedIntegrate); powerCase != nullptr) {
        const Variable& differential = powerCase->GetLeastSigOp();
        Oasis::Exponent exponent{powerCase->GetMostSigOp().GetMostSigOp()};
        auto integration = exponent.Integrate(differential);

        return std::make_unique<Multiply<Expression>>(powerCase->GetMostSigOp().GetLeastSigOp(), (*integration));
    }

    // U Substitution Rule
    // Needs Differentiation Implementation for U-Sub


    // Exponential Rule - a^x
    // Needs Euler's Number for Exponential Functions

    // Natural Log Rule
    // Needs Euler's Number for Exponential Functions

    // Euler's Number
    // Needs Euler's Number for Exponential Functions

    // Trigonometric Rules
    // Needs Trig for Exponential Functions

    // Powers of Sin and Cos
    // Needs Trig for Exponential Functions

    // Integration by Parts


    // Partial Fraction Decomposition



    return simplifiedIntegrate.Copy();
}

//auto Integrate<Expression>::Simplify(const Expression& upper, const Expression& lower) const -> std::unique_ptr<Expression>
//{
//
//}


auto Integrate<Expression>::ToString() const -> std::string
{
    return fmt::format("({} + {})", mostSigOp->ToString(), leastSigOp->ToString());
}

auto Integrate<Expression>::Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression>
{
    std::unique_ptr<Expression> simplifiedIntegrand, simplifiedDifferential;

    // To be Implemented : Performs Integration on simplified Expression



}

auto Integrate<Expression>::Specialize(const Expression& other) -> std::unique_ptr<Integrate<Expression, Expression>>
{
    if (!other.Is<Oasis::Integrate>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize();
    return std::make_unique<Integrate>(dynamic_cast<const Integrate&>(*otherGeneralized));
}

auto Integrate<Expression>::Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Integrate>
{
    if (!other.Is<Oasis::Integrate>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize(subflow);
    return std::make_unique<Integrate>(dynamic_cast<const Integrate&>(*otherGeneralized));
}

} // Oasis

//
// Created by Levy Lin on 2/09/2024.
//

#include "Oasis/Integral.hpp"

#include "Oasis/Add.hpp"
// #include "Oasis/Log.hpp"
// #include "Oasis/Imaginary.hpp"
#include "Oasis/Subtract.hpp"

namespace Oasis {

Integral<Expression>::Integral(const Expression& integrand, const Expression& differential)
    : BinaryExpression(integrand, differential)
{
}

auto Integral<Expression, Expression>::IntegrateWithBounds(const Expression& variable, const Expression& lower, const Expression& upper) -> std::unique_ptr<Expression>
{
    // If the bounds are equal, then the integral will always return 0.
    // Handle that here, instead of going through the whole process to get the same result.
    if (lower.Equals(upper)) {
        return Real { 0.0f }.Copy();
    }

    // To avoid a scenario with integrating 0 and having only a + C,
    // make sure that the integral does not just contain 0.
    if (this->GetMostSigOp().Equals( Real { 0.0f } )) {
        // The definite integral of 0 is always 0, so just return 0.
        return Real { 0.0f }.Copy();
    }

    SimplifyVisitor simplifyVisitor {};

    // Attempt to integrate the function.
    // If it fails (and returns nullptr), then just return out with a nullptr.
    std::unique_ptr<Expression> integrated = this->GetMostSigOp().Integrate(this->GetLeastSigOp());

    if (integrated == nullptr) return nullptr;

    // Cast to addition so that we can access only F(x) instead of F(x) + C
    std::unique_ptr<Add<Expression, Expression>> integratedFunction = RecursiveCast<Add<Expression, Expression>>(*integrated);

    // If we failed to cast to addition, then something went wrong (there should always be F(x) + C)
    if (integratedFunction == nullptr) {
        return nullptr;
    }

    // Calling make_unique for a Real allows the results
    // to be cast to general expressions, while still making use of make_unique
    // so that the pointers do not prematurely go out of scope
    std::unique_ptr<Expression> upperBoundResult = std::make_unique<Real>();
    std::unique_ptr<Expression> lowerBoundResult = std::make_unique<Real>();

    // Substitute in the upper and lower bounds for the variable
    upperBoundResult = integratedFunction->GetMostSigOp().Copy()->Substitute(variable, upper);
    lowerBoundResult = integratedFunction->GetMostSigOp().Copy()->Substitute(variable, lower);

    // Further simplify upperBoundResult and lowerBoundResult
    upperBoundResult = *upperBoundResult->Accept(simplifyVisitor);
    lowerBoundResult = *lowerBoundResult->Accept(simplifyVisitor);

    // Subtract the two values, then simplify them

    Subtract<Expression, Expression> subtracted = Subtract<Expression, Expression> {
        *upperBoundResult, *lowerBoundResult
    };

    // The result is returned through a pointer, so we can check if it worked by checking for nullptr

    auto result = subtracted.Accept(simplifyVisitor);

    if (!result) {
        return nullptr;
    }

    // Otherwise, get the answer and return it
    return std::move(result).value();
}

// auto Integral<Expression>::Simplify(const Expression& upper, const Expression& lower) const -> std::unique_ptr<Expression>
// {
//     // Returns simplified Integral
//     SimplifyVisitor simplifyVisitor {};
//
//     std::unique_ptr<Expression> simplifiedIntegrand = mostSigOp ? mostSigOp->Accept(simplifyVisitor).value() : std::unique_ptr<Expression> { nullptr };
//     std::unique_ptr<Expression> simplifiedDifferential = leastSigOp ? leastSigOp->Accept(simplifyVisitor).value() : std::unique_ptr<Expression> { nullptr };
//
//     return simplifiedIntegrand->IntegrateWithBounds(*simplifiedDifferential, upper, lower);
/*
    Integral simplifiedIntegrate { *simplifiedIntegrand, *simplifiedDifferential };

    // Bounded Integration Rules

    // Constant Rule
    if (auto constCase = RecursiveCast<Integral<Real, Variable>>(simplifiedIntegrate); constCase != nullptr) {
        const Real& constant = constCase->GetMostSigOp();

        auto upper_bound = RecursiveCast<Real>(upper);
        auto lower_bound = RecursiveCast<Real>(lower);

        if (upper_bound != nullptr && lower_bound != nullptr) {
            return std::make_unique<Real>((constant.GetValue() * upper_bound->GetValue()) - (constant.GetValue() * lower_bound->GetValue()));
        } else if (upper_bound != nullptr && lower_bound == nullptr) {
            return std::make_unique<Subtract<Real, Expression>>(Real { constant.GetValue() * upper_bound->GetValue() }, *Multiply<Expression> { constant, lower }.Simplify());
        } else if (upper_bound == nullptr && lower_bound != nullptr) {
            return std::make_unique<Subtract<Expression, Real>>(*Multiply<Expression> { constant, upper }.Simplify(), Real { constant.GetValue() * lower_bound->GetValue() });
        }
        return std::make_unique<Subtract<Expression>>(*Multiply<Expression> { constant, upper }.Simplify(), *Multiply<Expression> { constant, lower }.Simplify());
    }

    if (auto constCase = RecursiveCast<Integral<Divide<Real>, Variable>>(simplifiedIntegrate); constCase != nullptr) {
        Oasis::Divide constant { constCase->GetMostSigOp() };
        auto constReal = RecursiveCast<Real>(*constant.Simplify());

        auto upper_bound = RecursiveCast<Real>(upper);
        auto lower_bound = RecursiveCast<Real>(lower);

        if (upper_bound != nullptr && lower_bound != nullptr) {
            return std::make_unique<Real>((Real { *constReal }.GetValue() * upper_bound->GetValue()) - (Real { *constReal }.GetValue() * lower_bound->GetValue()));
        } else if (upper_bound != nullptr && lower_bound == nullptr) {
            return std::make_unique<Subtract<Real, Expression>>(Real { Real { *constReal }.GetValue() * upper_bound->GetValue() }, *Multiply<Expression> { *(constReal), lower }.Simplify());
        } else if (upper_bound == nullptr && lower_bound != nullptr) {
            return std::make_unique<Subtract<Expression, Real>>(*Multiply<Expression> { *(constReal), upper }.Simplify(), Real { Real { *constReal }.GetValue() * lower_bound->GetValue() });
        }
        return std::make_unique<Subtract<Expression>>(*Multiply<Expression> { *(constReal), upper }.Simplify(), *Multiply<Expression> { *(constReal), lower }.Simplify());
    }

    // Power Rule
    if (auto powerCase = RecursiveCast<Integral<Exponent<Expression>, Variable>>(simplifiedIntegrate); powerCase != nullptr) {
        const Variable& differential = powerCase->GetLeastSigOp();
        Oasis::Exponent exponent { powerCase->GetMostSigOp() };
        auto integral = RecursiveCast<Add<Divide<Exponent<Variable, Real>, Real>, Variable>>(*exponent.Integrate(differential));

        auto upper_bound = RecursiveCast<Real>(upper);
        auto lower_bound = RecursiveCast<Real>(lower);

        if (upper_bound != nullptr && lower_bound != nullptr) {

            //            const Real& divisor = integral->GetMostSigOp().GetLeastSigOp();
            //            const Real& power = integral->GetMostSigOp().GetMostSigOp().GetLeastSigOp();
            //            const IExpression<Real> auto& left = Exponent<Real>{ Real{upper_bound->GetValue()}, power };
            //            const IExpression<Real> auto& right = *Exponent<Real>{ Real{lower_bound->GetValue()}, power }.Simplify();

            //            return make_unique<Real>(  *Subtract<Real>{ *Divide<Real>{ left, divisor }.Simplify(), *Divide<Real>{ right, divisor }.Simplify() }.Simplify() );
        } else if (upper_bound != nullptr && lower_bound == nullptr) {

        } else if (upper_bound == nullptr && lower_bound != nullptr) {
        }
        //        return
    }
    //
    //    // Constant + Power Rule
    //    if (auto powerCase = RecursiveCast<Integrate<Multiply<Expression, Exponent<Expression>>, Variable>>(simplifiedIntegrate); powerCase != nullptr) {
    //        const Variable& differential = powerCase->GetLeastSigOp();
    //        Oasis::Exponent exponent{powerCase->GetMostSigOp().GetLeastSigOp()};
    //        auto integration = exponent.Integrate(differential);
    //
    //        return std::make_unique<Multiply<Expression>>(powerCase->GetMostSigOp().GetMostSigOp(), (*integration));
    //    }
    //
    //    if (auto powerCase = RecursiveCast<Integrate<Multiply<Exponent<Expression>, Expression>, Variable>>(simplifiedIntegrate); powerCase != nullptr) {
    //        const Variable& differential = powerCase->GetLeastSigOp();
    //        Oasis::Exponent exponent{powerCase->GetMostSigOp().GetMostSigOp()};
    //        auto integration = exponent.Integrate(differential);
    //
    //        return std::make_unique<Multiply<Expression>>(powerCase->GetMostSigOp().GetLeastSigOp(), (*integration));
    //    }

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
    // Needs Trig and Euler's Number

    // Partial Fraction Decomposition
    // Work in Progress

    //    return simplifiedIntegrate.Copy();
    return Copy();
    */
// }
} // Oasis

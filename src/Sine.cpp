//
// Created by Andrew Nazareth on 9/19/25.
//

#include "Oasis/Sine.hpp"
#include "Oasis/Expression.hpp"
#include "Oasis/UnaryExpression.hpp"

namespace Oasis {

    // // Sine(real*pi) --> some number
    // if (const auto piCase = RecursiveCast<Sine<Multiply<Real,Pi>>>(simplifiedOperand); piCase != nullptr) {
    //     return std::make_unique<Real>(Real(std::sin(piCase->GetOperand().GetMostSigOp().GetValue() * piCase->GetOperand().GetLeastSigOp().GetValue())));
    // }

    // // Sine(2npi + x) --> Sine(x)
    // if (const auto periodicCase = RecursiveCast<Sine<Add<Multiply<Real,Pi>,Expression>>>(simplifiedOperand); periodicCase != nullptr) {
    //     const Real& multreal = periodicCase->GetOperand().getMostSigOp().GetMostSigOp();
    //     if ((multreal.GetValue() % 2) == 0) {
    //         return std::make_unique<Sine<Expression>>(periodicCase->GetOperand().GetLeastSigOp());
    //     }
    // }
    //
    // return nullptr;
// }

// auto Sine<Expression>::Differentiate(const Expression&) const -> std::unique_ptr<Expression>
// {
//     return nullptr;
// }

// auto Sine<Expression>::Integrate(const Expression&) const -> std::unique_ptr<Expression>
// {
//     return nullptr;
// }
} // Oasis
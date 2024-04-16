//
//Created by Andrew Choi on 4/2/24
//

#include "Oasis/Summation.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/BinaryExpression.hpp"
#include "Oasis/Multiply.hpp"

namespace Oasis {
    Summation<Expression>::Summation(const Expression& lowBound, const Expression& upperBound, const Expression& exp)
     : BinaryExpression(lowBound, upperBound), Expression(exp) {}

    auto Summation<Expression>::Simplify() const -> std::unique_ptr<Expression> 
    {
     const auto simplifiedSum = mostSigOp ? mostSigOp->Simplify() : nullptr;
     const auto simplifiedExp = leastSigOp ? leastSigOp->Simplify() : nullptr;

     if(!simplifiedSum  || !simplifiedExp) {
        return nullptr;
     }

     const Summation simplifiedSummation { *simplifiedSum, *simplifiedExp };






    }
}
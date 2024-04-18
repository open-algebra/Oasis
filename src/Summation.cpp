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
     const auto simplifiedLow = mostSigOp ? mostSigOp->Simplify() : nullptr;
     const auto simplifiedUp = leastSigOp ? leastSigOp->Simplify() : nullptr;

     const auto simplifiedExp = exp.Simplify();

     if(!simplifiedSum  || !simplifiedExp || !simplifiedExp) {
        return nullptr;
     }

     return std::make_unique<Summation>(*simplifiedLow, *simplifiedUp, *simplifiedExp);

    }

    [[nodiscard]] auto ToString() const -> std::string final
    {
        return fmt::format("∑^{}_{}({})", this->lowBound, this->upperBound, this->exp);
    }
}
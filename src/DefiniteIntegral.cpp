//
// Created by Matthew McCall on 4/30/24.
//

#include "Oasis/DefiniteIntegral.hpp"
#include "Oasis/Subtract.hpp"

namespace Oasis {

inline SimplifyVisitor simplifyVisitor{};

template <IExpression OperandT, IExpression VariableT, IExpression LowerBoundT, IExpression UpperBoundT>
DefiniteIntegral<OperandT, VariableT, LowerBoundT, UpperBoundT>::DefiniteIntegral(const Integral<>& original)
{
    integral.SetLeastSigOp(original.GetLeastSigOp());
    integral.SetMostSigOp(original.GetMostSigOp());
}

template <IExpression OperandT, IExpression VariableT, IExpression LowerBoundT, IExpression UpperBoundT>
auto DefiniteIntegral<OperandT, VariableT, LowerBoundT, UpperBoundT>::AcceptInternal(const Visitor& visitor) -> any
{
    const auto generalized = this->Generalize();
    return visitor.Visit(generalized);
}

template <IExpression OperandT, IExpression VariableT, IExpression LowerBoundT, IExpression UpperBoundT>
auto DefiniteIntegral<OperandT, VariableT, LowerBoundT, UpperBoundT>::IntegrateWithBounds(const VariableT& var) -> std::unique_ptr<Expression>
{
    // Substitute in the upper and lower bounds for the variable
    std::unique_ptr<Expression> upperBoundResult = integral.Substitute(var, this->GetUpperBound());
    std::unique_ptr<Expression> lowerBoundResult = integral.Substitute(var, this->GetLowerBound());

    // Simplify upperBoundResult and lowerBoundResult
    upperBoundResult->Accept(simplifyVisitor);
    lowerBoundResult->Accept(simplifyVisitor);

    // Subtract the two values and simplify the result
    std::unique_ptr<Expression> result = Subtract<Expression, Expression> {
        *upperBoundResult, *lowerBoundResult
    }.Copy(); // ->Accept(simplifyVisitor);

    return result;
}

} // Oasis
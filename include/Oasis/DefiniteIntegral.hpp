//
// Created by Matthew McCall on 4/30/24.
//

#ifndef DEFINITEINTEGRAL_HPP
#define DEFINITEINTEGRAL_HPP

#include "BoundedBinaryExpression.hpp"
#include "Oasis/Integral.hpp"

namespace Oasis {

template <IExpression OperandT, IExpression VariableT, IExpression LowerBoundT, IExpression UpperBoundT>
class DefiniteIntegral : BoundedBinaryExpression<DefiniteIntegral, OperandT, VariableT, LowerBoundT, UpperBoundT> {
public:
    DefiniteIntegral(const Integral<Expression, Expression>& original);

    auto IntegrateWithBounds(const VariableT& var) -> std::unique_ptr<Expression>;

    auto AcceptInternal(const Visitor& visitor) -> any;
private:
    Integral<Expression, Expression> integral;
};

} // Oasis

#endif // DEFINITEINTEGRAL_HPP

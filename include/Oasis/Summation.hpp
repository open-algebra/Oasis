//
// Created by Matthew McCall on 5/2/24.
//

#ifndef OASIS_SUMMATION_HPP
#define OASIS_SUMMATION_HPP

#include "BoundedBinaryExpression.hpp"

namespace Oasis {

template <IExpression OperandT, IExpression IncrementingVarT, IExpression LowerBoundT, IExpression UpperBoundT>
class Summation final : public BoundedBinaryExpression<Summation, OperandT, IncrementingVarT, LowerBoundT, UpperBoundT> {
};

} // Oasis

#endif // OASIS_SUMMATION_HPP

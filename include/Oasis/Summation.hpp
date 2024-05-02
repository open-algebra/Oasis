//
// Created by Matthew McCall on 5/2/24.
//

#ifndef OASIS_SUMMATION_HPP
#define OASIS_SUMMATION_HPP

#include "BoundedUnaryExpression.hpp"

namespace Oasis {

template <IExpression OperandT, IExpression LowerBoundT, IExpression UpperBoundT>
class Summation : public BoundedUnaryExpression<Summation, OperandT, LowerBoundT, UpperBoundT> {
};

} // Oasis

#endif // OASIS_SUMMATION_HPP

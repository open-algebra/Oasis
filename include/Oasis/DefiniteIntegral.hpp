//
// Created by Matthew McCall on 4/30/24.
//

#ifndef DEFINITEINTEGRAL_HPP
#define DEFINITEINTEGRAL_HPP

#include "BoundedBinaryExpression.hpp"

namespace Oasis {

template <IExpression OperandT, IExpression VariableT, IExpression LowerBoundT, IExpression UpperBoundT>
class DefiniteIntegral final : BoundedBinaryExpression<DefiniteIntegral, OperandT, VariableT, LowerBoundT, UpperBoundT> { };

} // Oasis

#endif // DEFINITEINTEGRAL_HPP
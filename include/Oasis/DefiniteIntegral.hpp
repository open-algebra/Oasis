//
// Created by Matthew McCall on 4/30/24.
//

#ifndef DEFINITEINTEGRAL_HPP
#define DEFINITEINTEGRAL_HPP

#include "TernaryExpression.hpp"

namespace Oasis {

template <IExpression OperandT, IExpression LowerBoundT, IExpression UpperBoundT>
class DefiniteIntegral : public TernaryExpression<DefiniteIntegral, OperandT, LowerBoundT, UpperBoundT> {
};

} // Oasis

#endif // DEFINITEINTEGRAL_HPP

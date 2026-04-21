//
// Created by Matthew McCall on 3/29/24.
//

#ifndef OASIS_NEGATE_HPP
#define OASIS_NEGATE_HPP

#include "Multiply.hpp"
#include "UnaryExpression.hpp"

namespace Oasis {

template <typename OperandT = Expression>
class Negate final : public UnaryExpression<Negate, OperandT> {
public:
    Negate() = default;
    Negate(const Negate& other)
        : UnaryExpression<Negate, OperandT>(other)
    {
    }

    explicit Negate(const OperandT& operand)
        : UnaryExpression<Negate, OperandT>(operand)
    {
    }

    EXPRESSION_TYPE(Negate)
    EXPRESSION_CATEGORY(UnExp)
};

} // Oasis

#endif // OASIS_NEGATE_HPP

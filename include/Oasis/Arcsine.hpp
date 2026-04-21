//
// Created by Gregory Lemonnier on 3/27/26.
//

#ifndef OASIS_ARCSINE_HPP
#define OASIS_ARCSINE_HPP

#include "Expression.hpp"
#include "UnaryExpression.hpp"

namespace Oasis {

template <typename OperandT = Expression>
class Arcsine final : public UnaryExpression<Arcsine, OperandT> {
public:
    Arcsine() = default;
    Arcsine(const Arcsine& other)
        : UnaryExpression<Arcsine, OperandT>(other)
    {
    }

    explicit Arcsine(const OperandT& operand)
        : UnaryExpression<Arcsine, OperandT>(operand)
    {
    }


    [[nodiscard]] auto Integrate(const Expression& var) const -> std::unique_ptr<Expression> final;

    EXPRESSION_TYPE(Arcsine)
    EXPRESSION_CATEGORY(UnExp)
};

} // Oasis

#endif // OASIS_ARCSINE_HPP

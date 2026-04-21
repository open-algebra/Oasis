//
// Created by Gregory Lemonnier 2/xx/26.
//

#ifndef OASIS_ARCCOSINE_HPP
#define OASIS_ARCCOSINE_HPP

#include "Expression.hpp"
#include "UnaryExpression.hpp"

namespace Oasis {

template <typename OperandT = Expression>
class Arccosine final :public UnaryExpression<Arccosine, OperandT> {
public:
    Arccosine() = default;
    Arccosine(const Arccosine& other)
        : UnaryExpression<Arccosine, OperandT>(other)
    {
    }

    explicit Arccosine(const OperandT& operand)
        : UnaryExpression<Arccosine, OperandT>(operand)
    {
    }

    [[nodiscard]] auto Integrate(const Expression& var) const -> std::unique_ptr<Expression> final;

    EXPRESSION_TYPE(Arccosine)
    EXPRESSION_CATEGORY(UnExp)
};

} // Oasis

#endif // OASIS_ARCCOSINE_HPP
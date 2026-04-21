//
// Created by Gregory Lemonnier 2/xx/26.
//

#ifndef OASIS_COSINE_HPP
#define OASIS_COSINE_HPP

#include "Expression.hpp"
#include "UnaryExpression.hpp"

namespace Oasis {

template <typename OperandT = Expression>
class Cosine final :public UnaryExpression<Cosine, OperandT> {
public:
    Cosine() = default;
    Cosine(const Cosine& other)
        : UnaryExpression<Cosine, OperandT>(other)
    {
    }

    explicit Cosine(const OperandT& operand)
        : UnaryExpression<Cosine, OperandT>(operand)
    {
    }

    [[nodiscard]] auto Integrate(const Expression& vra) const -> std::unique_ptr<Expression> final;

    EXPRESSION_TYPE(Cosine)
    EXPRESSION_CATEGORY(UnExp)
};

} // Oasis

#endif // OASIS_COSINE_HPP
//
// Created by Andrew Nazareth on 10/8/24.
//

#ifndef OASIS_SINE_HPP
#define OASIS_SINE_HPP

#include "Real.hpp"
#include "UnaryExpression.hpp"

namespace Oasis {

// TODO: Fix so function(s) can be implemented in other files.

template <IExpression OperandT>
class Sine;

/// @cond
template <>
class Sine<Expression> : public UnaryExpression<Sine, Expression> {
public:
    //    using UnaryExpression::UnaryExpression;

    Sine() = default;
    Sine(const Sine<Expression>& other) = default;

    Sine(const Expression& Operand);

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression> final;
    [[nodiscard]] auto Differentiate(const Expression& differentiationVariable) const -> std::unique_ptr<Expression> final;

    DECL_SPECIALIZE(Sine)

    EXPRESSION_TYPE(Sine)
    EXPRESSION_CATEGORY(UnExp)
};
/// @endcond

/**
 * The Sine expression calculates the sine value of the operand.
 *
 * @tparam OperandT The type of the expression to add be added to.
 */
template <IExpression OperandT = Expression>
class Sine : public UnaryExpression<Sine, OperandT> {
public:
    Sine() = default;
    Sine(const Sine<OperandT>& other)
        : UnaryExpression<Oasis::Sine, OperandT>(other)
    {
    }

    explicit Sine(const OperandT& operand)
        : UnaryExpression<Oasis::Sine, OperandT>(operand)
    {
    }

    IMPL_SPECIALIZE_UNARYEXPR(Sine, OperandT)

    auto operator=(const Sine& other) -> Sine& = default;

    EXPRESSION_TYPE(Sine)
    EXPRESSION_CATEGORY(0)
};

}

#endif // OASIS_SINE_HPP

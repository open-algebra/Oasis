//
// Created by bachia on 4/12/2024.
//

#ifndef OASIS_DERIVATIVE_HPP
#define OASIS_DERIVATIVE_HPP

#include "BinaryExpression.hpp"

namespace Oasis {

template <IExpression Exp, IExpression Var>
class Derivative;

/// @cond
template <>
class Derivative<Expression, Expression> : public BinaryExpression<Derivative> {
public:
    Derivative() = default;
    Derivative(const Derivative<Expression, Expression>& other) = default;

    Derivative(const Expression& Exp, const Expression& Var);

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto Differentiate(const Expression& differentiationVariable) const -> std::unique_ptr<Expression> override;

    static auto Specialize(const Expression& other) -> std::unique_ptr<Derivative>;

    EXPRESSION_TYPE(Derivative)
    EXPRESSION_CATEGORY(BinExp)
};
/// @endcond

/**
 * The Derivative class template calculates the derivative of given expressions.
 *
 * @tparam DependentT The expression type that the derivative will be calculated of.
 * @tparam IndependentT The type of the variable with respect to which the derivative will be calculated.
 */
template <IExpression DependentT = Expression, IExpression IndependentT = DependentT>
class Derivative : public BinaryExpression<Derivative, DependentT, IndependentT> {
public:
    Derivative() = default;
    Derivative(const Derivative<DependentT, IndependentT>& other)
        : BinaryExpression<Derivative, DependentT, IndependentT>(other)
    {
    }

    Derivative(const DependentT& exp, const IndependentT& var)
        : BinaryExpression<Derivative, DependentT, IndependentT>(exp, var)
    {
    }

    IMPL_SPECIALIZE(Derivative, DependentT, IndependentT)

    auto operator=(const Derivative& other) -> Derivative& = default;

    EXPRESSION_TYPE(Derivative)
    EXPRESSION_CATEGORY(BinExp)
};

} // namespace Oasis

#endif // OASIS_DERIVATIVE_HPP

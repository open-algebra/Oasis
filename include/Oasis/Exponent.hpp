//
// Created by Andrew Nazareth on 9/19/23.
//

#ifndef OASIS_EXPONENT_HPP
#define OASIS_EXPONENT_HPP

#include "BinaryExpression.hpp"
#include "Variable.hpp"

namespace Oasis {

template <IExpression BaseT, IExpression PowerT>
class Exponent;

/// @cond
template <>
class Exponent<Expression, Expression> : public BinaryExpression<Exponent> {
public:
    Exponent() = default;
    Exponent(const Exponent<Expression, Expression>& other) = default;

    Exponent(const Expression& base, const Expression& power);

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto Differentiate(const Expression& differentiationVariable) const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression> final;

    static auto Specialize(const Expression& other) -> std::unique_ptr<Exponent>;

    EXPRESSION_TYPE(Exponent)
    EXPRESSION_CATEGORY(BinExp)
};
/// @endcond

/**
 * The exponent expression creates an exponent two expressions.
 *
 * @tparam BaseT The expression to be used as the base.
 * @tparam PowerT The expression to be used as the power.
 */
template <IExpression BaseT = Expression, IExpression PowerT = BaseT>
class Exponent : public BinaryExpression<Exponent, BaseT, PowerT> {
public:
    Exponent() = default;
    Exponent(const Exponent<BaseT, PowerT>& other)
        : BinaryExpression<Exponent, BaseT, PowerT>(other)
    {
    }

    Exponent(const BaseT& base, const PowerT& power)
        : BinaryExpression<Exponent, BaseT, PowerT>(base, power)
    {
    }

    IMPL_SPECIALIZE(Exponent, BaseT, PowerT)

    auto operator=(const Exponent& other) -> Exponent& = default;

    EXPRESSION_TYPE(Exponent)
    EXPRESSION_CATEGORY(BinExp)
};

}
#endif // OASIS_EXPONENT_HPP

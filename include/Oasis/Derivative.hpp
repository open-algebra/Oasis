//
// Created by bachia on 4/12/2024.
//

#ifndef OASIS_DERIVATIVE_HPP
#define OASIS_DERIVATIVE_HPP

#include "fmt/core.h"

#include "BinaryExpression.hpp"
#include "Expression.hpp"
#include "Real.hpp"

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
    // auto Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto ToString() const -> std::string final;

    static auto Specialize(const Expression& other) -> std::unique_ptr<Derivative>;
    static auto Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Derivative>;

    EXPRESSION_TYPE(Derivative)
    EXPRESSION_CATEGORY(BinExp)
};
/// @endcond

/**
 * The Derivative expression Derivatives the expression given
 *
 * @tparam Exp The expression to be Derived.
 * @tparam Var The variable to be Derived on.
 */
template <IExpression Exp = Expression, IExpression Var = Exp>
class Derivative : public BinaryExpression<Derivative, Exp, Var> {
public:
    Derivative() = default;
    Derivative(const Derivative<Exp, Var>& other)
        : BinaryExpression<Derivative, Exp, Var>(other)
    {
    }

    Derivative(const Exp& exp, const Var& var)
        : BinaryExpression<Derivative, Exp, Var>(exp, var)
    {
    }

    [[nodiscard]] auto ToString() const -> std::string final
    {
        return fmt::format("(d/d{}({}))", this->leastSigOp->ToString(), this->mostSigOp->ToString());
    }

    IMPL_SPECIALIZE(Derivative, Exp, Var)

    auto operator=(const Derivative& other) -> Derivative& = default;

    EXPRESSION_TYPE(Derivative)
    EXPRESSION_CATEGORY(BinExp)
};

} // namespace Oasis

#endif // OASIS_DERIVATIVE_HPP

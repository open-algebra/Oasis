//
// Created by Levy Lin on 2/09/2024.
//

#ifndef OASIS_INTEGRATE_HPP
#define OASIS_INTEGRATE_HPP

#include "BinaryExpression.hpp"
#include "Expression.hpp"
#include "Real.hpp"

namespace Oasis {

template <IExpression Integrand, IExpression Differential>
class Integral;

/// @cond
template <>
class Integral<Expression, Expression> : public BinaryExpression<Integral> {
public:
    Integral() = default;
    Integral(const Integral<Expression, Expression>& other) = default;

    Integral(const Expression& integrand, const Expression& differential);

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> final;
    [[nodiscard]] auto Simplify(const Expression& upper, const Expression& lower) const -> std::unique_ptr<Expression> /* final */;

    static auto Specialize(const Expression& other) -> std::unique_ptr<Integral>;

    EXPRESSION_TYPE(Integral)
    EXPRESSION_CATEGORY(Associative | Commutative)
};
/// @endcond

/**
 * The Integral expression integrates the two expressions together.
 *
 * @tparam IntegrandT The type of the expression to be integrated.
 * @tparam DifferentialT The type of the expression defining which variable is integrating in the IntegrandT.
 */
template <IExpression IntegrandT = Expression, IExpression DifferentialT = IntegrandT>
class Integral : public BinaryExpression<Integral, IntegrandT, DifferentialT> {
public:
    Integral() = default;
    Integral(const Integral<IntegrandT, DifferentialT>& other)
        : BinaryExpression<Integral, IntegrandT, DifferentialT>(other)
    {
    }

    Integral(const IntegrandT& integrand, const DifferentialT& differential)
        : BinaryExpression<Integral, IntegrandT, DifferentialT>(integrand, differential)
    {
    }

    IMPL_SPECIALIZE(Integral, IntegrandT, DifferentialT)

    auto operator=(const Integral& other) -> Integral& = default;

    EXPRESSION_TYPE(Integral)
    EXPRESSION_CATEGORY(Associative | Commutative)
};

} // namespace Oasis

#endif // OASIS_INTEGRATE_HPP

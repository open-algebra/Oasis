//
// Created by linle on 2/24/2024.
//

#ifndef OASIS_INTEGRATE_HPP
#define OASIS_INTEGRATE_HPP

#include "fmt/core.h"

#include "Expression.hpp"
#include "BinaryExpression.hpp"
#include "Real.hpp"

namespace Oasis {

template <IExpression Integrand, IExpression Differential>
class Integrate;


/// @cond
template<>
class Integrate<Expression, Expression> : public BinaryExpression<Integrate> {
public:
    Integrate() = default;
    Integrate(const Integrate<Expression, Expression>& other) = default;

    Integrate(const Expression& integrand, const Expression& differential);

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> final;
    auto Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto ToString() const -> std::string final;

    static auto Specialize(const Expression& other) -> std::unique_ptr<Integrate>;
    static auto Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Integrate>;

    EXPRESSION_TYPE(Integrate)
    EXPRESSION_CATEGORY(Associative | Commutative)
};
/// @endcond

/**
 * The Integrate expression integrates the two expressions together.
 *
 * @tparam IntegrandT The type of the expression to be integrated.
 * @tparam DifferentialT The type of the expression defining which variable is integrating in the IntegrandT.
 */
template <IExpression IntegrandT = Expression, IExpression DifferentialT = IntegrandT>
class Integrate : public BinaryExpression<Integrate, IntegrandT, DifferentialT> {
public:
    Integrate() = default;
    Integrate(const Integrate<IntegrandT, DifferentialT>& other)
        : BinaryExpression<Integrate, IntegrandT, DifferentialT>(other)
    {
    }

    Integrate(const IntegrandT& integrate, const DifferentialT& differential)
        : BinaryExpression<Integrate, IntegrandT, DifferentialT>(integrate, differential)
    {
    }

    [[nodiscard]] auto ToString() const -> std::string final
    {
        return fmt::format("({} + {})", this->mostSigOp->ToString(), this->leastSigOp->ToString());
    }

    IMPL_SPECIALIZE(Integrate, IntegrandT, DifferentialT)

    auto operator=(const Integrate& other) -> Integrate& = default;

    EXPRESSION_TYPE(Integrate)
    EXPRESSION_CATEGORY(Associative | Commutative)
};

} // namespace Oasis

#endif // OASIS_INTEGRATE_HPP

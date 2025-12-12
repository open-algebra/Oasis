//
// Created by Levy Lin on 2/09/2024.
//

#ifndef OASIS_INTEGRATE_HPP
#define OASIS_INTEGRATE_HPP

#include "BinaryExpression.hpp"
#include "Expression.hpp"
#include "Real.hpp"

namespace Oasis {

/// @cond
template <>
class Integral<Expression, Expression> : public BinaryExpression<Integral> {
public:
    Integral() = default;
    Integral(const Integral<Expression, Expression>& other) = default;

    Integral(const Expression& integrand, const Expression& differential);

    [[deprecated]] [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> final;

    [[deprecated]] [[nodiscard]] auto Simplify(const Expression& upper, const Expression& lower) const -> std::unique_ptr<Expression> /* final */;

    EXPRESSION_TYPE(Integral)
    EXPRESSION_CATEGORY(Associative | Commutative)
};
/// @endcond

/**
 * The Integral expression integrates the two expressions together.
 *
 * @section param Parameters:
 * @tparam IntegrandT The type of the expression to be integrated.
 * @tparam DifferentialT The type of the expression defining which variable is integrating in the IntegrandT.
 *
 * @section examples Example Usage:
 * @code
std::string exp = {"x+5"};

const auto preproc = Oasis::PreProcessInFix(exp);
auto midResult = Oasis::FromInFix(preproc);

const std::unique_ptr<Oasis::Expression> expression = std::move(midResult).value();

Oasis::Integral in {
    *expression,
    Oasis::Variable{"x"}
};
Oasis::InFixSerializer result;

auto resultant = in.Simplify();

std::println("Result: {}",resultant->Accept(result).value());
// Will print (((5*x)+(0.5*(x^2)))+C)
 * @endcode
 */
template <typename IntegrandT = Expression, typename DifferentialT = IntegrandT>
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

    auto operator=(const Integral& other) -> Integral& = default;

    EXPRESSION_TYPE(Integral)
    EXPRESSION_CATEGORY(Associative | Commutative)
};

} // namespace Oasis

#endif // OASIS_INTEGRATE_HPP

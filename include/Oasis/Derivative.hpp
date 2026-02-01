//
// Created by bachia on 4/12/2024.
//

#ifndef OASIS_DERIVATIVE_HPP
#define OASIS_DERIVATIVE_HPP

#include "BinaryExpression.hpp"

namespace Oasis {

/// @cond
template <>
class Derivative<Expression, Expression> : public BinaryExpression<Derivative> {
public:
    Derivative() = default;
    Derivative(const Derivative<Expression, Expression>& other) = default;

    Derivative(const Expression& Exp, const Expression& Var);

    [[deprecated]] [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto Differentiate(const Expression& differentiationVariable) const -> std::unique_ptr<Expression> override;

    EXPRESSION_TYPE(Derivative)
    EXPRESSION_CATEGORY(BinExp)
};
/// @endcond

/**
 * The Derivative class template calculates the derivative of given expressions.
 * The Derivative class template can take two and only two parameters.
 *
 * @section Parameters:
 * @tparam DependentT The expression type that the derivative will be calculated of.
 * @tparam IndependentT The type of the variable with respect to which the derivative will be calculated.
 *
 * @section Example Usage:
 *
 *
 * @subsection simple Simple derivative function
 * @code
 * std::string exp = {"3x^3+x^2+5x+1"};

    const auto preproc = Oasis::PreProcessInFix(exp);
    auto midResult = Oasis::FromInFix(preproc);

    const std::unique_ptr<Oasis::Expression> expression = std::move(midResult).value();

    Oasis::Derivative dv {
        *expression,
        Oasis::Variable{"x"}
    };
    Oasis::InFixSerializer result;

    auto resultant = dv.Simplify();

    std::println("Result: {}", resultant->Accept(result).value());
    // Will print: (((9*(x^2))+(2*x))+5)
 * @endcode
 *
 */
template <typename DependentT = Expression, typename IndependentT = DependentT>
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

    auto operator=(const Derivative& other) -> Derivative& = default;

    EXPRESSION_TYPE(Derivative)
    EXPRESSION_CATEGORY(BinExp)
};

} // namespace Oasis

#endif // OASIS_DERIVATIVE_HPP

//
// Created by Andrew Nazareth on 9/19/23.
//

#ifndef OASIS_EXPONENT_HPP
#define OASIS_EXPONENT_HPP

#include "BinaryExpression.hpp"
#include "Variable.hpp"

namespace Oasis {

/// @cond
template <>
class Exponent<Expression, Expression> : public BinaryExpression<Exponent> {
public:
    Exponent() = default;
    Exponent(const Exponent<Expression, Expression>& other) = default;

    Exponent(const Expression& base, const Expression& power);

    [[deprecated]] [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto Differentiate(const Expression& differentiationVariable) const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression> final;

    EXPRESSION_TYPE(Exponent)
    EXPRESSION_CATEGORY(BinExp)
};
/// @endcond

/**
 * The exponent expression creates an exponent two expressions.
 *
 * @section param Parameters:
 * @tparam BaseT The expression to be used as the base.
 * @tparam PowerT The expression to be used as the power.
 *
 * @section examples Example Usage:
 *
 * @subsection eval Evaluating Real values:
 * @code
 * // std::string exp = {"x+5"};

    Oasis::Exponent exp {
        Oasis::Real{5},
        Oasis::Real{5}
    };
    Oasis::InFixSerializer result;

    auto resultant = exp.Simplify();

    std::println("Result: {}", resultant->Accept(result).value());
    // Will print: 3125
 * @endcode
 *
 * @subsection expr Raising an Expression by another Expression:
 * @note While Exponent will accept two expressions, it may not be in its simplest form.
 * @code
 * std::string exp1 = {"x+5"};
    std::string exp2 = {"2x+3y+12"};

    const auto preproc1 = Oasis::PreProcessInFix(exp1);
    const auto preproc2 = Oasis::PreProcessInFix(exp2);
    auto midResult1 = Oasis::FromInFix(preproc1);
    auto midResult2 = Oasis::FromInFix(preproc2);

    const std::unique_ptr<Oasis::Expression> expression1 = std::move(midResult1).value();
    const std::unique_ptr<Oasis::Expression> expression2 = std::move(midResult2).value();

    Oasis::Exponent exp {
        *expression1,
        *expression2
    };
    Oasis::InFixSerializer result;

    auto resultant = exp.Simplify();

    std::println("Result: {}", resultant->Accept(result).value());
    // Will print: ((x+5)^(((2*x)+(3*y))+12))
 * @endcode
 *
 */
template <typename BaseT = Expression, typename PowerT = BaseT>
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

    auto operator=(const Exponent& other) -> Exponent& = default;

    EXPRESSION_TYPE(Exponent)
    EXPRESSION_CATEGORY(BinExp)
};

}
#endif // OASIS_EXPONENT_HPP

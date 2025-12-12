//
// Created by Matthew McCall on 8/10/23.
//

#ifndef OASIS_MULTIPLY_HPP
#define OASIS_MULTIPLY_HPP

#include "BinaryExpression.hpp"
#include "Real.hpp"

namespace Oasis {

/// @cond
template <>
class Multiply<Expression, Expression> : public BinaryExpression<Multiply> {
public:
    using BinaryExpression::BinaryExpression;

    [[deprecated]] [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto Differentiate(const Expression& differentiationVariable) const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression> final;

    EXPRESSION_TYPE(Multiply)
    EXPRESSION_CATEGORY(Associative | Commutative | BinExp)
};
/// @endcond

/**
 * The Multiply expression multiplies two expressions.
 * The Multiply expression accepts two and only two expressions.
 *
 * @section param Parameters
 * @tparam MultiplicandT The expression to be multiplied by.
 * @tparam MultiplierT The expression to multiply the multiplicand by.
 *
 * @section examples Example Usage:
 *
 * @subsection real Multiplying two Real values together:
 * @code
 * Oasis::Multiply myMult {
        Oasis::Real{12},
        Oasis::Real{5}
    };
    Oasis::InFixSerializer result;

    auto resultant = myMult.Simplify();

    std::println("Result: {}", resultant->Accept(result).value());
    // Will print: 60
 * @endcode
 *
 * @subsection var Multiplying one Real and one Variable value together:
 * @code
 * Oasis::Multiply myMult {
        Oasis::Variable{"x"},
        Oasis::Real{5}
    };
    Oasis::InFixSerializer result;

    auto resultant = myMult.Simplify();

    std::println("Result: {}", resultant->Accept(result).value());
    // Will print: (x*5)
 * @endcode
 *
 * @subsection varSame Multiplying two Variable values together:
 * @code
 * Oasis::Multiply myMult {
        Oasis::Variable{"x"},
        Oasis::Variable{"x"}
    };
    Oasis::InFixSerializer result;

    auto resultant = myMult.Simplify();

    std::println("Result: {}", resultant->Accept(result).value());
    // Will print: (x^2)
 * @endcode
 *
 * or if they are different variables:
 *
 * @code
 * Oasis::Multiply myMult {
        Oasis::Variable{"x"},
        Oasis::Variable{"y"}
    };
    Oasis::InFixSerializer result;

    auto resultant = myMult.Simplify();

    std::println("Result: {}", resultant->Accept(result).value());
    // Will print: (x*y)
 * @endcode
 *
 * @subsection exprMult Multiplying two expressions together:
 * @note The Multiply class can store two expressions, but they will not be in the most simplified form.
 * @code
 * std::string expr1 = {"2x+3y+15"};
    std::string expr2 = {"5x+9y-10"};

    const auto prepoc1 = Oasis::PreProcessInFix(expr1);
    const auto prepoc2 = Oasis::PreProcessInFix(expr2);

    auto midResult1 = Oasis::FromInFix(prepoc1);
    auto midResult2 = Oasis::FromInFix(prepoc2);

    const std::unique_ptr<Oasis::Expression> expression1 = std::move(midResult1).value();
    const std::unique_ptr<Oasis::Expression> expression2 = std::move(midResult2).value();

    Oasis::Multiply myMult {
        *expression1,
        *expression2
    };
    Oasis::InFixSerializer result;

    auto resultant = myMult.Simplify();

    std::println("Result: {}", resultant->Accept(result).value());
    // Will print: ((((2*x)+(3*y))+15)*(((5*x)+(9*y))+-10))
 * @endcode
 *
 */
template <typename MultiplicandT = Expression, typename MultiplierT = MultiplicandT>
class Multiply : public BinaryExpression<Multiply, MultiplicandT, MultiplierT> {
public:
    Multiply() = default;
    Multiply(const Multiply<MultiplicandT, MultiplierT>& other)
        : BinaryExpression<Multiply, MultiplicandT, MultiplierT>(other)
    {
    }

    Multiply(const MultiplicandT& addend1, const MultiplierT& addend2)
        : BinaryExpression<Multiply, MultiplicandT, MultiplierT>(addend1, addend2)
    {
    }

    auto operator=(const Multiply& other) -> Multiply& = default;

    EXPRESSION_TYPE(Multiply)
    EXPRESSION_CATEGORY(Associative | Commutative | BinExp)
};

} // Oasis

#endif // OASIS_MULTIPLY_HPP

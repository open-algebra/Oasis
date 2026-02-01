//
// Created by Matthew McCall on 8/10/23.
//

#ifndef OASIS_SUBTRACT_HPP
#define OASIS_SUBTRACT_HPP

#include "BinaryExpression.hpp"
#include "Real.hpp"

namespace Oasis {

/// @cond
template <>
class Subtract<Expression, Expression> : public BinaryExpression<Subtract> {
public:
    Subtract() = default;
    Subtract(const Subtract<Expression, Expression>& other) = default;

    Subtract(const Expression& minuend, const Expression& subtrahend);

    [[deprecated]] [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto Differentiate(const Expression& differentiationVariable) const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression> final;

    EXPRESSION_TYPE(Subtract)
    EXPRESSION_CATEGORY(BinExp)
};
/// @endcond

/**
 * The Subtract expression subtracts two expressions.
 *
 * @section Parameters
 * @tparam MinuendT The expression to be subtracted from.
 * @tparam SubtrahendT The expression to subtract from the minuend.
 *
 * @section Examples
 * The Subtract expression can take in a multitude of Oasis classes.
 *
 * @subsection simp Subtracting two Real values from each other.
 * @code
 * Oasis::Subtract sub {
        Oasis::Real{4},
        Oasis::Real{5}
    };
    Oasis::InFixSerializer result;

    auto resultant = sub.Simplify();

    std::println("Result: {}", resultant->Accept(result).value());
    // Will print: -1
 * @endcode
 *
 * @subsection var_val Subtracting one Real value from a Variable expression:
 * @code
 * Oasis::Subtract sub {
        Oasis::Variable{"x"},
        Oasis::Real{5}
    };
    Oasis::InFixSerializer result;

    auto resultant = sub.Simplify();

    std::println("Result: {}", resultant->Accept(result).value());
    // Will print: x+-5
 * @endcode
 *
 * @subsection var_var Subtracting two variables from each other if they are the same:
 * @code
 * Oasis::Subtract sub {
        Oasis::Variable{"x"},
        Oasis::Variable{"x"}
    };
    Oasis::InFixSerializer result;

    auto resultant = sub.Simplify();

    std::println("Result: {}", resultant->Accept(result).value());
    // Will print: 0
 * @endcode
 *
 * @subsection var_var_xy Subtracting two variables from each other if they are different:
 * @code
 * Oasis::Subtract sub {
        Oasis::Variable{"x"},
        Oasis::Variable{"y"}
    };
    Oasis::InFixSerializer result;

    auto resultant = sub.Simplify();

    std::println("Result: {}", resultant->Accept(result).value());
    // Will print: (x+(-1*y))
 * @endcode
 *
 * @subsection expr_expr Subtracting two expressions from each other:
 *
 * @note As of right now, while Oasis will accept two expressions into its constructor, results may be inaccurate. For example,
 * expressions containing more than two variables will produce inaccurate results. See Example 2 down below.
 *
 *
 * @subsection ex1 Example 1:
 * @code
 * std::string exp1 = {"2y-15"};
    std::string exp2 = {"4y-10"};

    const auto preproc1 = Oasis::PreProcessInFix(exp1);
    auto midResult1 = Oasis::FromInFix(preproc1);

    const std::unique_ptr<Oasis::Expression> expression1 = std::move(midResult1).value();

    const auto preproc2 = Oasis::PreProcessInFix(exp2);
    auto midResult2 = Oasis::FromInFix(preproc2);

    const std::unique_ptr<Oasis::Expression> expression2 = std::move(midResult2).value();


    Oasis::Subtract sub {
        *expression1,
        *expression2
    };
    Oasis::InFixSerializer result;

    auto resultant = sub.Simplify();

    std::println("Result: {}", resultant->Accept(result).value());
    // Will print: ((-2*y)+-5)
 * @endcode
 *
 * @subsection ex2 Example 2 (Inaccurate Results):
 * @code
 * std::string exp1 = {"4x+2y-15"};
    std::string exp2 = {"2x+4y-10"};

    const auto preproc1 = Oasis::PreProcessInFix(exp1);
    auto midResult1 = Oasis::FromInFix(preproc1);

    const std::unique_ptr<Oasis::Expression> expression1 = std::move(midResult1).value();

    const auto preproc2 = Oasis::PreProcessInFix(exp2);
    auto midResult2 = Oasis::FromInFix(preproc2);

    const std::unique_ptr<Oasis::Expression> expression2 = std::move(midResult2).value();

    Oasis::Subtract sub {
        *expression1,
        *expression2
    };
    Oasis::InFixSerializer result;

    auto resultant = sub.Simplify();

    std::println("Result: {}", resultant->Accept(result).value());
    // Will print: (((4*x)+(2*y))+-5)
 * @endcode
 */
template <typename MinuendT = Expression, typename SubtrahendT = MinuendT>
class Subtract : public BinaryExpression<Subtract, MinuendT, SubtrahendT> {
public:
    Subtract() = default;
    Subtract(const Subtract<MinuendT, SubtrahendT>& other)
        : BinaryExpression<Subtract, MinuendT, SubtrahendT>(other)
    {
    }

    Subtract(const MinuendT& addend1, const SubtrahendT& addend2)
        : BinaryExpression<Subtract, MinuendT, SubtrahendT>(addend1, addend2)
    {
    }

    auto operator=(const Subtract& other) -> Subtract& = default;

    EXPRESSION_TYPE(Subtract)
    EXPRESSION_CATEGORY(BinExp)
};

} // Oasis

#endif // OASIS_SUBTRACT_HPP

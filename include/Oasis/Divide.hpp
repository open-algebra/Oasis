//
// Created by Matthew McCall on 8/10/23.
//

#ifndef OASIS_DIVIDE_HPP
#define OASIS_DIVIDE_HPP

#include "BinaryExpression.hpp"

namespace Oasis {

/// @cond
template <>
class Divide<Expression, Expression> : public BinaryExpression<Divide> {
public:
    Divide() = default;
    Divide(const Divide<Expression, Expression>& other) = default;

    Divide(const Expression& dividend, const Expression& divisor);

    [[deprecated]] [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto Differentiate(const Expression& differentiationVariable) const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression> final;

    EXPRESSION_TYPE(Divide)
    EXPRESSION_CATEGORY(BinExp)
};
/// @endcond

/**
 * The Divide expression divides two expressions.
 * The Divide expression takes two and only two expressions.
 *
 * @section param Parameters:
 * @tparam DividendT The expression to be divided into.
 * @tparam DivisorT The expression to divide the dividend by.
 *
 * @note If a 0 is passed into the DivisorT parameter, Oasis will evaluate it as DividendT*inf.
 *
 * @section examples Example Usage:
 *
 * @subsection real Dividing two Real values together:
 *
 * @code
 * Oasis::Divide myDiv {
        Oasis::Real{50},
        Oasis::Real{12}
    };
    Oasis::InFixSerializer result;

    auto resultant = myDiv.Simplify();

    std::println("Result: {}", resultant->Accept(result).value());
    // Will print: 4.1667
 * @endcode
 *
 * @subsection var Dividing a Variable and Real value together:
 * Oasis will convert the expression into a multiplication equivalent instead. See below
 * @code
 * Oasis::Divide myDiv {
        Oasis::Variable{"x"},
        Oasis::Real{12}
    };
    Oasis::InFixSerializer result;

    auto resultant = myDiv.Simplify();

    std::println("Result: {}", resultant->Accept(result).value());
    // Will print: (x*0.083333)
 * @endcode
 *
 * @subsection var_var Dividing a Variable and a Variable value together:
 * @code
 * Oasis::Divide myDiv {
        Oasis::Variable{"x"},
        Oasis::Variable{"y"}
    };
    Oasis::InFixSerializer result;

    auto resultant = myDiv.Simplify();

    std::println("Result: {}", resultant->Accept(result).value());
    // Will print: (x/y)
 * @endcode
 *
 * @subsection expressions Dividing two expressions together:
 * Oasis will accept two expressions, but it will not display as its simplest form. See below for example.
 * @code
 * std::string expr1 = {"2x+3y+15"};
    std::string expr2 = {"5x+9y-10"};

    const auto prepoc1 = Oasis::PreProcessInFix(expr1);
    const auto prepoc2 = Oasis::PreProcessInFix(expr2);

    auto midResult1 = Oasis::FromInFix(prepoc1);
    auto midResult2 = Oasis::FromInFix(prepoc2);

    const std::unique_ptr<Oasis::Expression> expression1 = std::move(midResult1).value();
    const std::unique_ptr<Oasis::Expression> expression2 = std::move(midResult2).value();

    Oasis::Divide myDiv {
        *expression1,
        *expression2
    };
    Oasis::InFixSerializer result;

    auto resultant = myDiv.Simplify();

    std::println("Result: {}", resultant->Accept(result).value());
    // Will print: ((((2*x)+(3*y))+15)/(((5*x)+(9*y))+-10))
 * @endcode
 *
 */
template <typename DividendT = Expression, typename DivisorT = DividendT>
class Divide : public BinaryExpression<Divide, DividendT, DivisorT> {
public:
    Divide() = default;
    Divide(const Divide<DividendT, DivisorT>& other)
        : BinaryExpression<Divide, DividendT, DivisorT>(other)
    {
    }

    Divide(const DividendT& addend1, const DivisorT& addend2)
        : BinaryExpression<Divide, DividendT, DivisorT>(addend1, addend2)
    {
    }

    auto operator=(const Divide& other) -> Divide& = default;

    EXPRESSION_TYPE(Divide)
    EXPRESSION_CATEGORY(BinExp)
};

} // Oasis

#endif // OASIS_DIVIDE_HPP

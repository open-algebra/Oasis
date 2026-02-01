//
// Created by Matthew McCall on 7/2/23.
//

#ifndef OASIS_ADD_HPP
#define OASIS_ADD_HPP

#include "BinaryExpression.hpp"
#include "Real.hpp"

namespace Oasis {

/// @cond
template <>
class Add<
    Expression, Expression> : public BinaryExpression<Add> {
public:
    using BinaryExpression::BinaryExpression;

    [[deprecated]] [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression> final;
    [[nodiscard]] auto Differentiate(const Expression& differentiationVariable) const -> std::unique_ptr<Expression> final;

    EXPRESSION_TYPE(Add)
    EXPRESSION_CATEGORY(Associative | Commutative | BinExp)
};
/// @endcond

/**
 * The Add expression adds two expressions together.
 * The Add expression takes in two parameters of any Oasis type.
 * The Add expression may take in more than two parameters of Real types.
 *
 *
 * @section Parameters
 * @tparam AugendT The type of the expression to add be added to.
 * @tparam AddendT The type of the expression to add to the augend.
 *
 * @section Example Usage:
 * The Add expression can take in a multitude of Oasis classes.
 *
 * @subsection val_val Adding two Real values together:
 * @code
 * Oasis::Add myAdditionSimple {
 *      Oasis::Real{4},
 *      Oasis::Real{4},
 *  };
 *  Oasis::InFixSerializer result;
 *
 *    auto resultant = myAdditionSimple.Simplify();
 *
 *  std::println("Result of addition: {}", resultant->Accept(result).value());
 *  // Will print 8
 * @endcode
 *
 * @subsection val_var Adding a Real value and a Variable together:
 * @code
 * Oasis::Add myAddition {
        Oasis::Variable{"x"},
        Oasis::Real{4}
    };
    //Oasis::InFixSerializer result;

    resultant = myAddition.Simplify();

    std::println("Result of addition: {}", resultant->Accept(result).value());
    // Will print x+4
 * @endcode
 *
 * @subsection var_var Adding two Variables together:
 * @code
 * Oasis::Add myAdditionX {
        Oasis::Variable{"x"},
        Oasis::Variable{"x"}
    };

    resultant = myAdditionX.Simplify();

    std::println("Result of addition: {}", resultant->Accept(result).value());
    // Will print 2*x
 * @endcode
 *
 * @subsection var_var_n if they are not the same variable:
 * @code
 * Oasis::Add myAdditionXY {
        Oasis::Variable{"x"},
        Oasis::Variable{"y"}
    };

    resultant = myAdditionXY.SimplifyVisitor();

    std::println("Result of addition: {}", resultant->Accept(result).value());
    // Will print x+y
 * @endcode
 *
 * @subsubsection multi Adding more than two values together:
 *
 * @code
 *  auto myAdditionSimple = Oasis::Add<> {
        Oasis::Real{4},
        Oasis::Real{4},
        Oasis::Real{4}
    };
    Oasis::InFixSerializer result;

    auto resultant = myAdditionSimple.Simplify();

    std::println("Result of addition: {}", resultant->Accept(result).value());
 * @endcode
 *
 * @subsection exprAdd Adding two expressions together:
 *
 * @code
 *  std::string expr1 = {"2x+3y+15"};
    std::string expr2 = {"5x+9y-10"};

    const auto prepoc1 = Oasis::PreProcessInFix(expr1);
    const auto prepoc2 = Oasis::PreProcessInFix(expr2);

    auto midResult1 = Oasis::FromInFix(prepoc1);
    auto midResult2 = Oasis::FromInFix(prepoc2);

    const std::unique_ptr<Oasis::Expression> expression1 = std::move(midResult1).value();
    const std::unique_ptr<Oasis::Expression> expression2 = std::move(midResult2).value();

    Oasis::Add exprAdd {
        *expression1,
        *expression2
    };
    Oasis::InFixSerializer result;

    auto resultant = exprAdd.Simplify();

    std::println("Result: {}", resultant->Accept(result).value());
    // Will print (((7*x)+(12*y))+5)
 * @endcode
 *
 */
template <typename AugendT = Expression, typename AddendT = AugendT>
class Add : public BinaryExpression<Add, AugendT, AddendT> {
public:
    Add() = default;
    Add(const Add<AugendT, AddendT>& other)
        : BinaryExpression<Add, AugendT, AddendT>(other)
    {
    }

    Add(const AugendT& addend1, const AddendT& addend2)
        : BinaryExpression<Add, AugendT, AddendT>(addend1, addend2)
    {
    }

    auto operator=(const Add& other) -> Add& = default;

    EXPRESSION_TYPE(Add)
    EXPRESSION_CATEGORY(Associative | Commutative)
};

} // namespace Oasis

#endif // OASIS_ADD_HPP

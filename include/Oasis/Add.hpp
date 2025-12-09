//
// Created by Matthew McCall on 7/2/23.
//

#ifndef OASIS_ADD_HPP
#define OASIS_ADD_HPP

#include "BinaryExpression.hpp"
#include "Real.hpp"

namespace Oasis {

template <IExpression AugendT, IExpression AddendT>
class Add;

/// @cond
template <>
class Add<
    Expression, Expression> : public BinaryExpression<Add> {
public:
    using BinaryExpression::BinaryExpression;

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression> final;
    [[nodiscard]] auto Differentiate(const Expression& differentiationVariable) const -> std::unique_ptr<Expression> final;

    EXPRESSION_TYPE(Add)
    EXPRESSION_CATEGORY(Associative | Commutative | BinExp)
};
/// @endcond

/**
 * The Add expression adds two expressions together.
 * The Add expression takes in two and only two objects 
 *
 * 
 * @section Parameters
 * @tparam AugendT The type of the expression to add be added to.
 * @tparam AddendT The type of the expression to add to the augend.
 * 
 * @section Examples
 * 
 * The Add expression can add a multitude of OASIS class types.
 * 
 * @subsection Adding two Real values together.
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
 * @subsection Adding a Real value and a Variable together.
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
 * @subsection Adding two Variables together.
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
 * or if they are not the same variable:
 * @code
 * Oasis::Add myAdditionXY {
        Oasis::Variable{"x"},
        Oasis::Variable{"y"}
    };

    resultant = myAdditionXY.Simplify();

    std::println("Result of addition: {}", resultant->Accept(result).value());
    // Will print x+y
 * @endcode
 */
template <IExpression AugendT = Expression, IExpression AddendT = AugendT>
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

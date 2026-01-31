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
 *
 * @tparam AugendT The type of the expression to add be added to.
 * @tparam AddendT The type of the expression to add to the augend.
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

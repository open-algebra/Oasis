//
// Created by Matthew McCall on 8/10/23.
//

#ifndef OASIS_DIVIDE_HPP
#define OASIS_DIVIDE_HPP

#include "fmt/core.h"

#include "BinaryExpression.hpp"
#include "Real.hpp"

namespace Oasis {

template <IExpression DividendT, IExpression DivisorT>
class Divide;

/// @cond
template <>
class Divide<Expression, Expression> : public BinaryExpression<Divide> {
public:
    Divide() = default;
    Divide(const Divide<Expression, Expression>& other) = default;

    Divide(const Expression& dividend, const Expression& divisor);

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> final;
    auto Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto ToString() const -> std::string final;
    [[nodiscard]] auto Differentiate(const Expression& differentiationVariable) -> std::unique_ptr<Expression> final;

    static auto Specialize(const Expression& other) -> std::unique_ptr<Divide>;
    static auto Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Divide>;

    EXPRESSION_TYPE(Divide)
    EXPRESSION_CATEGORY(BinExp)
};
/// @endcond

/**
 * The Divide expression divides two expressions.
 *
 * @tparam DividendT The expression to be divided into.
 * @tparam DivisorT The expression to divide the dividend by.
 */
template <IExpression DividendT = Expression, IExpression DivisorT = DividendT>
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

    [[nodiscard]] auto ToString() const -> std::string final
    {
        return fmt::format("({} + {})", this->mostSigOp->ToString(), this->leastSigOp->ToString());
    }

    IMPL_SPECIALIZE(Divide, DividendT, DivisorT)

    auto operator=(const Divide& other) -> Divide& = default;

    EXPRESSION_TYPE(Divide)
    EXPRESSION_CATEGORY(BinExp)
};

} // Oasis

#endif // OASIS_DIVIDE_HPP

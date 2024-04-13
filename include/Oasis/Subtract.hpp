//
// Created by Matthew McCall on 8/10/23.
//

#ifndef OASIS_SUBTRACT_HPP
#define OASIS_SUBTRACT_HPP

#include "fmt/core.h"

#include "BinaryExpression.hpp"
#include "Real.hpp"

namespace Oasis {

template <IExpression MinuendT, IExpression SubtrahendT>
class Subtract;

/// @cond
template <>
class Subtract<Expression, Expression> : public BinaryExpression<Subtract> {
public:
    Subtract() = default;
    Subtract(const Subtract<Expression, Expression>& other) = default;

    Subtract(const Expression& minuend, const Expression& subtrahend);

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> final;
    auto Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto ToString() const -> std::string final;
    [[nodiscard]] auto Differentiate(const Expression& differentiationVariable) -> std::unique_ptr<Expression> final;

    static auto Specialize(const Expression& other) -> std::unique_ptr<Subtract>;
    static auto Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Subtract>;

    EXPRESSION_TYPE(Subtract)
    EXPRESSION_CATEGORY(BinExp)
};
/// @endcond

/**
 * The Subtract expression subtracts two expressions.
 *
 * @tparam MinuendT The expression to be subtracted from.
 * @tparam SubtrahendT The expression to subtract from the minuend.
 */
template <IExpression MinuendT = Expression, IExpression SubtrahendT = MinuendT>
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

    [[nodiscard]] auto ToString() const -> std::string final
    {
        return fmt::format("({} - {})", this->mostSigOp->ToString(), this->leastSigOp->ToString());
    }

    IMPL_SPECIALIZE(Subtract, MinuendT, SubtrahendT)

    auto operator=(const Subtract& other) -> Subtract& = default;

    EXPRESSION_TYPE(Subtract)
    EXPRESSION_CATEGORY(BinExp)
};

} // Oasis

#endif // OASIS_SUBTRACT_HPP

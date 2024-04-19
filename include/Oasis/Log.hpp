//
// Created by Matthew McCall on 10/6/23.
//

#ifndef OASIS_LOG_HPP
#define OASIS_LOG_HPP

#include "fmt/core.h"

#include "BinaryExpression.hpp"
#include "Real.hpp"

namespace Oasis {

template <IExpression BaseT, IExpression ArgumentT>
class Log;

/// @cond
template <>
class Log<Expression, Expression> : public BinaryExpression<Log> {
public:
    Log() = default;
    Log(const Log<Expression, Expression>& other) = default;

    Log(const Expression& base, const Expression& argument);

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> final;
    auto Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto ToString() const -> std::string final;

    static auto Specialize(const Expression& other) -> std::unique_ptr<Log>;
    static auto Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Log>;

    EXPRESSION_TYPE(Log)
    EXPRESSION_CATEGORY(BinExp)
};
/// @endcond

/**
 * The Log expression represents the logarithm of a base and an argument.
 *
 * @tparam BaseT The type of the base expression.
 * @tparam ArgumentT The type of the argument expression.
 */
template <IExpression BaseT = Expression, IExpression ArgumentT = BaseT>
class Log : public BinaryExpression<Log, BaseT, ArgumentT> {
public:
    Log() = default;
    Log(const Log<BaseT, ArgumentT>& other)
        : BinaryExpression<Log, BaseT, ArgumentT>(other)
    {
    }

    Log(const BaseT& base, const ArgumentT& argument)
        : BinaryExpression<Log, BaseT, ArgumentT>(base, argument)
    {
    }

    [[nodiscard]] auto ToString() const -> std::string final
    {
        return fmt::format("log({}, {})", this->mostSigOp->ToString(), this->leastSigOp->ToString());
    }

    IMPL_SPECIALIZE(Log, BaseT, ArgumentT);

    auto operator=(const Log& other) -> Log& = default;

    EXPRESSION_TYPE(Log);
    EXPRESSION_CATEGORY(BinExp);
};

} // Oasis

#endif // OASIS_LOG_HPP

//
// Created by Matthew McCall on 10/6/23.
//

#ifndef OASIS_LOG_HPP
#define OASIS_LOG_HPP

#include "BinaryExpression.hpp"
#include "Real.hpp"

namespace Oasis {

/// @cond
template <>
class Log<Expression, Expression> : public BinaryExpression<Log> {
public:
    Log() = default;
    Log(const Log<Expression, Expression>& other) = default;

    Log(const Expression& base, const Expression& argument);

    [[deprecated]] [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression> final;
    [[nodiscard]] auto Differentiate(const Expression& differentiationVariable) const -> std::unique_ptr<Expression> final;

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
template <typename BaseT = Expression, typename ArgumentT = BaseT>
class Log : public BinaryExpression<Log, BaseT, ArgumentT> {
public:
    Log() = default;
    Log(const Log<BaseT, ArgumentT>& other)
        : BinaryExpression<Log, BaseT, ArgumentT>(other)
    {
    }

    Log(const BaseT& base, const ArgumentT& argument)
        : BinaryExpression<Log, BaseT, ArgumentT>(base, argument) { }

        ;

    auto operator=(const Log& other) -> Log& = default;

    EXPRESSION_TYPE(Log);
    EXPRESSION_CATEGORY(BinExp);
};

} // Oasis

#endif // OASIS_LOG_HPP

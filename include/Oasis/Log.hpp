//
// Created by Matthew McCall on 10/6/23.
//

#ifndef OASIS_LOG_HPP
#define OASIS_LOG_HPP

#include "fmt/core.h"

#include "BinaryExpression.hpp"

namespace Oasis {

/**
 * The Log expression represents the logarithm of a base and an argument.
 *
 * @tparam BaseT The type of the base expression.
 * @tparam ArgumentT The type of the argument expression.
 */
template <IExpression BaseT = Expression, IExpression ArgumentT = BaseT>
class Log : public BinaryExpression<Log, BaseT, ArgumentT> {
    Log() = default;
    Log(const Log<BaseT, ArgumentT>& other)
            : BinaryExpression<Log, BaseT, ArgumentT>(other)
    { }

    Log(const BaseT& base, const ArgumentT& argument)
            : BinaryExpression<Log, BaseT, ArgumentT>(base, argument)
    { }

    [[nodiscard]] auto ToString() const -> std::string final
    {
        return fmt::format("log({}, {})", this->GetBase(), this->GetArgument());
    }

    IMPL_SPECIALIZE(Log, BaseT, ArgumentT);

    auto operator=(const Log& other) -> Log& = default;

    EXPRESSION_TYPE(Log);
    EXPRESSION_CATEGORY(None);
};

} // Oasis

#endif // OASIS_LOG_HPP

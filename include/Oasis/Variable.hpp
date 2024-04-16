//
// Created by Matthew McCall on 8/15/23.
//

#ifndef OASIS_VARIABLE_HPP
#define OASIS_VARIABLE_HPP

#include <stdexcept>
#include <string>

#include "LeafExpression.hpp"

namespace Oasis {

/**
 * An algebraic variable.
 *
 * Variables are used to represent unknown values in an expression. Variables
 * can have names such as "x" or "y" or "x_1" and so on.
 */
class Variable : public LeafExpression<Variable> {
public:
    Variable() = default;
    Variable(const Variable& other) = default;

    explicit Variable(std::string name);

    [[nodiscard]] virtual auto Equals(const Expression& other) const -> bool final;

    EXPRESSION_TYPE(Variable)
    EXPRESSION_CATEGORY(UnExp)

    /**
     * Gets the name of the variable.
     *
     * @return The name of the variable.
     */
    [[nodiscard]] auto GetName() const -> std::string;

    [[nodiscard]] auto ToString() const -> std::string final;
    [[nodiscard]] auto Differentiate(const Expression& differentiationVariable) -> std::unique_ptr<Expression> final;

    static auto Specialize(const Expression& other) -> std::unique_ptr<Variable>;
    static auto Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Variable>;

    auto Substitute(const Expression& var, const Expression& val) -> std::unique_ptr<Expression> override;

    auto operator=(const Variable& other) -> Variable& = default;

private:
    std::string name {};
};

} // Oasis

#endif // OASIS_VARIABLE_HPP

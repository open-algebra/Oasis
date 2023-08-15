//
// Created by Matthew McCall on 8/15/23.
//

#ifndef OASIS_VARIABLE_HPP
#define OASIS_VARIABLE_HPP

#include <string>

#include "LeafExpression.hpp"

namespace Oasis {

class Variable : public LeafExpression<Variable> {
public:
    Variable() = default;
    Variable(const Variable& other) = default;

    explicit Variable(std::string name);

    [[nodiscard]] virtual auto Equals(const Expression& other) const -> bool final;

    EXPRESSION_TYPE(Variable)
    EXPRESSION_CATEGORY(0)

    [[nodiscard]] auto GetName() const -> std::string;

    [[nodiscard]] auto ToString() const -> std::string final;

    static auto Specialize(const Expression& other) -> std::unique_ptr<Variable>;
    static auto Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Variable>;

    auto operator=(const Variable& other) -> Variable& = default;

private:
    std::string name {};
};

} // Oasis

#endif // OASIS_VARIABLE_HPP

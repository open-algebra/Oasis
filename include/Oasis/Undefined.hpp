//
// Created by Blake Kessler on 10/13/23.
//

#ifndef OASIS_UNDEFINED_HPP
#define OASIS_UNDEFINED_HPP

#include "LeafExpression.hpp"

namespace Oasis {

/**
 * An undefined expression.
 */
class Undefined : public LeafExpression<Undefined> {
public:
    Undefined() = default;
    Undefined(const Undefined& other) = default;

    [[nodiscard]] auto Equals(const Expression&) const -> bool final { return false; }

    EXPRESSION_TYPE(None)
    EXPRESSION_CATEGORY(UnExp)

    [[nodiscard]] auto ToString() const -> std::string final;

    static auto Specialize(const Expression& other) -> std::unique_ptr<Undefined>;
    static auto Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Undefined>;

    auto operator=(const Undefined& other) -> Undefined& = default;
};

} // Oasis

#endif // OASIS_UNDEFINED_HPP

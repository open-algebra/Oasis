//
// Created by Andrew Nazareth on 6/25/24.
//

#ifndef OASIS_PI_HPP
#define OASIS_PI_HPP

#include "LeafExpression.hpp"

namespace Oasis {

/**
 * Mathematical Constant Pi
 */
class Pi : public LeafExpression<Pi> {
public:
    Pi() = default;
    Pi(const Pi& other) = default;

    [[nodiscard]] auto Equals(const Expression& other) const -> bool final;

    EXPRESSION_TYPE(Pi)
    EXPRESSION_CATEGORY(UnExp)

    static auto Specialize(const Expression& other) -> std::unique_ptr<Pi>;
    static auto GetValue() -> double;
};
}

#endif // OASIS_PI_HPP

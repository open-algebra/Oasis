//
// Created by Andrew Nazareth on 6/25/24.
//

#ifndef OASIS_EULERNUMBER_HPP
#define OASIS_EULERNUMBER_HPP

#include "LeafExpression.hpp"

namespace Oasis {

/**
 * An 'EulerNumber' number.
 */
class EulerNumber : public LeafExpression<EulerNumber> {
public:
    EulerNumber() = default;
    EulerNumber(const EulerNumber& other) = default;

    [[nodiscard]] auto Equals(const Expression& other) const -> bool final;

    EXPRESSION_TYPE(EulerNumber)
    EXPRESSION_CATEGORY(UnExp)

    static auto Specialize(const Expression& other) -> std::unique_ptr<EulerNumber>;
    static auto GetValue() -> double;
};
}

#endif // OASIS_EULERNUMBER_HPP

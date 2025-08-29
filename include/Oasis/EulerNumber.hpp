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

    [[nodiscard]] auto Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto Differentiate(const Expression&) const -> std::unique_ptr<Expression> final;

    EXPRESSION_TYPE(EulerNumber)
    EXPRESSION_CATEGORY(UnExp)

    static auto GetValue() -> double;
};
}

#endif // OASIS_EULERNUMBER_HPP

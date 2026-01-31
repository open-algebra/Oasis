//
// Created by Andrew Nazareth on 9/19/25.
//

#ifndef OASIS_SINE_HPP
#define OASIS_SINE_HPP

#include "UnaryExpression.hpp"

namespace Oasis {

template <typename OperandT>
class Sine;

template <>
class Sine<Expression> final : public UnaryExpression<Sine> {
public:
    Sine() = default;
    Sine(const Sine& other)
        : UnaryExpression<Sine>(other)
    {
    }

    explicit Sine(const Expression& operand)
        : UnaryExpression<Sine>(operand)
    {
    }

    [[deprecated]] [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> override;

    [[nodiscard]] auto Differentiate(const Expression& var) const -> std::unique_ptr<Expression> override;

    [[nodiscard]] auto Integrate(const Expression& var) const -> std::unique_ptr<Expression> override;

    EXPRESSION_TYPE(Sine)
    EXPRESSION_CATEGORY(UnExp)
};

} // Oasis

#endif // OASIS_SINE_HPP

#ifndef OASIS_TAN_HPP
#define OASIS_TAN_HPP

#include "Expression.hpp"
#include "UnaryExpression.hpp"

namespace Oasis {

template <typename OperandT = Expression>
class Tan final : public UnaryExpression<Tan, OperandT> {
public:
    Tan() = default;
    Tan(const Tan& other)
        : UnaryExpression<Tan, OperandT>(other)
    {
    }

    explicit Tan(const OperandT& operand)
        : UnaryExpression<Tan, OperandT>(operand)
    {
    }

    [[deprecated]] [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto Differentiate(const Expression& var) const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto Integrate(const Expression& var) const -> std::unique_ptr<Expression> final;

    EXPRESSION_TYPE(Tan)
    EXPRESSION_CATEGORY(UnExp)
};

} // Oasis

#endif // OASIS_TAN_HPP

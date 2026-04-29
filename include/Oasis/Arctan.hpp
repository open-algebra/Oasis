#ifndef OASIS_ARCTAN_HPP
#define OASIS_ARCTAN_HPP

#include "Expression.hpp"
#include "UnaryExpression.hpp"

namespace Oasis {

template <typename OperandT = Expression>
class Arctan final : public UnaryExpression<Arctan, OperandT> {
public:
    Arctan() = default;
    Arctan(const Arctan& other)
        : UnaryExpression<Arctan, OperandT>(other)
    {
    }

    explicit Arctan(const OperandT& operand)
        : UnaryExpression<Arctan, OperandT>(operand)
    {
    }

    [[nodiscard]] auto Integrate(const Expression& var) const -> std::unique_ptr<Expression> final;

    EXPRESSION_TYPE(Arctan)
    EXPRESSION_CATEGORY(UnExp)
};

} // Oasis

#endif // OASIS_ARCTAN_HPP

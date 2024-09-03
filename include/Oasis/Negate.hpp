//
// Created by Matthew McCall on 3/29/24.
//

#ifndef OASIS_NEGATE_HPP
#define OASIS_NEGATE_HPP

#include "Multiply.hpp"
#include "UnaryExpression.hpp"

namespace Oasis {

template <typename OperandT>
class Negate final : public UnaryExpression<Negate, OperandT> {
public:
    Negate() = default;
    Negate(const Negate& other)
        : UnaryExpression<Negate, OperandT>(other)
    {
    }

    explicit Negate(const OperandT& operand)
        : UnaryExpression<Negate, OperandT>(operand)
    {
    }

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> override
    {
        return Multiply {
            Real { -1.0 },
            this->GetOperand()
        }
            .Simplify();
    }

    [[nodiscard]] auto Differentiate(const Expression& var) const -> std::unique_ptr<Expression> override
    {
        const std::unique_ptr<Expression> operandDerivative = this->GetOperand().Differentiate(var);
        return Negate<Expression> {
            *operandDerivative
        }
            .Simplify();
    }

    IMPL_SPECIALIZE_UNARYEXPR(Negate, OperandT)

    EXPRESSION_TYPE(Negate)
    EXPRESSION_CATEGORY(UnExp)
};

} // Oasis

#endif // OASIS_NEGATE_HPP

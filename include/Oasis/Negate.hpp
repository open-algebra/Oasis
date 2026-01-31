//
// Created by Matthew McCall on 3/29/24.
//

#ifndef OASIS_NEGATE_HPP
#define OASIS_NEGATE_HPP

#include "Multiply.hpp"
#include "UnaryExpression.hpp"

namespace Oasis {

template <typename OperandT = Expression>
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

    [[deprecated]] [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> override
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
            .Generalize(); // TODO: FIX WITH VISITOR
    }

    EXPRESSION_TYPE(Negate)
    EXPRESSION_CATEGORY(UnExp)
};

} // Oasis

#endif // OASIS_NEGATE_HPP

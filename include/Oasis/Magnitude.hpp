//
// Created by Andrew Nazareth on 6/28/24.
//

#ifndef OASIS_MAGNITUDE_HPP
#define OASIS_MAGNITUDE_HPP

#include "UnaryExpression.hpp"
#include "Real.hpp"
#include "memory"

namespace Oasis {

/**
 *
 * @tparam OperandT Type of child operand
 * This represents magnitude/absolute value
 */
template <typename OperandT>
class Magnitude final : public UnaryExpression<Magnitude, OperandT> {
public:
    Magnitude() = default;
    Magnitude(const Magnitude& other)
        : UnaryExpression<Magnitude, OperandT>(other)
    {
    }

    explicit Magnitude(const OperandT& operand)
        : UnaryExpression<Magnitude, OperandT>(operand)
    {
    }

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> override
    {
        // TODO: Implement
        if (auto realCase = Real::Specialize(this->GetOperand()); realCase != nullptr)
        {
            double val = realCase->GetValue();
            return val >= 0.0 ? std::make_unique<Real>(val) : std::make_unique<Real>(-val);
        }

        return this->GetOperand().Generalize();
    }

    auto Simplify(tf::Subflow& /*subflow*/) const -> std::unique_ptr<Expression> override
    {
        // TODO: Implement
        return this->Generalize();
    }

    [[nodiscard]] auto Differentiate(const Expression& var) const -> std::unique_ptr<Expression> override
    {
        // TODO: Implement

        const std::unique_ptr<Expression> operandDerivative = this->GetOperand().Differentiate(var);
        return Magnitude<Expression> {
            *operandDerivative
        }
            .Simplify();
    }

    [[nodiscard]] auto Integrate(const Expression& /*integrationVariable*/) -> std::unique_ptr<Expression> override
    {
        // TODO: Implement

        return this->Generalize();
    }

    IMPL_SPECIALIZE_UNARYEXPR(Magnitude, OperandT)

    EXPRESSION_TYPE(Magnitude)
    EXPRESSION_CATEGORY(UnExp)
};

} // Oasis


#endif // OASIS_MAGNITUDE_HPP

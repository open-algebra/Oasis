//
// Created by Andrew Nazareth on 6/28/24.
//

#ifndef OASIS_MAGNITUDE_HPP
#define OASIS_MAGNITUDE_HPP

#include <memory>

#include "Add.hpp"
#include "Exponent.hpp"
#include "Expression.hpp"
#include "Imaginary.hpp"
#include "Matrix.hpp"
#include "Multiply.hpp"
#include "Oasis/SimplifyVisitor.hpp"
#include "Real.hpp"
#include "RecursiveCast.hpp"
#include "UnaryExpression.hpp"

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

    [[nodiscard]] auto Differentiate(const Expression& var) const -> std::unique_ptr<Expression> override
    {
        // TODO: Implement
        Oasis::SimplifyVisitor simplifyVisitor {};

        const std::unique_ptr<Expression> operandDerivative = this->GetOperand().Differentiate(var);
        return Magnitude<Expression> {
            *operandDerivative
        }
            .Generalize();
    }

    [[nodiscard]] auto Integrate(const Expression& integrationVar) const -> std::unique_ptr<Expression> override
    {
        // TODO: Implement
        const std::unique_ptr<Expression> operandDerivative = this->GetOperand().Integrate(integrationVar);
        return Magnitude<Expression> {
            *operandDerivative
        }
            .Generalize();
    }

    EXPRESSION_TYPE(Magnitude)
    EXPRESSION_CATEGORY(UnExp)
};

} // Oasis

#endif // OASIS_MAGNITUDE_HPP

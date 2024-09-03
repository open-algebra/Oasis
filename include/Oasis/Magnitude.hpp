//
// Created by Andrew Nazareth on 6/28/24.
//

#ifndef OASIS_MAGNITUDE_HPP
#define OASIS_MAGNITUDE_HPP

#include "Add.hpp"
#include "Exponent.hpp"
#include "Expression.hpp"
#include "Imaginary.hpp"
#include "Matrix.hpp"
#include "Multiply.hpp"
#include "Real.hpp"
#include "Subtract.hpp"
#include "UnaryExpression.hpp"
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
        auto simpOp = this->GetOperand().Simplify();
        if (auto realCase = Real::Specialize(*simpOp); realCase != nullptr) {
            double val = realCase->GetValue();
            return val >= 0.0 ? std::make_unique<Real>(val) : std::make_unique<Real>(-val);
        }
        if (auto imgCase = Imaginary::Specialize(*simpOp); imgCase != nullptr) {
            return std::make_unique<Real>(1.0);
        }
        if (auto mulImgCase = Multiply<Expression, Imaginary>::Specialize(*simpOp); mulImgCase != nullptr) {
            return Magnitude<Expression> { mulImgCase->GetMostSigOp() }.Simplify();
        }
        if (auto addCase = Add<Expression, Imaginary>::Specialize(*simpOp); addCase != nullptr) {
            return Exponent { Add<Expression> { Exponent<Expression> { addCase->GetMostSigOp(), Real { 2 } },
                                  Real { 1.0 } },
                Real { 0.5 } }
                .Simplify();
        }
        if (auto addCase = Add<Expression, Multiply<Expression, Imaginary>>::Specialize(*simpOp); addCase != nullptr) {
            return Exponent { Add<Expression> { Exponent<Expression> { addCase->GetMostSigOp(), Real { 2 } },
                                  Exponent<Expression> { addCase->GetLeastSigOp().GetMostSigOp(), Real { 2 } } },
                Real { 0.5 } }
                .Simplify();
        }
        if (auto matrixCase = Matrix::Specialize(*simpOp); matrixCase != nullptr) {
            double sum = 0;
            for (size_t i = 0; i < matrixCase->GetRows(); i++) {
                for (size_t j = 0; j < matrixCase->GetCols(); j++) {
                    sum += pow(matrixCase->GetMatrix()(i, j), 2);
                }
            }
            return Exponent { Real { sum }, Real { 0.5 } }.Simplify();
        }

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

    [[nodiscard]] auto Integrate(const Expression& integrationVar) const -> std::unique_ptr<Expression> override
    {
        // TODO: Implement
        const std::unique_ptr<Expression> operandDerivative = this->GetOperand().Integrate(integrationVar);
        return Magnitude<Expression> {
            *operandDerivative
        }
            .Simplify();
    }

    IMPL_SPECIALIZE_UNARYEXPR(Magnitude, OperandT)

    EXPRESSION_TYPE(Magnitude)
    EXPRESSION_CATEGORY(UnExp)
};

} // Oasis

#endif // OASIS_MAGNITUDE_HPP

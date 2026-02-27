//
// Created by Gregory Lemonnier 2/xx/26.
//

#ifndef OASIS_SECANT_HPP
#define OASIS_SECANT_HPP

#include "Expression.hpp"
#include "UnaryExpression.hpp"

namespace Oasis {

template <typename OperandT = Expression>
class Secant final :public UnaryExpression<Secant, OperandT> {
public:
    Secant() = default;
    Secant(const Secant& other)
        : UnaryExpression<Secant, OperandT>(other)
    {
    }

    explicit Secant(const OperandT& operand)
        : UnaryExpression<Secant, OperandT>(operand)
    {
    }

    [[deprecated]] [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> override 
    { 
        return nullptr;
    };

    [[nodiscard]] auto Differentiate(const Expression& var) const -> std::unique_ptr<Expression> override {
        return nullptr;
     };

    [[nodiscard]] auto Integrate(const Expression& var) const -> std::unique_ptr<Expression> override {
        return nullptr;
     };

    EXPRESSION_TYPE(Secant)
    EXPRESSION_CATEGORY(UnExp)
};

} // Oasis

#endif // OASIS_SECANT_HPP
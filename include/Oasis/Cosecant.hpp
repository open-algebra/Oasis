//
// Created by Gregory Lemonnier 2/27/26.
//

#ifndef OASIS_COSECANT_HPP
#define OASIS_COSECANT_HPP

#include "Expression.hpp"
#include "UnaryExpression.hpp"

namespace Oasis {

template <typename OperandT = Expression>
class Cosecant final :public UnaryExpression<Cosecant, OperandT> {
public:
    Cosecant() = default;
    Cosecant(const Cosecant& other)
        : UnaryExpression<Cosecant, OperandT>(other)
    {
    }

    explicit Cosecant(const OperandT& operand)
        : UnaryExpression<Cosecant, OperandT>(operand)
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

    EXPRESSION_TYPE(Cosecant)
    EXPRESSION_CATEGORY(UnExp)
};

} // Oasis

#endif // OASIS_COSECANT_HPP
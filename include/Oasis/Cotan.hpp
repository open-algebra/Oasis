//
// Created by Gregory Lemonnier 2/xx/26.
//

#ifndef OASIS_COTAN_HPP
#define OASIS_COTAN_HPP

#include "Expression.hpp"
#include "UnaryExpression.hpp"

namespace Oasis {

template <typename OperandT = Expression>
class Cotan final :public UnaryExpression<Cotan, OperandT> {
public:
    Cotan() = default;
    Cotan(const Cotan& other)
        : UnaryExpression<Cotan, OperandT>(other)
    {
    }

    explicit Cotan(const OperandT& operand)
        : UnaryExpression<Cotan, OperandT>(operand)
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

    EXPRESSION_TYPE(Cotan)
    EXPRESSION_CATEGORY(UnExp)
};

} // Oasis

#endif // OASIS_COTAN_HPP
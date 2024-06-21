#ifndef OASIS_ARCSIN_HPP
#define OASIS_ARCSIN_HPP

#include "UnaryExpression.hpp"

namespace Oasis {

class Arcsin : public UnaryExpression {

public: 
    explicit Arcsin(std::unique_ptr<UnaryExpression> operand);

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<UnaryExpression> override;
    [[nodiscard]] auto ToString() const -> std::string override;

    EXPRESSION_TYPE(Arcsin)
    EXPRESSION_CATEGORY(UnExp)
    
}

}

#endif
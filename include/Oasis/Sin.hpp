#ifndef OASIS_SIN_HPP
#define OASIS_SIN_HPP

#include "UnaryExpression.hpp"

namespace Oasis {

class Sin : public UnaryExpression {

public: 
    explicit Sin(std::unique_ptr<UnaryExpression> operand);

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<UnaryExpression> override;
    [[nodiscard]] auto ToString() const -> std::string override;

    EXPRESSION_TYPE(Sin)
    EXPRESSION_CATEGORY(UnExp)
}

}
#endif
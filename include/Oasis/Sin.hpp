#ifndef OASIS_SIN_HPP
#define OASIS_SIN_HPP

#include "UnaryExpression.hpp"

namespace Oasis {

class Sin : public UnaryExpression {
public: 
    explicit Sin(const Expression& operand);

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> override;
    [[nodiscard]] auto Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> override;
    [[nodiscard]] auto ToString() const -> std::string override;
    [[nodiscard]] auto Copy() const -> std::unique_ptr<Expression> override;

    IMPL_SPECIALIZE_UNARYEXPR(Sin, OperandT)

    EXPRESSION_TYPE(Sin)
    EXPRESSION_CATEGORY(UnExp)
};

} // namespace Oasis

#endif // OASIS_SIN_HPP

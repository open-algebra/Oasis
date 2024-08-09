#ifndef OASIS_ARCSIN_HPP
#define OASIS_ARCSIN_HPP

#include "UnaryExpression.hpp"

namespace Oasis {

class Arcsin : public UnaryExpression {
public:
    explicit Arcsin(const Expression& operand);

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> override;
    [[nodiscard]] auto Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> override;
    [[nodiscard]] auto ToString() const -> std::string override;
    [[nodiscard]] auto Copy() const -> std::unique_ptr<Expression> override;

    IMPL_SPECIALIZE_UNARYEXPR(Arcsin, OperandT)

    EXPRESSION_TYPE(Arcsin)
    EXPRESSION_CATEGORY(UnExp)
};

} // namespace Oasis

#endif // OASIS_ARCSIN_HPP

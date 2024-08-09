#ifndef OASIS_ARCCOS_HPP
#define OASIS_ARCCOS_HPP

#include "UnaryExpression.hpp"

namespace Oasis {

class Arccos : public UnaryExpression {
public:
    explicit Arccos(const Expression& operand);

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> override;
    [[nodiscard]] auto Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> override;
    [[nodiscard]] auto ToString() const -> std::string override;
    [[nodiscard]] auto Copy() const -> std::unique_ptr<Expression> override;

    IMPL_SPECIALIZE_UNARYEXPR(Arccos, OperandT)

    EXPRESSION_TYPE(Arccos)
    EXPRESSION_CATEGORY(UnExp)
};

} // namespace Oasis

#endif // OASIS_ARCCOS_HPP

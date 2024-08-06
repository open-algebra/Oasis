#ifndef OASIS_ARCCOS_HPP
#define OASIS_ARCCOS_HPP

#include "UnaryExpression.hpp"

namespace Oasis {

class ArcCos : public UnaryExpression {
public: 
    explicit ArcCos(const Expression& operand);

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> override;
    [[nodiscard]] auto Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> override;
    [[nodiscard]] auto ToString() const -> std::string override;
    [[nodiscard]] auto Copy() const -> std::unique_ptr<Expression> override;

    IMPL_SPECIALIZE_UNARYEXPR(ArcCos, OperandT)

    EXPRESSION_TYPE(ArcCos)
    EXPRESSION_CATEGORY(UnExp)
};

} // namespace Oasis

#endif // OASIS_ARCCOS_HPP

#ifndef OASIS_ARCSIN_HPP
#define OASIS_ARCSIN_HPP

#include "Expression.hpp"

namespace Oasis {

class Arcsin : public Expression {
public:
    explicit Arcsin(std::unique_ptr<Expression> operand);

    [[nodiscard]] auto Copy() const -> std::unique_ptr<Expression> override;
    [[nodiscard]] auto Copy(tf::Subflow& subflow) const -> std::unique_ptr<Expression> override;
    [[nodiscard]] auto Equals(const Expression& other) const -> bool override;
    [[nodiscard]] auto StructurallyEquivalent(const Expression& other) const -> bool override;
    [[nodiscard]] auto StructurallyEquivalent(const Expression& other, tf::Subflow& subflow) const -> bool override;
    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> override;
    [[nodiscard]] auto Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> override;
    [[nodiscard]] auto ToString() const -> std::string override;

    EXPRESSION_TYPE(Arcsin)

private:
    std::unique_ptr<Expression> operand;
};

} // namespace Oasis

#endif // OASIS_ARCSIN_HPP

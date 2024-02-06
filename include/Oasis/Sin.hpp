#ifndef OASIS_SIN_HPP
#define OASIS_SIN_HPP

#include "Expression.hpp"

namespace Oasis {

class Sin : public Expression {
public:
    explicit Sin(std::unique_ptr<Expression> operand);

    [[nodiscard]] auto Copy() const -> std::unique_ptr<Expression> override;
    [[nodiscard]] auto Copy(tf::Subflow& subflow) const -> std::unique_ptr<Expression> override;
    [[nodiscard]] auto Equals(const Expression& other) const -> bool override;
    [[nodiscard]] auto StructurallyEquivalent(const Expression& other) const -> bool override;
    [[nodiscard]] auto StructurallyEquivalent(const Expression& other, tf::Subflow& subflow) const -> bool override;
    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> override;
    [[nodiscard]] auto Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> override;
    [[nodiscard]] auto ToString() const -> std::string override;

    EXPRESSION_TYPE(Sin)

private:
    std::unique_ptr<Expression> operand;
};

} // namespace Oasis

#endif // OASIS_SIN_HPP

#ifndef OASIS_POWER_HPP
#define OASIS_POWER_HPP

#include "Expression.hpp"
#include <memory>

namespace Oasis {

class Power : public Expression {
public:
    Power(std::unique_ptr<Expression> base, std::unique_ptr<Expression> exponent);

    [[nodiscard]] auto Copy() const -> std::unique_ptr<Expression> override;
    [[nodiscard]] auto Copy(tf::Subflow& subflow) const -> std::unique_ptr<Expression> override;
    [[nodiscard]] auto Equals(const Expression& other) const -> bool override;
    [[nodiscard]] auto StructurallyEquivalent(const Expression& other) const -> bool override;
    [[nodiscard]] auto StructurallyEquivalent(const Expression& other, tf::Subflow& subflow) const -> bool override;
    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> override;
    [[nodiscard]] auto Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> override;
    [[nodiscard]] auto ToString() const -> std::string override;

    EXPRESSION_TYPE(Power)

private:
    std::unique_ptr<Expression> base;
    std::unique_ptr<Expression> exponent;
};

} // namespace Oasis

#endif // OASIS_POWER_HPP

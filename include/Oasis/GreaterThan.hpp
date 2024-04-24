#ifndef OASIS_GREATER_THAN_HPP
#define OASIS_GREATER_THAN_HPP

#include "Expression.hpp"
#include <memory>

namespace Oasis {

class GreaterThan : public Expression {
public:
    GreaterThan(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right);
    [[nodiscard]] auto Copy() const -> std::unique_ptr<Expression> override;
    [[nodiscard]] auto Equals(const Expression& other) const -> bool override;
    [[nodiscard]] auto StructurallyEquivalent(const Expression& other) const -> bool override;
    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> override;
    [[nodiscard]] auto ToString() const -> std::string override;
    [[nodiscard]] const Expression* GetLeftPtr() const;
    [[nodiscard]] const Expression* GetRightPtr() const;
    auto GetType() const -> ExpressionType override;

private:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
};

auto GreaterThan::GetType() const -> ExpressionType {
    return ExpressionType::GreaterThan;
}

} // namespace Oasis

#endif // OASIS_GREATER_THAN_HPP
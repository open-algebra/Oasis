#ifndef OASIS_COSH_HPP
#define OASIS_COSH_HPP

#include "Expression.hpp"
#include <memory>

namespace Oasis {

class Cosh : public Expression {
public:
    explicit Cosh(std::unique_ptr<Expression> operand);

    [[nodiscard]] auto Copy() const -> std::unique_ptr<Expression> override;
    [[nodiscard]] auto Equals(const Expression& other) const -> bool override;
    [[nodiscard]] auto StructurallyEquivalent(const Expression& other) const -> bool override;
    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> override;
    [[nodiscard]] auto Derivative() const -> std::unique_ptr<Expression>;
    [[nodiscard]] auto ToString() const -> std::string override;
    [[nodiscard]] const Expression* GetOperandPtr() const;

    auto GetType() const -> ExpressionType override;

private:
    std::unique_ptr<Expression> operand;
};

auto Cosh::GetType() const -> ExpressionType {
    return ExpressionType::Cosh;
}

} // namespace Oasis

#endif // OASIS_COSH_HPP

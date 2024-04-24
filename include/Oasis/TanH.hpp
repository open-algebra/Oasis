#ifndef OASIS_TANH_HPP
#define OASIS_TANH_HPP

#include "Expression.hpp"
#include <memory>

namespace Oasis {

class Tanh : public Expression {
public:
    explicit Tanh(std::unique_ptr<Expression> operand);
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

auto Tanh::GetType() const -> ExpressionType {
    return ExpressionType::Tanh;
}

} // namespace Oasis

#endif // OASIS_TANH_HPP
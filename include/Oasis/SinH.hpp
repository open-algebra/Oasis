#ifndef OASIS_SINH_HPP
#define OASIS_SINH_HPP

#include "Expression.hpp"
#include <memory>

namespace Oasis {

class Sinh : public Expression {
public:
    explicit Sinh(std::unique_ptr<Expression> operand);

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

auto Sinh::GetType() const -> ExpressionType {
    return ExpressionType::Sinh;
}

} // namespace Oasis

#endif // OASIS_SINH_HPP

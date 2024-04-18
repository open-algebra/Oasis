#ifndef OASIS_SIN_HPP
#define OASIS_SIN_HPP

#include "Expression.hpp"

namespace Oasis {

class Sin : public Expression {
public:
    explicit Sin(std::unique_ptr<Expression> operand);

    [[nodiscard]] auto Copy() const -> std::unique_ptr<Expression> override;
    [[nodiscard]] auto Equals(const Expression& other) const -> bool override;
    [[nodiscard]] auto StructurallyEquivalent(const Expression& other) const -> bool override;
    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> override;
    [[nodiscard]] auto ToString() const -> std::string override;
    [[nodiscard]] const Expression* GetOperandPtr() const;
    [[nodiscard]] auto Derivative() const -> std::unique_ptr<Expression>;
 
    auto GetType() const -> ExpressionType override;

private:
    std::unique_ptr<Expression> operand;
};

auto Sin::GetType() const -> ExpressionType {
    return ExpressionType::Sin;
}

} // namespace Oasis

#endif // OASIS_SIN_HPP

#ifndef OASIS_TAN_HPP
#define OASIS_TAN_HPP

#include "Expression.hpp"
#include <memory>

namespace Oasis {

class Tan : public Expression {
public:
    explicit Tan(std::unique_ptr<Expression> operand);

    [[nodiscard]] auto Copy() const -> std::unique_ptr<Expression> override;
    [[nodiscard]] auto Equals(const Expression& other) const -> bool override;
    [[nodiscard]] auto StructurallyEquivalent(const Expression& other) const -> bool override;
    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> override;
    [[nodiscard]] auto ToString() const -> std::string override;

    [[nodiscard]] const Expression* GetOperandPtr() const;

    auto GetType() const -> ExpressionType override;

private:
    std::unique_ptr<Expression> operand;
};

} // namespace Oasis

#endif // OASIS_TAN_HPP

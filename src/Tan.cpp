#include "Tan.hpp"


namespace Oasis {

Tan::Tan(std::unique_ptr<Expression> operand) : operand(std::move(operand)) {}

auto Tan::Copy() const -> std::unique_ptr<Expression> {
    return std::make_unique<Tan>(operand->Copy());
}

auto Tan::Equals(const Expression& other) const -> bool {
    if (const auto* o = dynamic_cast<const Tan*>(&other)) {
        return operand->Equals(*o->operand);
    }
    return false;
}

auto Tan::StructurallyEquivalent(const Expression& other) const -> bool {
    if (const auto* o = dynamic_cast<const Tan*>(&other)) {
        return operand->StructurallyEquivalent(*o->operand);
    }
    return false;
}

std::unique_ptr<Expression> Tan::Simplify() const {
    auto simplifiedOperand = operand->Simplify();
    // Placeholder for specific Tan simplification logic, if applicable.
    return std::make_unique<Tan>(std::move(simplifiedOperand));
}

auto Tan::ToString() const -> std::string {
    return "tan(" + operand->ToString() + ")";
}

const Expression* Tan::GetOperandPtr() const {
    return operand.get();
}

auto Tan::GetType() const -> ExpressionType {
    return ExpressionType::Tan;
}

} // namespace Oasis

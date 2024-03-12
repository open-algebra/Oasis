#include "sin.hpp"
#include <cmath> // Only if you need to compute actual sin values, possibly for simplification or evaluation.
#include <memory> // For std::unique_ptr
#include <utility> // For std::move


namespace Oasis {

Sin::Sin(std::unique_ptr<Expression> operand) : operand(std::move(operand)) {}

auto Sin::Copy() const -> std::unique_ptr<Expression> {
    return std::make_unique<Sin>(operand->Copy());
}


auto Sin::Equals(const Expression& other) const -> bool {
    if (auto o = dynamic_cast<const Sin*>(&other)) {
        return operand->Equals(*o->operand);
    }
    return false;
}

auto Sin::StructurallyEquivalent(const Expression& other) const -> bool {
    if (auto o = dynamic_cast<const Sin*>(&other)) {
        return operand->StructurallyEquivalent(*o->operand);
    }
    return false;
}


std::unique_ptr<Expression> Sin::Simplify() const {
    // Attempt to simplify the operand.
    auto simplifiedOperand = operand->Simplify();
    return std::make_unique<Sin>(std::move(simplifiedOperand));
}


auto Sin::ToString() const -> std::string {
    // Convert the expression to a string representation.
    return "sin(" + operand->ToString() + ")";
}

const Expression* Sin::GetOperandPtr() const {
    return operand.get();
}


} // namespace Oasis

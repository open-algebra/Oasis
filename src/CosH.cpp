#include "Cosh.hpp"
#include "Sinh.hpp"
#include <cmath> // For std::cosh

namespace Oasis {

Cosh::Cosh(std::unique_ptr<Expression> operand)
    : operand(std::move(operand)) {}

auto Cosh::Copy() const -> std::unique_ptr<Expression> {
    return std::make_unique<Cosh>(operand->Copy());
}

auto Cosh::Equals(const Expression& other) const -> bool {
    if (auto o = dynamic_cast<const Cosh*>(&other)) {
        return operand->Equals(*o->operand);
    }
    return false;
}

auto Cosh::StructurallyEquivalent(const Expression& other) const -> bool {
    if (auto o = dynamic_cast<const Cosh*>(&other)) {
        return operand->StructurallyEquivalent(*o->operand);
    }
    return false;
}

std::unique_ptr<Expression> Cosh::Simplify() const {
    // might consider checking if the operand is a constant and simplifying accordingly
    return std::make_unique<Cosh>(operand->Simplify());
}

std::unique_ptr<Expression> Cosh::Derivative() const {
    return std::make_unique<Sinh>(operand->Copy());
}

auto Cosh::ToString() const -> std::string {
    return "cosh(" + operand->ToString() + ")";
}

const Expression* Cosh::GetOperandPtr() const {
    return operand.get();
}

} // namespace Oasis
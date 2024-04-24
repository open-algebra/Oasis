#include "Sinh.hpp"
#include <cmath> // For std::sinh

namespace Oasis {

Sinh::Sinh(std::unique_ptr<Expression> operand)
    : operand(std::move(operand)) {}

auto Sinh::Copy() const -> std::unique_ptr<Expression> {
    return std::make_unique<Sinh>(operand->Copy());
}

auto Sinh::Equals(const Expression& other) const -> bool {
    if (auto o = dynamic_cast<const Sinh*>(&other)) {
        return operand->Equals(*o->operand);
    }
    return false;
}

auto Sinh::StructurallyEquivalent(const Expression& other) const -> bool {
    if (auto o = dynamic_cast<const Sinh*>(&other)) {
        return operand->StructurallyEquivalent(*o->operand);
    }
    return false;
}

std::unique_ptr<Expression> Sinh::Simplify() const {
    //might consider checking if the operand is a constant and simplifying accordingly
    return std::make_unique<Sinh>(operand->Simplify());
}

std::unique_ptr<Expression> Sinh::Derivative() const {
    return std::make_unique<Cosh>(operand->Copy());
}

auto Sinh::ToString() const -> std::string {
    return "sinh(" + operand->ToString() + ")";
}

const Expression* Sinh::GetOperandPtr() const {
    return operand.get();
}

} // namespace Oasis

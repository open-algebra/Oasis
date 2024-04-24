#include "TanH.hpp"
#include <cmath> 

namespace Oasis {

Tanh::Tanh(std::unique_ptr<Expression> operand)
    : operand(std::move(operand)) {}

auto Tanh::Copy() const -> std::unique_ptr<Expression> {
    return std::make_unique<Tanh>(operand->Copy());
}

auto Tanh::Equals(const Expression& other) const -> bool {
    if (auto o = dynamic_cast<const Tanh*>(&other)) {
        return operand->Equals(*o->operand);
    }
    return false;
}

auto Tanh::StructurallyEquivalent(const Expression& other) const -> bool {
    if (auto o = dynamic_cast<const Tanh*>(&other)) {
        return operand->StructurallyEquivalent(*o->operand);
    }
    return false;
}

std::unique_ptr<Expression> Tanh::Simplify() const {
    // might consider checking if the operand is a constant and simplifying accordingly
    return std::make_unique<Tanh>(operand->Simplify());
}

std::unique_ptr<Expression> Tanh::Derivative() const {
    // derivative of tanh(x) is 1 - tanh^2(x)
    // may need to adjust this based on your specific Expression classes and operators
    return std::make_unique<Tanh>(operand->Copy());
}

auto Tanh::ToString() const -> std::string {
    return "tanh(" + operand->ToString() + ")";
}

const Expression* Tanh::GetOperandPtr() const {
    return operand.get();
}

} // namespace Oasis
#include "Arcsin.hpp"
#include "Sin.hpp"
#include "Sin.cpp"
#include <cmath>  
#include <memory> 
#include <utility> 

namespace Oasis {

Arcsin::Arcsin(std::unique_ptr<Expression> operand) : operand(std::move(operand)) {}

auto Arcsin::Copy() const -> std::unique_ptr<Expression> {
    return std::make_unique<Arcsin>(operand->Copy());
}

auto Arcsin::Equals(const Expression& other) const -> bool {
    if (const auto* o = dynamic_cast<const Arcsin*>(&other)) {
        return operand->Equals(*o->operand);
    }
    return false;
}

auto Arcsin::StructurallyEquivalent(const Expression& other) const -> bool {
    if (const auto* o = dynamic_cast<const Arcsin*>(&other)) {
        return operand->StructurallyEquivalent(*o->operand);
    }
    return false;
}

std::unique_ptr<Expression> Arcsin::Simplify() const {
    auto simplifiedOperand = operand->Simplify();
    if (const auto* sinOperand = dynamic_cast<const Sin*>(simplifiedOperand.get())) {
        return sinOperand->GetOperandPtr()->Copy();
    }
    return std::make_unique<Arcsin>(std::move(simplifiedOperand));
}



auto Arcsin::ToString() const -> std::string {
    return "arcsin(" + operand->ToString() + ")";
}

auto Arcsin::GetType() const -> ExpressionType {
    return ExpressionType::Arcsin; 
}

} // namespace Oasis

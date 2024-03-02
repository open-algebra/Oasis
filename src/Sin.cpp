#include "sin.hpp"
#include <cmath> // Only if you need to compute actual sin values, possibly for simplification or evaluation.
#include <memory> // For std::unique_ptr
#include <utility> // For std::move

namespace Oasis {

Sin::Sin(std::unique_ptr<Expression> operand) : operand(std::move(operand)) {}

auto Sin::Copy() const -> std::unique_ptr<Expression> {
    return std::make_unique<Sin>(operand->Copy());
}

auto Sin::Copy(tf::Subflow& subflow) const -> std::unique_ptr<Expression> {
    return Copy(); 
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

auto Sin::StructurallyEquivalent(const Expression& other, tf::Subflow& subflow) const -> bool {
    // Implement parallel structural equivalence if needed, otherwise just call the regular StructurallyEquivalent.
    return StructurallyEquivalent(other);
}

auto Sin::Simplify() const -> std::unique_ptr<Expression> {
    // Simplification logic here. For now, just return a copy.
    return Copy();
}

auto Sin::Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> {
    // Implement parallel simplification if needed, otherwise just call the regular Simplify.
    return Simplify(); 
}

auto Sin::ToString() const -> std::string {
    // Convert the expression to a string representation.
    return "sin(" + operand->ToString() + ")";
}

EXPRESSION_TYPE(Sin) // Implement this macro as needed, likely setting some static type ID for runtime checks.

} // namespace Oasis

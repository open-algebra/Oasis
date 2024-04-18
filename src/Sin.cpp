#include "Sin.hpp"
#include <cmath> // Only if you need to compute actual sin values, possibly for simplification or evaluation.
#include <memory> // For std::unique_ptr
#include <utility> // For std::move
#include "MathUtils.hpp" // For MathUtils::isNumericConstant, MathUtils::toNumeric, MathUtils::createNumericExpression
// uncomment after merge -- #include "Cos.hpp" 

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
    auto simplifiedOperand = operand->Simplify();

    // Checking if the operand is a numeric constant
    if (MathUtils::isNumericConstant(simplifiedOperand)) {
        double value = MathUtils::toNumeric(simplifiedOperand);
        
        // Simplifying sin(0) = 0
        if (MathUtils::equals(value, 0)) {
            return MathUtils::createNumericExpression(0);
        }
        // Simplifying sin(π) = 0
        if (MathUtils::equals(value, M_PI)) {
            return MathUtils::createNumericExpression(0);
        }
        // Simplifying sin(π/2) = 1
        if (MathUtils::equals(value, M_PI / 2)) {
            return MathUtils::createNumericExpression(1);
        }
        // Simplifying sin(3π/2) = -1
        if (MathUtils::equals(value, 3 * M_PI / 2)) {
            return MathUtils::createNumericExpression(-1);
        }
    }

    return std::make_unique<Sin>(std::move(simplifiedOperand));
}


auto Sin::ToString() const -> std::string {
    // Convert the expression to a string representation.
    return "sin(" + operand->ToString() + ")";
}

const Expression* Sin::GetOperandPtr() const {
    return operand.get();
}


std::unique_ptr<Expression> Sin::Derivative() const {
    //return std::make_unique<Cos>(operand->Copy()); // uncomment after merge.
}

} // namespace Oasis

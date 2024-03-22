#include "Tan.hpp"
#include "MathUtils.hpp"

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
    if (MathUtils::isNumericConstant(simplifiedOperand)) {
        double value = MathUtils::toNumeric(simplifiedOperand);
        if (MathUtils::equals(value, 0)) {
            return MathUtils::createNumericExpression(0);
        }
        if (MathUtils::equals(value, MathUtils::PI / 4)) {
            return MathUtils::createNumericExpression(1);
        }
    }
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

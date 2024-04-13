#include "Oasis/Power.hpp"
#include "Oasis/MathUtils.hpp"

namespace Oasis {

Power::Power(std::unique_ptr<Expression> base, std::unique_ptr<Expression> exponent)
    : base(std::move(base)), exponent(std::move(exponent)) {}

auto Power::Copy() const -> std::unique_ptr<Expression> {
    return std::make_unique<Power>(base->Copy(), exponent->Copy());
}

auto Power::Equals(const Expression& other) const -> bool {
    if (auto o = dynamic_cast<const Power*>(&other)) {
        return base->Equals(*o->base) && exponent->Equals(*o->exponent);
    }
    return false;
}

auto Power::StructurallyEquivalent(const Expression& other) const -> bool {
    if (auto o = dynamic_cast<const Power*>(&other)) {
        return base->StructurallyEquivalent(*o->base) && exponent->StructurallyEquivalent(*o->exponent);
    }
    return false;
}

std::unique_ptr<Expression> Power::Simplify() const {
    auto simplifiedBase = base->Simplify();
    auto simplifiedExponent = exponent->Simplify();
    if (MathUtils::isNumericConstant(simplifiedExponent) && MathUtils::toNumeric(simplifiedExponent) == 0) {
        return MathUtils::createNumericExpression(1);  // x^0 = 1
    }
    if (MathUtils::isNumericConstant(simplifiedExponent) && MathUtils::toNumeric(simplifiedExponent) == 1) {
        return simplifiedBase;  // x^1 = x
    }
    return std::make_unique<Power>(std::move(simplifiedBase), std::move(simplifiedExponent));
}

auto Power::ToString() const -> std::string {
    return "(" + base->ToString() + ")^(" + exponent->ToString() + ")";
}

} // namespace Oasis

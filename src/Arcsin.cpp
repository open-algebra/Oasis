#include "Arcsin.hpp"
#include "Real.hpp"
#include <cmath>

namespace Oasis {

Arcsin::Arcsin(const Expression& operand) : UnaryExpression(operand) {}

auto Arcsin::Simplify() const -> std::unique_ptr<Expression> {
    auto simplifiedOperand = GetOperand().Simplify();
    Arcsin simplifiedArcsin { *simplifiedOperand };

    if (auto realOperand = Real::Specialize(*simplifiedOperand); realOperand != nullptr) {
        return std::make_unique<Real>(std::asin(realOperand->GetValue()));
    }

    return simplifiedArcsin.Copy();
}

auto Arcsin::Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> {
    std::unique_ptr<Expression> simplifiedOperand;
    subflow.emplace([this, &simplifiedOperand](tf::Subflow& sbf) {
        simplifiedOperand = GetOperand().Simplify(sbf);
    }).join();

    if (auto realOperand = Real::Specialize(*simplifiedOperand)) {
        return std::make_unique<Real>(std::asin(realOperand->GetValue()));
    }

    return std::make_unique<Arcsin>(*simplifiedOperand);
}

auto Arcsin::ToString() const -> std::string {
    return "arcsin(" + GetOperand().ToString() + ")";
}

auto Arcsin::Copy() const -> std::unique_ptr<Expression> {
    return std::make_unique<Arcsin>(*this);
}

} // namespace Oasis

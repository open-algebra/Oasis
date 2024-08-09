#include "Oasis/Arccos.hpp"
#include "Oasis/Real.hpp"
#include <cmath>

namespace Oasis {

Arccos::Arccos(const Expression& operand) : UnaryExpression(operand) {}

auto Arccos::Simplify() const -> std::unique_ptr<Expression> {
    auto simplifiedOperand = GetOperand().Simplify();
    Arccos simplifiedArccos { *simplifiedOperand };

    if (auto realOperand = Real::Specialize(*simplifiedOperand); realOperand != nullptr) {
        return std::make_unique<Real>(std::acos(realOperand->GetValue()));
    }

    return simplifiedArccos.Copy();
}

auto Arccos::Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> {
    std::unique_ptr<Expression> simplifiedOperand;
    subflow.emplace([this, &simplifiedOperand](tf::Subflow& sbf) {
        simplifiedOperand = GetOperand().Simplify(sbf);
    }).join();

    if (auto realOperand = Real::Specialize(*simplifiedOperand)) {
        return std::make_unique<Real>(std::acos(realOperand->GetValue()));
    }

    return std::make_unique<Arccos>(*simplifiedOperand);
}

auto Arccos::ToString() const -> std::string {
    return "arccos(" + GetOperand().ToString() + ")";
}

auto Arccos::Copy() const -> std::unique_ptr<Expression> {
    return std::make_unique<Arccos>(*this);
}

} // namespace Oasis

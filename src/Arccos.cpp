#include "Oasis/ArcCos.hpp"
#include "Oasis/Real.hpp"
#include <cmath>

namespace Oasis {

ArcCos::ArcCos(const Expression& operand) : UnaryExpression(operand) {}

auto ArcCos::Simplify() const -> std::unique_ptr<Expression> {
    auto simplifiedOperand = GetOperand().Simplify();
    if (auto realOperand = Real::Specialize(*simplifiedOperand)) {
        return std::make_unique<Real>(std::acos(realOperand->GetValue()));
    }
    return std::make_unique<ArcCos>(*simplifiedOperand);
}

auto ArcCos::Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> {
    std::unique_ptr<Expression> simplifiedOperand;
    subflow.emplace([this, &simplifiedOperand](tf::Subflow& sbf) {
        simplifiedOperand = GetOperand().Simplify(sbf);
    }).join();

    if (auto realOperand = Real::Specialize(*simplifiedOperand)) {
        return std::make_unique<Real>(std::acos(realOperand->GetValue()));
    }
    return std::make_unique<ArcCos>(*simplifiedOperand);
}

auto ArcCos::ToString() const -> std::string {
    return "arccos(" + GetOperand().ToString() + ")";
}

auto ArcCos::Copy() const -> std::unique_ptr<Expression> {
    return std::make_unique<ArcCos>(*this);
}

} // namespace Oasis

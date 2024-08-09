#include "Cos.hpp"
#include "Real.hpp"
#include <cmath>

namespace Oasis {

Cos::Cos(const Expression& operand) : UnaryExpression(operand) {}

auto Cos::Simplify() const -> std::unique_ptr<Expression> {
    auto simplifiedOperand = GetOperand().Simplify();
    if (auto realOperand = Real::Specialize(*simplifiedOperand)) {
        return std::make_unique<Real>(std::cos(realOperand->GetValue()));
    }
    return std::make_unique<Cos>(*simplifiedOperand);
}

auto Cos::Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> {
    std::unique_ptr<Expression> simplifiedOperand;
    subflow.emplace([this, &simplifiedOperand](tf::Subflow& sbf) {
        simplifiedOperand = GetOperand().Simplify(sbf);
    }).join();

    if (auto realOperand = Real::Specialize(*simplifiedOperand)) {
        return std::make_unique<Real>(std::cos(realOperand->GetValue()));
    }
    return std::make_unique<Cos>(*simplifiedOperand);
}

auto Cos::ToString() const -> std::string {
    return "cos(" + GetOperand().ToString() + ")";
}

auto Cos::Copy() const -> std::unique_ptr<Expression> {
    return std::make_unique<Cos>(*this);
}

} // namespace Oasis

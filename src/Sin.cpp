#include "Sin.hpp"
#include "Real.hpp"
#include <cmath>

namespace Oasis {

Sin::Sin(const Expression& operand) : UnaryExpression(operand) {}

auto Sin::Simplify() const -> std::unique_ptr<Expression> {
    auto simplifiedOperand = GetOperand().Simplify();
    Sin simplifiedSin{*simplifiedOperand};

    if (auto realOperand = Real::Specialize(*simplifiedOperand); realOperand != nullptr) {
        double value = realOperand->GetValue();
        if (value == 0.0) {
            return std::make_unique<Real>(0.0);
        } else if (value == M_PI / 2) {
            return std::make_unique<Real>(1.0);
        } else if (value == M_PI) {
            return std::make_unique<Real>(0.0);
        } else if (value == 3 * M_PI / 2) {
            return std::make_unique<Real>(-1.0);
        } else if (value == 2 * M_PI) {
            return std::make_unique<Real>(0.0);
        }
        return std::make_unique<Real>(sin(value));
    }

    return simplifiedSin.Copy();
}

auto Sin::Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> {
    std::unique_ptr<Expression> simplifiedOperand;
    subflow.emplace([this, &simplifiedOperand](tf::Subflow& sbf) {
        simplifiedOperand = GetOperand().Simplify(sbf);
    }).join();

    Sin simplifiedSin{*simplifiedOperand};
    return simplifiedSin.Copy();
}

std::string Sin::ToString() const {
    return "sin(" + GetOperand().ToString() + ")";
}

auto Sin::Copy() const -> std::unique_ptr<Expression> {
    return std::make_unique<Sin>(*this);
}

} // namespace Oasis

#include "Arcsin.hpp"
#include "Real.hpp"
#include <cmath>

namespace Oasis {
    Arcsin::Arcsin(const Expression& operand) : UnaryExpression(operand) {}

    auto Arcsin::Simplify() const -> std::unique_ptr<Expression> {
        auto simplifiedOperand = GetOperand().Simplify();
        Arcsin simplifiedArcsin { *simplifiedOperand };

        if (auto realOperand = Real::Specialize(*simplifiedOperand); realOperand != nullptr) {
            double value = realOperand->GetValue();
            if (value == 0.0) {
                return std::make_unique<Real>(0.0);
            } else if (value == 1.0) {
                return std::make_unique<Real>(M_PI / 2);
            } else if (value == -1.0) {
                return std::make_unique<Real>(-M_PI / 2);
            }
            return std::make_unique<Real>(asin(value));
        }

        return simplifiedArcsin.Copy();
    }
    

    auto Arcsin::Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> {
    
    }

    std::string Arcsin::ToString() const {
        return ("arcsin(" + GetOperand().ToString() + ")");
    }

    auto Arcsin::Copy() const -> std::unique_ptr<Expression> {
        return std::make_unique<Arcsin>(*this);
    }
} // namespace Oasis

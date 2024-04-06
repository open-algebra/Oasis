#include "Tan.hpp"
#include "MathUtils.hpp"
#include "Add.hpp"
#include "Power.hpp"

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
            // Handling tan(0) = 0
            if (MathUtils::equals(value, 0)) {
                return MathUtils::createNumericExpression(0);
            } 
            // Handling tan(π/4) = 1
            if (MathUtils::equals(value, MathUtils::PI / 4)) {
                return MathUtils::createNumericExpression(1);
            }
            // Handling tan(π/2) = Undefined
            if (MathUtils::equals(value, MathUtils::PI / 2)) {
                return nullptr; 
            }
        }
        
        return std::make_unique<Tan>(std::move(simplifiedOperand));
    }


    std::unique_ptr<Expression> Tan::Derivative() const {
    // Represents the constant 1
    auto one = MathUtils::createNumericExpression(1);
    // Represents tan(x)^2, which is the operand of this Tan instance raised to the power of 2
    auto tanSquared = std::make_unique<Power>(
        operand->Copy(), // Copy the operand for use in the new expression
        MathUtils::createNumericExpression(2) // The exponent 2
    );
    // Adds 1 and tan^2(x) together to form the derivative
    auto derivative = std::make_unique<Add>(
        std::move(one),
        std::move(tanSquared)
    );

    return derivative;
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




//
// Created by Matthew McCall on 3/29/24.
//

#ifndef UNARYEXPRESSION_HPP
#define UNARYEXPRESSION_HPP

#include "Expression.hpp"
#include "Visit.hpp"

namespace Oasis {

template <template <IExpression> class DerivedT, IExpression OperandT = Expression>
class UnaryExpression : public Expression {

    using DerivedSpecialized = DerivedT<OperandT>;
    using DerivedGeneralized = DerivedT<Expression>;

public:
    UnaryExpression() = default;

    UnaryExpression(const UnaryExpression& other)
        : Expression(other)
    {
        if (other.HasOperand()) {
            SetOperand(other.GetOperand());
        }
    }

    explicit UnaryExpression(const OperandT& operand)
    {
        SetOperand(operand);
    }

    [[nodiscard]] auto Copy() const -> std::unique_ptr<Expression> final
    {
        return std::make_unique<DerivedSpecialized>(*static_cast<const DerivedSpecialized*>(this));
    }

    [[nodiscard]] auto Equals(const Expression& other) const -> bool final
    {
        if (!other.Is<DerivedSpecialized>()) {
            return false;
        }

        // generalize
        const auto otherGeneralized = other.Generalize();
        const auto& otherUnaryGeneralized = dynamic_cast<const DerivedGeneralized&>(*otherGeneralized);

        return op->Equals(otherUnaryGeneralized.GetOperand());
    }

    [[nodiscard]] auto Generalize() const -> std::unique_ptr<Expression> final
    {
        DerivedGeneralized generalized;
        if (this->op) {
            generalized.SetOperand(*this->op->Copy());
        }
        return std::make_unique<DerivedGeneralized>(generalized);
    }

    auto GetOperand() const -> const OperandT&
    {
        return *op;
    }

    auto HasOperand() const -> bool
    {
        return op != nullptr;
    }

    [[nodiscard]] auto StructurallyEquivalent(const Expression& other) const -> bool final
    {
        return this->GetType() == other.GetType();
    }

    auto SetOperand(const OperandT& operand) -> void
    {
        if constexpr (std::same_as<OperandT, Expression>) {
            this->op = operand.Copy();
        } else {
            this->op = std::make_unique<OperandT>(operand);
        }
    }

    auto Substitute(const Expression& var, const Expression& val) -> std::unique_ptr<Expression> override
    {
        std::unique_ptr<Expression> right = ((GetOperand().Copy())->Substitute(var, val));
        DerivedT<Expression> comb = DerivedT<Expression> { *right };
        //        auto ret = comb.Accept();
        return comb.Generalize();
    }

    auto AcceptInternal(Visitor& visitor) const -> any override
    {
        const auto generalized = Generalize();
        const auto& derivedGeneralized = dynamic_cast<const DerivedGeneralized&>(*generalized);
        return visitor.Visit(derivedGeneralized);
    }

protected:
    std::unique_ptr<OperandT> op;
};

} // Oasis

#endif // UNARYEXPRESSION_HPP

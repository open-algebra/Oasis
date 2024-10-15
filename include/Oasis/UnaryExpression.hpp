//
// Created by Matthew McCall on 3/29/24.
//

#ifndef UNARYEXPRESSION_HPP
#define UNARYEXPRESSION_HPP

#include "Expression.hpp"
#include "Serialization.hpp"

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

        if (this->HasOperand()) {
            generalized.SetOperand(*this->op->Copy());
        }

        return std::make_unique<DerivedGeneralized>(generalized);
    }

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> override
    {
        return Generalize()->Simplify();
    }

    [[nodiscard]] auto Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression> override
    {
        return Generalize()->Integrate(integrationVariable);
    }

    [[nodiscard]] auto Differentiate(const Expression& differentiationVariable) const -> std::unique_ptr<Expression> override
    {
        return Generalize()->Differentiate(differentiationVariable);
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
        auto ret = comb.Simplify();
        return ret;
    }

    auto operator=(const UnaryExpression& other) -> UnaryExpression& = default;

    void Serialize(SerializationVisitor& visitor) const override
    {
        const auto generalized = Generalize();
        const auto& derivedGeneralized = dynamic_cast<const DerivedGeneralized&>(*generalized);
        visitor.Serialize(derivedGeneralized);
    }

protected:
    std::unique_ptr<OperandT> op;
};

#define IMPL_SPECIALIZE_UNARYEXPR(DerivedT, OperandT)                                           \
    static auto Specialize(const Expression& other) -> std::unique_ptr<DerivedT<OperandT>>      \
    {                                                                                           \
        if (!other.Is<DerivedT>()) {                                                            \
            return nullptr;                                                                     \
        }                                                                                       \
                                                                                                \
        auto specialized = std::make_unique<DerivedT<OperandT>>();                              \
        std::unique_ptr<Expression> otherGeneralized = other.Generalize();                      \
        const auto& otherUnary = dynamic_cast<const DerivedT<Expression>&>(*otherGeneralized);  \
                                                                                                \
        if (auto operand = OperandT::Specialize(otherUnary.GetOperand()); operand != nullptr) { \
            specialized->op = std::move(operand);                                               \
            return specialized;                                                                 \
        }                                                                                       \
                                                                                                \
        return nullptr;                                                                         \
    }

} // Oasis

#endif // UNARYEXPRESSION_HPP

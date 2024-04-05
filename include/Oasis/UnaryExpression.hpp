//
// Created by Matthew McCall on 3/29/24.
//

#ifndef UNARYEXPRESSION_HPP
#define UNARYEXPRESSION_HPP

#include "Expression.hpp"

namespace Oasis {

template <template <IExpression> class DerivedT, IExpression OperandT>
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

    auto Copy(tf::Subflow&) const -> std::unique_ptr<Expression> final
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
        return std::make_unique<DerivedGeneralized>(*this);
    }

    auto Generalize(tf::Subflow& subflow) const -> std::unique_ptr<Expression> final
    {
        return DerivedGeneralized { *this }.Copy(subflow);
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

    auto StructurallyEquivalent(const Expression& other, tf::Subflow&) const -> bool final
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

protected:
    std::unique_ptr<OperandT> op;
};

#define IMPL_SPECIALIZE_UNARYEXPR(DerivedT, OperandT)                                           \
    static auto Specialize(const Expression& other) -> std::unique_ptr<DerivedT>                \
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
    }                                                                                           \
                                                                                                \
    static auto Specialize(const Expression& other, tf::Subflow&) -> std::unique_ptr<DerivedT>  \
    {                                                                                           \
        /* TODO: Actually implement */                                                          \
        return DerivedT<OperandT>::Specialize(other);                                           \
    }

} // Oasis

#endif // UNARYEXPRESSION_HPP

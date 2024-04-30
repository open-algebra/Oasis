//
// Created by Matthew McCall on 4/30/24.
//

#ifndef OASIS_TERNARYEXPRESSION_HPP
#define OASIS_TERNARYEXPRESSION_HPP

#include <cassert>

#include "Expression.hpp"
#include "Serialization.hpp"

namespace Oasis {

template <template <IExpression, IExpression, IExpression> class DerivedT, IExpression OperandT, IExpression LowerBoundT = Expression, IExpression UpperBoundT = LowerBoundT>
class TernaryExpression : public Expression {

    using DerivedSpecialized = DerivedT<OperandT, LowerBoundT, UpperBoundT>;
    using DerivedGeneralized = DerivedT<Expression, Expression, Expression>;

public:
    TernaryExpression() = default;

    TernaryExpression(const TernaryExpression& other)
    {
        if (other.HasOperand())
            SetOperand(other.GetOperand());
        if (other.HasLowerBound())
            SetLowerBound(other.GetLowerBound());
        if (other.HasUpperBound())
            SetUpperBound(other.GetUpperBound());
    }

    TernaryExpression(const OperandT& operand, const LowerBoundT& lowerBound, const UpperBoundT& upperBound)
    {
        SetOperand(operand);
        SetLowerBound(lowerBound);
        SetUpperBound(upperBound);
    }

    [[nodiscard]] auto Copy() const -> std::unique_ptr<Expression> final
    {
        return std::make_unique<DerivedSpecialized>(*static_cast<const DerivedSpecialized*>(this));
    }

    auto Copy(tf::Subflow&) const -> std::unique_ptr<Expression> final
    {
        // TODO: Actually implement
        return std::make_unique<DerivedSpecialized>(*static_cast<const DerivedSpecialized*>(this));
    }

    [[nodiscard]] auto Differentiate(const Expression& differentiationVariable) const -> std::unique_ptr<Expression> override
    {
        return Generalize()->Differentiate(differentiationVariable);
    }

    [[nodiscard]] auto Equals(const Expression& other) const -> bool final
    {

        if (const auto otherExpression = DerivedSpecialized::Specialize(other); otherExpression != nullptr) {
            return HasOperand() == otherExpression->HasOperand()
                && HasLowerBound() == otherExpression->HasLowerBound()
                && HasUpperBound() == otherExpression->HasUpperBound();
        }

        return false;
    }

    [[nodiscard]] auto HasOperand() const -> bool
    {
        return operand != nullptr;
    }

    [[nodiscard]] auto GetOperand() const -> const OperandT&
    {
        assert(HasOperand() && "Operand is not set.");
        return *operand;
    }

    [[nodiscard]] auto HasLowerBound() const -> bool
    {
        return lowerBound != nullptr;
    }

    [[nodiscard]] auto GetLowerBound() const -> const LowerBoundT&
    {
        assert(HasLowerBound() && "LowerBound is not set.");
        return *lowerBound;
    }

    [[nodiscard]] auto HasUpperBound() const -> bool
    {
        return upperBound != nullptr;
    }

    [[nodiscard]] auto GetUpperBound() const -> const UpperBoundT&
    {
        assert(HasUpperBound() && "UpperBound is not set.");
        return *upperBound;
    }

    template <typename T>
        requires IsAnyOf<T, OperandT, Expression>
    void SetOperand(const T& expr)
    {
        if constexpr (std::is_same_v<T, OperandT>) {
            operand = std::make_unique<OperandT>(expr);
        } else {
            operand = expr.Copy();
        }
    }

    template <typename T>
        requires IsAnyOf<T, LowerBoundT, Expression>
    void SetLowerBound(const T& expr)
    {
        if constexpr (std::is_same_v<T, LowerBoundT>) {
            lowerBound = std::make_unique<LowerBoundT>(expr);
        } else {
            lowerBound = expr.Copy();
        }
    }

    template <typename T>
        requires IsAnyOf<T, UpperBoundT, Expression>
    void SetUpperBound(const T& expr)
    {
        if constexpr (std::is_same_v<T, UpperBoundT>) {
            upperBound = std::make_unique<UpperBoundT>(expr);
        } else {
            upperBound = expr.Copy();
        }
    }

    auto Substitute(const Expression& var, const Expression& val) -> std::unique_ptr<Expression> override
    {
        std::unique_ptr<Expression> newOperand = operand->Substitute(var, val);
        std::unique_ptr<Expression> newLowerBound = lowerBound->Substitute(var, val);
        std::unique_ptr<Expression> newUpperBound = upperBound->Substitute(var, val);
        DerivedGeneralized substituted = DerivedGeneralized { operand, newLowerBound, newUpperBound };
        return substituted.Copy();
    }

    auto operator=(const TernaryExpression& other) -> TernaryExpression& = default;

    void Serialize(SerializationVisitor& visitor) const override
    {
        const auto generalized = Generalize();
        const auto& derivedGeneralized = dynamic_cast<const DerivedGeneralized&>(*generalized);
        visitor.Serialize(derivedGeneralized);
    }

private:
    std::unique_ptr<OperandT> operand;
    std::unique_ptr<LowerBoundT> lowerBound;
    std::unique_ptr<UpperBoundT> upperBound;
};

}

#endif // OASIS_TERNARYEXPRESSION_HPP

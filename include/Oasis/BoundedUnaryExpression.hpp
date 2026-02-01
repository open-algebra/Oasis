//
// Created by Matthew McCall on 4/30/24.
//

#ifndef OASIS_BOUNDEDUNARYEXPRESSION_HPP
#define OASIS_BOUNDEDUNARYEXPRESSION_HPP

#include <cassert>

#include "Bounds.hpp"
#include "Expression.hpp"
#include "Visit.hpp"

namespace Oasis {

/**
 * A concept for an operand of a unary expression with bounds.
 * @note This class is not intended to be used directly by end users.
 *
 * @section Parameters
 * @tparam MostSigOpT The type of the most significant operand.
 * @tparam LeastSigOpT The type of the least significant operand.
 * @tparam T The type to check.
 */
template <template <IExpression, IExpression, IExpression> class DerivedT, IExpression OperandT, IExpression LowerBoundT = Expression, IExpression UpperBoundT = LowerBoundT>
class BoundedUnaryExpression : public BoundedExpression<LowerBoundT, UpperBoundT> {

    using DerivedSpecialized = DerivedT<OperandT, LowerBoundT, UpperBoundT>;
    using DerivedGeneralized = DerivedT<Expression, Expression, Expression>;

public:
    BoundedUnaryExpression() = default;

    BoundedUnaryExpression(const BoundedUnaryExpression& other)
    {
        if (other.HasOperand())
            SetOperand(other.GetOperand());
        if (other.HasLowerBound())
            SetLowerBound(other.GetLowerBound());
        if (other.HasUpperBound())
            SetUpperBound(other.GetUpperBound());
    }

    BoundedUnaryExpression(const OperandT& operand, const LowerBoundT& lowerBound, const UpperBoundT& upperBound)
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
        return this->Generalize()->Differentiate(differentiationVariable);
    }

    [[nodiscard]] auto Equals(const Expression& other) const -> bool final
    {

        // TODO: Reimplement now that Specialize is gone
        //        if (const auto otherExpression = DerivedSpecialized::Specialize(other); otherExpression != nullptr) {
        //            return HasOperand() == otherExpression->HasOperand()
        //                && this->HasLowerBound() == otherExpression->HasLowerBound()
        //                && this->HasUpperBound() == otherExpression->HasUpperBound();
        //        }

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

    auto Substitute(const Expression& var, const Expression& val) -> std::unique_ptr<Expression> override
    {
        std::unique_ptr<Expression> newOperand = operand->Substitute(var, val);
        std::unique_ptr<Expression> newLowerBound = this->GetLowerBound().Substitute(var, val);
        std::unique_ptr<Expression> newUpperBound = this->GetUpperBound().Substitute(var, val);
        DerivedGeneralized substituted = DerivedGeneralized { operand, newLowerBound, newUpperBound };
        return substituted.Copy();
    }

    auto operator=(const BoundedUnaryExpression& other) -> BoundedUnaryExpression& = default;

    void Serialize(SerializationVisitor& visitor) const override
    {
        const auto generalized = this->Generalize();
        const auto& derivedGeneralized = dynamic_cast<const DerivedGeneralized&>(*generalized);
        visitor.Serialize(derivedGeneralized);
    }

private:
    std::unique_ptr<OperandT> operand;
};

}

#endif // OASIS_BOUNDEDUNARYEXPRESSION_HPP

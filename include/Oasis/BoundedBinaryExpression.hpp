//
// Created by Matthew McCall on 5/2/24.
//

#ifndef OASIS_BOUNDEDBINARYEXPRESSION_HPP
#define OASIS_BOUNDEDBINARYEXPRESSION_HPP

#include "BoundedExpression.hpp"
#include "Expression.hpp"
#include "RecursiveCast.hpp"
#include "Visit.hpp"

namespace Oasis {
/**
 * A concept for an operand of a binary expression with bounds.
 * @note This class is not intended to be used directly by end users.
 *
 * @section Parameters
 * @tparam MostSigOpT The type of the most significant operand.
 * @tparam LeastSigOpT The type of the least significant operand.
 * @tparam LowerBoundT The lower bound of the binary expression.
 * @tparam UpperBoundT The upper bound of the binary expression.
 */
template <template <IExpression, IExpression, IExpression, IExpression> class DerivedT, IExpression MostSigOpT = Expression, IExpression LeastSigOpT = MostSigOpT, IExpression LowerBoundT = Expression, IExpression UpperBoundT = LowerBoundT>
class BoundedBinaryExpression : public BoundedExpression<LowerBoundT, UpperBoundT> {

    using DerivedSpecialized = DerivedT<MostSigOpT, LeastSigOpT, LowerBoundT, UpperBoundT>;
    using DerivedGeneralized = DerivedT<Expression, Expression, Expression, Expression>;

public:
    BoundedBinaryExpression() = default;
    BoundedBinaryExpression(const BoundedBinaryExpression& other)
    {
        if (other.HasMostSigOp()) {
            SetMostSigOp(other.GetMostSigOp());
        }

        if (other.HasLeastSigOp()) {
            SetLeastSigOp(other.GetLeastSigOp());
        }
    }

    BoundedBinaryExpression(const MostSigOpT& mostSigOp, const LeastSigOpT& leastSigOp)
    {
        SetMostSigOp(mostSigOp);
        SetLeastSigOp(leastSigOp);
    }

    [[nodiscard]] auto Copy() const -> std::unique_ptr<Expression> final
    {
        return std::make_unique<DerivedSpecialized>(*static_cast<const DerivedSpecialized*>(this));
    }

    auto Copy(tf::Subflow& subflow) const -> std::unique_ptr<Expression> final
    {
        DerivedSpecialized copy;

        if (this->mostSigOp) {
            subflow.emplace([this, &copy](tf::Subflow& sbf) {
                copy.SetMostSigOp(mostSigOp->Copy(sbf), sbf);
            });
        }

        if (this->leastSigOp) {
            subflow.emplace([this, &copy](tf::Subflow& sbf) {
                copy.SetLeastSigOp(leastSigOp->Copy(sbf), sbf);
            });
        }

        subflow.join();

        return std::make_unique<DerivedSpecialized>(copy);
    }
    [[nodiscard]] auto Differentiate(const Expression& differentiationVariable) const -> std::unique_ptr<Expression> override
    {
        return Generalize()->Differentiate(differentiationVariable);
    }
    [[nodiscard]] auto Equals(const Expression& other) const -> bool final
    {
        if (this->GetType() != other.GetType()) {
            return false;
        }

        const auto otherGeneralized = other.Generalize();
        const auto& otherBinaryGeneralized = static_cast<const DerivedGeneralized&>(*otherGeneralized);

        bool mostSigOpMismatch = false, leastSigOpMismatch = false;

        if (this->HasMostSigOp() == otherBinaryGeneralized.HasMostSigOp()) {
            if (mostSigOp && otherBinaryGeneralized.HasMostSigOp()) {
                mostSigOpMismatch = !mostSigOp->Equals(otherBinaryGeneralized.GetMostSigOp());
            }
        } else {
            mostSigOpMismatch = true;
        }

        if (this->HasLeastSigOp() == otherBinaryGeneralized.HasLeastSigOp()) {
            if (this->HasLeastSigOp() && otherBinaryGeneralized.HasLeastSigOp()) {
                leastSigOpMismatch = !leastSigOp->Equals(otherBinaryGeneralized.GetLeastSigOp());
            }
        } else {
            mostSigOpMismatch = true;
        }

        if (!mostSigOpMismatch && !leastSigOpMismatch) {
            return true;
        }

        if (!(this->GetCategory() & Associative)) {
            return false;
        }

        auto thisFlattened = std::vector<std::unique_ptr<Expression>> {};
        auto otherFlattened = std::vector<std::unique_ptr<Expression>> {};

        this->Flatten(thisFlattened);
        otherBinaryGeneralized.Flatten(otherFlattened);

        for (const auto& thisOperand : thisFlattened) {
            if (std::find_if(otherFlattened.begin(), otherFlattened.end(), [&thisOperand](const auto& otherOperand) {
                    return thisOperand->Equals(*otherOperand);
                })
                == otherFlattened.end()) {
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] auto Generalize() const -> std::unique_ptr<Expression> final
    {
        DerivedGeneralized generalized;

        if (this->mostSigOp) {
            generalized.SetMostSigOp(*this->mostSigOp->Copy());
        }

        if (this->leastSigOp) {
            generalized.SetLeastSigOp(*this->leastSigOp->Copy());
        }

        return std::make_unique<DerivedGeneralized>(generalized);
    }

    auto Generalize(tf::Subflow& subflow) const -> std::unique_ptr<Expression> final
    {
        DerivedGeneralized generalized;

        if (this->mostSigOp) {
            subflow.emplace([this, &generalized](tf::Subflow& sbf) {
                generalized.SetMostSigOp(*this->mostSigOp->Copy(sbf));
            });
        }

        if (this->leastSigOp) {
            subflow.emplace([this, &generalized](tf::Subflow& sbf) {
                generalized.SetLeastSigOp(*this->leastSigOp->Copy(sbf));
            });
        }

        subflow.join();

        return std::make_unique<DerivedGeneralized>(generalized);
    }

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> override
    {
        return Generalize()->Simplify();
    }

    auto Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> override
    {
        std::unique_ptr<Expression> generalized, simplified;

        tf::Task generalizeTask = subflow.emplace([this, &generalized](tf::Subflow& sbf) {
            generalized = Generalize(sbf);
        });

        tf::Task simplifyTask = subflow.emplace([&generalized, &simplified](tf::Subflow& sbf) {
            simplified = generalized->Simplify(sbf);
        });

        simplifyTask.succeed(generalizeTask);
        subflow.join();

        return simplified;
    }

    [[nodiscard]] auto StructurallyEquivalent(const Expression& other) const -> bool final
    {
        if (this->GetType() != other.GetType()) {
            return false;
        }

        const std::unique_ptr<Expression> otherGeneralized = other.Generalize();
        const auto& otherBinaryGeneralized = static_cast<const DerivedGeneralized&>(*otherGeneralized);

        if (this->HasMostSigOp() == otherBinaryGeneralized.HasMostSigOp()) {
            if (this->HasMostSigOp() && otherBinaryGeneralized.HasMostSigOp()) {
                if (!mostSigOp->StructurallyEquivalent(otherBinaryGeneralized.GetMostSigOp())) {
                    return false;
                }
            }
        }

        if (this->HasLeastSigOp() == otherBinaryGeneralized.HasLeastSigOp()) {
            if (this->HasLeastSigOp() && otherBinaryGeneralized.HasLeastSigOp()) {
                if (!leastSigOp->StructurallyEquivalent(otherBinaryGeneralized.GetLeastSigOp())) {
                    return false;
                }
            }
        }

        return true;
    }

    auto StructurallyEquivalent(const Expression& other, tf::Subflow& subflow) const -> bool final
    {
        if (this->GetType() != other.GetType()) {
            return false;
        }

        std::unique_ptr<Expression> otherGeneralized;

        tf::Task generalizeTask = subflow.emplace([&](tf::Subflow& sbf) {
            otherGeneralized = other.Generalize(sbf);
        });

        bool mostSigOpEquivalent = false, leastSigOpEquivalent = false;

        if (this->mostSigOp) {
            tf::Task compMostSigOp = subflow.emplace([this, &otherGeneralized, &mostSigOpEquivalent](tf::Subflow& sbf) {
                if (const auto& otherBinary = static_cast<const DerivedGeneralized&>(*otherGeneralized); otherBinary.HasMostSigOp()) {
                    mostSigOpEquivalent = mostSigOp->StructurallyEquivalent(otherBinary.GetMostSigOp(), sbf);
                }
            });

            compMostSigOp.succeed(generalizeTask);
        }

        if (this->leastSigOp) {
            tf::Task compLeastSigOp = subflow.emplace([this, &otherGeneralized, &leastSigOpEquivalent](tf::Subflow& sbf) {
                if (const auto& otherBinary = static_cast<const DerivedGeneralized&>(*otherGeneralized); otherBinary.HasLeastSigOp()) {
                    leastSigOpEquivalent = leastSigOp->StructurallyEquivalent(otherBinary.GetLeastSigOp(), sbf);
                }
            });

            compLeastSigOp.succeed(generalizeTask);
        }

        subflow.join();

        return mostSigOpEquivalent && leastSigOpEquivalent;
    }

    /**
     * Flattens this expression.
     *
     * Flattening an expression means that all operands of the expression are copied into a vector.
     * For example, flattening the expression `Add { Add { Real { 1.0 }, Real { 2.0 } }, Real { 3.0 } }`
     * would result in a vector containing three `Real` expressions, `Real { 1.0 }`, `Real { 2.0 }`, and
     * `Real { 3.0 }`. This is useful for simplifying expressions, as it allows the simplifier to
     * operate on the operands of the expression without having to worry about the structure of the
     * expression.
     * @param out The vector to copy the operands into.
     */
    auto Flatten(std::vector<std::unique_ptr<Expression>>& out) const -> void
    {
        if (this->mostSigOp->template Is<DerivedT>()) {
            auto generalizedMostSigOp = this->mostSigOp->Generalize();
            const auto& mostSigOp = static_cast<const DerivedGeneralized&>(*generalizedMostSigOp);
            mostSigOp.Flatten(out);
        } else {
            out.push_back(this->mostSigOp->Copy());
        }

        if (this->leastSigOp->template Is<DerivedT>()) {
            auto generalizedLeastSigOp = this->leastSigOp->Generalize();
            const auto& leastSigOp = static_cast<const DerivedGeneralized&>(*generalizedLeastSigOp);
            leastSigOp.Flatten(out);
        } else {
            out.push_back(this->leastSigOp->Copy());
        }
    }

    /**
     * Gets the most significant operand of this expression.
     * @return The most significant operand of this expression.
     */
    auto GetMostSigOp() const -> const MostSigOpT&
    {
        assert(mostSigOp != nullptr);
        return *mostSigOp;
    }

    /**
     * Gets the least significant operand of this expression.
     * @return The least significant operand of this expression.
     */
    auto GetLeastSigOp() const -> const LeastSigOpT&
    {
        assert(leastSigOp != nullptr);
        return *leastSigOp;
    }

    /**
     * Gets whether this expression has a most significant operand.
     * @return True if this expression has a most significant operand, false otherwise.
     */
    [[nodiscard]] auto HasMostSigOp() const -> bool
    {
        return mostSigOp != nullptr;
    }

    /**
     * Gets whether this expression has a least significant operand.
     * @return True if this expression has a least significant operand, false otherwise.
     */
    [[nodiscard]] auto HasLeastSigOp() const -> bool
    {
        return leastSigOp != nullptr;
    }

    /**
     * \brief Sets the most significant operand of this expression.
     * \param op The operand to set.
     */
    template <typename T>
        requires IsAnyOf<T, MostSigOpT, Expression>
    auto SetMostSigOp(const T& op) -> bool
    {
        if constexpr (std::same_as<MostSigOpT, Expression>) {
            this->mostSigOp = op.Copy();
            return true;
        }

        if constexpr (std::same_as<MostSigOpT, T> && !std::same_as<MostSigOpT, Expression>) {
            this->mostSigOp = std::make_unique<MostSigOpT>(op);
            return true;
        }

        if (auto castedOp = Oasis::RecursiveCast<MostSigOpT>(op); castedOp) {
            mostSigOp = std::move(castedOp);
            return true;
        }

        return false;
    }

    /**
     * Sets the least significant operand of this expression.
     * @param op The operand to set.
     */
    template <typename T>
        requires IsAnyOf<T, LeastSigOpT, Expression>
    auto SetLeastSigOp(const T& op) -> bool
    {
        if constexpr (std::same_as<LeastSigOpT, Expression>) {
            this->leastSigOp = op.Copy();
            return true;
        }

        if constexpr (std::same_as<LeastSigOpT, T> && !std::same_as<LeastSigOpT, Expression>) {
            this->leastSigOp = std::make_unique<LeastSigOpT>(op);
            return true;
        }

        if (auto castedOp = Oasis::RecursiveCast<LeastSigOpT>(op); castedOp) {
            leastSigOp = std::move(castedOp);
            return true;
        }

        return false;
    }

    auto Substitute(const Expression& var, const Expression& val) -> std::unique_ptr<Expression> override
    {
        const std::unique_ptr<Expression> left = GetMostSigOp().Substitute(var, val);
        const std::unique_ptr<Expression> right = GetLeastSigOp().Substitute(var, val);
        DerivedGeneralized comb { *left, *right };
        auto ret = comb.Simplify();
        return ret;
    }
    /**
     * Swaps the operands of this expression.
     * @return A new expression with the operands swapped.
     */
    auto SwapOperands() const -> DerivedSpecialized
    {
        return DerivedT { *this->leastSigOp, *this->mostSigOp };
    }

    auto operator=(const BoundedBinaryExpression& other) -> BoundedBinaryExpression& = default;

    void Serialize(SerializationVisitor& visitor) const override
    {
        const auto generalized = Generalize();
        const auto& derivedGeneralized = dynamic_cast<const DerivedGeneralized&>(*generalized);
        visitor.Serialize(derivedGeneralized);
    }

private:
    std::unique_ptr<MostSigOpT> mostSigOp;
    std::unique_ptr<LeastSigOpT> leastSigOp;
};

}

#endif // OASIS_BOUNDEDBINARYEXPRESSION_HPP

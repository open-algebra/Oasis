//
// Created by Matthew McCall on 7/2/23.
//

#ifndef OASIS_BINARYEXPRESSION_HPP
#define OASIS_BINARYEXPRESSION_HPP

#include "taskflow/taskflow.hpp"

#include "Expression.hpp"

namespace Oasis {

template <IExpression MostSigOpT, IExpression LeastSigOpT>
class BinaryExpressionBase;

/// @cond
template <>
class BinaryExpressionBase<Expression, Expression> : public Expression {
public:
    BinaryExpressionBase() = default;
    BinaryExpressionBase(const BinaryExpressionBase& other);

    BinaryExpressionBase(const Expression& mostSigOp, const Expression& leastSigOp);

    [[nodiscard]] auto Equals(const Expression& other) const -> bool final;

    [[nodiscard]] auto StructurallyEquivalent(const Expression& other) const -> bool override;
    auto StructurallyEquivalent(const Expression& other, tf::Subflow& subflow) const -> bool override;

    auto AddOperand(const std::unique_ptr<Expression>& operand) -> bool;

    auto SetMostSigOp(const Expression& op) -> void;
    auto SetLeastSigOp(const Expression& op) -> void;

    [[nodiscard]] auto HasMostSigOp() const -> bool;
    [[nodiscard]] auto HasLeastSigOp() const -> bool;

    [[nodiscard]] auto GetMostSigOp() const -> const Expression&;
    [[nodiscard]] auto GetLeastSigOp() const -> const Expression&;

    auto operator=(const BinaryExpressionBase& other) -> BinaryExpressionBase&;

protected:
    std::unique_ptr<Expression> mostSigOp;
    std::unique_ptr<Expression> leastSigOp;
};
/// @endcond

/**
 * A concept for an operand of a binary expression.
 * @tparam MostSigOpT The type of the most significant operand.
 * @tparam LeastSigOpT The type of the least significant operand.
 * @tparam T The type to check.
 */
template <typename MostSigOpT, typename LeastSigOpT, typename T>
concept IOperand = std::is_same_v<T, MostSigOpT> || std::is_same_v<T, LeastSigOpT>;

template <template <typename, typename> typename T>
concept IAssociativeAndCommutative = IExpression<T<Expression, Expression>> && requires { (T<Expression, Expression>::GetStaticCategory() & (Associative | Commutative)) != 0; };

/**
 * The base class for all binary expressions.
 *
 * The BinaryExpressionBase class is a base class for all binary expressions. It provides a common
 * interface for all binary expressions, and implements common functionality. Specifically, it provides
 * functionality not dependent on the Derived class.
 *
 * @note This class is not intended to be used directly by end users.
 *
 * @tparam MostSigOpT The type of the most significant operand.
 * @tparam LeastSigOpT The type of the least significant operand.
 */
template <IExpression MostSigOpT = Expression, IExpression LeastSigOpT = MostSigOpT>
class BinaryExpressionBase : public Expression {
public:
    BinaryExpressionBase() = default;
    BinaryExpressionBase(const BinaryExpressionBase<MostSigOpT, LeastSigOpT>& other)
    {
        if (other.mostSigOp) {
            SetMostSigOp(*other.mostSigOp);
        }

        if (other.leastSigOp) {
            SetLeastSigOp(*other.leastSigOp);
        }
    }

    BinaryExpressionBase(const MostSigOpT& mostSigOp, const LeastSigOpT& leastSigOp)
        : mostSigOp(std::make_unique<MostSigOpT>(mostSigOp))
        , leastSigOp(std::make_unique<LeastSigOpT>(leastSigOp))
    {
    }

    [[nodiscard]] auto Equals(const Expression& other) const -> bool final
    {
        if (this->GetType() != other.GetType()) {
            return false;
        }

        const auto& otherBinary = static_cast<const BinaryExpressionBase&>(other);

        if ((mostSigOp == nullptr) == (otherBinary.mostSigOp == nullptr)) {
            if (mostSigOp && otherBinary.mostSigOp) {
                if (!mostSigOp->Equals(*otherBinary.mostSigOp)) {
                    return false;
                }
            }
        } else {
            return false;
        }

        if ((leastSigOp == nullptr) == (otherBinary.leastSigOp == nullptr)) {
            if (leastSigOp && otherBinary.leastSigOp) {
                if (!leastSigOp->Equals(*otherBinary.leastSigOp)) {
                    return false;
                }
            }
        } else {
            return false;
        }

        return true;
    }

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> final
    {
        return Generalize()->Simplify();
    }

    auto Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> final
    {
        std::unique_ptr<Expression> normalized, simplified;

        tf::Task normalizeTask = subflow.emplace([this, &normalized](tf::Subflow& sbf) {
            normalized = Generalize(sbf);
        });

        tf::Task simplifyTask = subflow.emplace([&normalized, &simplified](tf::Subflow& sbf) {
            simplified = normalized->Simplify(sbf);
        });

        simplifyTask.succeed(normalizeTask);
        subflow.join();

        return simplified;
    }

    [[nodiscard]] auto StructurallyEquivalent(const Expression& other) const -> bool override
    {
        if (this->GetType() != other.GetType()) {
            return false;
        }

        std::unique_ptr<Expression> otherGeneralized = other.Generalize();
        const auto& otherBinary = static_cast<const BinaryExpressionBase&>(*otherGeneralized);

        if ((mostSigOp == nullptr) == (otherBinary.mostSigOp == nullptr)) {
            if (mostSigOp && otherBinary.mostSigOp) {
                if (!mostSigOp->StructurallyEquivalent(*otherBinary.mostSigOp)) {
                    return false;
                }
            }
        }

        if ((leastSigOp == nullptr) == (otherBinary.leastSigOp == nullptr)) {
            if (leastSigOp && otherBinary.leastSigOp) {
                if (!leastSigOp->StructurallyEquivalent(*otherBinary.leastSigOp)) {
                    return false;
                }
            }
        }

        return true;
    }

    auto StructurallyEquivalent(const Expression& other, tf::Subflow& subflow) const -> bool override
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
                const auto& otherBinary = dynamic_cast<const BinaryExpressionBase<Expression>&>(*otherGeneralized);
                if (otherBinary.HasMostSigOp()) {
                    mostSigOpEquivalent = mostSigOp->StructurallyEquivalent(otherBinary.GetMostSigOp(), sbf);
                }
            });

            compMostSigOp.succeed(generalizeTask);
        }

        if (this->leastSigOp) {
            tf::Task compLeastSigOp = subflow.emplace([this, &otherGeneralized, &leastSigOpEquivalent](tf::Subflow& sbf) {
                const auto& otherBinary = dynamic_cast<const BinaryExpressionBase<Expression>&>(*otherGeneralized);
                if (otherBinary.HasLeastSigOp()) {
                    leastSigOpEquivalent = leastSigOp->StructurallyEquivalent(otherBinary.GetLeastSigOp(), sbf);
                }
            });

            compLeastSigOp.succeed(generalizeTask);
        }

        subflow.join();

        return mostSigOpEquivalent && leastSigOpEquivalent;
    }

    /**
     * Adds an operand to this expression.
     *
     * If this expression already has two operands, this function returns false. If this expression
     * does not have its most significant operance set, this function sets it. Otherwise, it sets the
     * least significant operand if it is not already set.
     * @tparam T The type of the operand to add.
     * @param operand The operand to add.
     * @return true if the operand was added, false otherwise.
     */
    template <IOperand<MostSigOpT, LeastSigOpT> T>
    auto AddOperand(const std::unique_ptr<T>& operand) -> bool
    {
        if (mostSigOp && leastSigOp) {
            return false;
        }

        assert(operand != nullptr);

        if (mostSigOp == nullptr) {
            mostSigOp = operand->Copy();
            return true;
        }

        if (leastSigOp == nullptr) {
            leastSigOp = operand->Copy();
            return true;
        }

        return false;
    }

    /**
     * Sets the most significant operand of this expression.
     * @param op The operand to set.
     */
    auto SetMostSigOp(const MostSigOpT& op) -> void
    {
        if constexpr (std::is_same_v<MostSigOpT, Expression>) {
            mostSigOp = op.Copy();
        } else {
            mostSigOp = std::make_unique<MostSigOpT>(op);
        }
    }

    /**
     * Sets the least significant operand of this expression.
     * @param op The operand to set.
     */
    auto SetLeastSigOp(const LeastSigOpT& op) -> void
    {
        if constexpr (std::is_same_v<LeastSigOpT, Expression>) {
            leastSigOp = op.Copy();
        } else {
            leastSigOp = std::make_unique<LeastSigOpT>(op);
        }
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
     * Assignment operator.
     * @param other The other expression to assign to this expression.
     * @return This expression.
     */
    auto operator=(const BinaryExpressionBase<MostSigOpT, LeastSigOpT>& other) -> BinaryExpressionBase<MostSigOpT, LeastSigOpT>& = default;

protected:
    std::unique_ptr<MostSigOpT> mostSigOp;
    std::unique_ptr<LeastSigOpT> leastSigOp;
};

/**
 * A binary expression.
 *
 * The BinaryExpression class is a base class for all binary expressions. It provides a common
 * interface for all binary expressions, and implements common functionality. Specifically, it provides
 * functionality dependent on the Derived class. The Derived class must be a template class that takes
 * two template parameters, the types of the most significant and least significant operands, respectively.
 * This class uses a [Curiously Recurring Template Pattern](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern)
 *
 * @note This class is not intended to be used directly by end users.
 *
 * @tparam DerivedT The derived class.
 * @tparam MostSigOpT The type of the most significant operand.
 * @tparam LeastSigOpT The type of the least significant operand.
 */
template <template <IExpression, IExpression> class DerivedT, IExpression MostSigOpT = Expression, IExpression LeastSigOpT = MostSigOpT>
class BinaryExpression : public BinaryExpressionBase<MostSigOpT, LeastSigOpT> {

    using DerivedSpecialized = DerivedT<MostSigOpT, LeastSigOpT>;
    using DerivedGeneralized = DerivedT<Expression, Expression>;

public:
    BinaryExpression() = default;
    BinaryExpression(const BinaryExpression<DerivedT, MostSigOpT, LeastSigOpT>& other)
        : BinaryExpressionBase<MostSigOpT, LeastSigOpT>(other)
    {
    }

    BinaryExpression(const MostSigOpT& mostSigOp, const LeastSigOpT& leastSigOp)
        : BinaryExpressionBase<MostSigOpT, LeastSigOpT>(mostSigOp, leastSigOp)
    {
    }

    [[nodiscard]] auto Copy() const -> std::unique_ptr<Expression> final
    {
        return std::make_unique<DerivedSpecialized>(*static_cast<const DerivedSpecialized*>(this));
    }

    auto Copy(tf::Subflow& subflow) const -> std::unique_ptr<Expression> final
    {
        DerivedSpecialized copy;

        // This is not actually parallelized.
        if (this->mostSigOp) {
            subflow.emplace([this, &copy](tf::Subflow&) {
                copy.SetMostSigOp(*this->mostSigOp);
            });
        }

        if (this->leastSigOp) {
            subflow.emplace([this, &copy](tf::Subflow&) {
                copy.SetLeastSigOp(*this->leastSigOp);
            });
        }

        subflow.join();

        return std::make_unique<DerivedSpecialized>(copy);
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

    /**
     * Swaps the operands of this expression.
     * @return A new expression with the operands swapped.
     */
    auto SwapOperands() -> DerivedT<LeastSigOpT, MostSigOpT>
    {
        return DerivedT { *this->leastSigOp, *this->mostSigOp };
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

    auto operator=(const BinaryExpression& other) -> BinaryExpression& = default;
};

/// @cond
template <template <IExpression, IExpression> class DerivedT>
class BinaryExpression<DerivedT, Expression, Expression> : public BinaryExpressionBase<Expression, Expression> {

    using DerivedGeneralized = DerivedT<Expression, Expression>;

public:
    BinaryExpression() = default;
    BinaryExpression(const BinaryExpression& other)
        : BinaryExpressionBase<Expression, Expression>(other)
    {
    }

    BinaryExpression(const Expression& mostSigOp, const Expression& leastSigOp)
        : BinaryExpressionBase<Expression, Expression>(mostSigOp, leastSigOp)
    {
    }

    [[nodiscard]] auto Copy() const -> std::unique_ptr<Expression> override
    {
        return std::make_unique<DerivedGeneralized>(*static_cast<const DerivedGeneralized*>(this));
    }

    auto Copy(tf::Subflow& subflow) const -> std::unique_ptr<Expression> final
    {
        std::unique_ptr<DerivedGeneralized> copy = std::make_unique<DerivedGeneralized>();

        // This is not actually parallelized.
        if (mostSigOp) {
            subflow.emplace([this, &copy](tf::Subflow&) {
                copy->SetMostSigOp(*mostSigOp);
            });
        }

        if (leastSigOp) {
            subflow.emplace([&](tf::Subflow&) {
                copy->SetLeastSigOp(*leastSigOp);
            });
        }

        subflow.join();

        return copy;
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

    auto SwapOperands() -> DerivedGeneralized
    {
        return DerivedGeneralized { *this->leastSigOp, *this->mostSigOp };
    }

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

    auto operator=(const BinaryExpression& other) -> BinaryExpression& = default;
};
/// @endcond

/**
 * Builds a reasonably balanced binary expression from a vector of operands.
 * @tparam T The type of the binary expression, e.g. Add and Multiply.
 * @param ops The vector of operands.
 * @return A binary expression with the operands in the vector.
 */
template <template <typename, typename> typename T>
    requires IAssociativeAndCommutative<T>
auto BuildFromVector(const std::vector<std::unique_ptr<Expression>>& ops) -> std::unique_ptr<Expression>
{
    using GeneralizedT = T<Expression, Expression>;

    if (ops.size() == 2) {
        return std::make_unique<GeneralizedT>(*ops[0], *ops[1]);
    }

    std::vector<std::unique_ptr<Expression>> reducedOps;
    reducedOps.reserve((ops.size() / 2) + 1);

    for (unsigned int i = 0; i < ops.size(); i += 2) {
        if (i + 1 >= ops.size()) {
            reducedOps.push_back(ops[i]->Copy());
            break;
        }

        reducedOps.push_back(std::make_unique<GeneralizedT>(*ops[i], *ops[i + 1]));
    }

    return BuildFromVector<T>(reducedOps);
}

#define IMPL_SPECIALIZE(Derived, FirstOp, SecondOp)                                                                      \
    static auto Specialize(const Expression& other) -> std::unique_ptr<Derived<FirstOp, SecondOp>>                       \
    {                                                                                                                    \
        if (!other.Is<Oasis::Derived>()) {                                                                               \
            return nullptr;                                                                                              \
        }                                                                                                                \
                                                                                                                         \
        Derived<FirstOp, SecondOp> specialized;                                                                          \
                                                                                                                         \
        std::unique_ptr<Expression> otherGeneralized = other.Generalize();                                               \
        const auto& otherBinaryExpression = dynamic_cast<const Derived<Expression>&>(*otherGeneralized);                 \
                                                                                                                         \
        bool swappedOperands = false;                                                                                    \
                                                                                                                         \
        if (otherBinaryExpression.HasMostSigOp()) {                                                                      \
            auto specializedMostSigOp = FirstOp::Specialize(otherBinaryExpression.GetMostSigOp());                       \
            if (!specializedMostSigOp) {                                                                                 \
                if (!(Derived::GetStaticCategory() & Commutative)) {                                                     \
                    return nullptr;                                                                                      \
                }                                                                                                        \
                specializedMostSigOp = FirstOp::Specialize(otherBinaryExpression.GetLeastSigOp());                       \
                if (specializedMostSigOp) {                                                                              \
                    swappedOperands = true;                                                                              \
                } else {                                                                                                 \
                    return nullptr;                                                                                      \
                }                                                                                                        \
            }                                                                                                            \
            specialized.SetMostSigOp(*specializedMostSigOp);                                                             \
        }                                                                                                                \
                                                                                                                         \
        if (otherBinaryExpression.HasLeastSigOp()) {                                                                     \
            if (swappedOperands && otherBinaryExpression.HasMostSigOp()) {                                               \
                auto specializedLeastSigOp = SecondOp::Specialize(otherBinaryExpression.GetMostSigOp());                 \
                if (!specializedLeastSigOp) {                                                                            \
                    return nullptr;                                                                                      \
                }                                                                                                        \
                specialized.SetLeastSigOp(*specializedLeastSigOp);                                                       \
            } else {                                                                                                     \
                auto specializedLeastSigOp = SecondOp::Specialize(otherBinaryExpression.GetLeastSigOp());                \
                if (!specializedLeastSigOp) {                                                                            \
                    return nullptr;                                                                                      \
                }                                                                                                        \
                specialized.SetLeastSigOp(*specializedLeastSigOp);                                                       \
            }                                                                                                            \
        }                                                                                                                \
                                                                                                                         \
        return std::make_unique<Derived<FirstOp, SecondOp>>(specialized);                                                \
    }                                                                                                                    \
                                                                                                                         \
    static auto Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Derived<FirstOp, SecondOp>> \
    {                                                                                                                    \
        if (!other.Is<Oasis::Derived>()) {                                                                               \
            return nullptr;                                                                                              \
        }                                                                                                                \
                                                                                                                         \
        Derived<FirstOp, SecondOp> multiply;                                                                             \
                                                                                                                         \
        std::unique_ptr<Expression> otherGeneralized;                                                                    \
                                                                                                                         \
        tf::Task generalizeTask = subflow.emplace([&other, &otherGeneralized](tf::Subflow& sbf) {                        \
            otherGeneralized = other.Generalize(sbf);                                                                    \
        });                                                                                                              \
                                                                                                                         \
        tf::Task mostSigOpTask = subflow.emplace([&multiply, &otherGeneralized](tf::Subflow& sbf) {                      \
            const auto& otherBinaryExpression = dynamic_cast<const Derived<Expression>&>(*otherGeneralized);             \
            if (otherBinaryExpression.HasMostSigOp()) {                                                                  \
                multiply.SetMostSigOp(*FirstOp::Specialize(otherBinaryExpression.GetMostSigOp(), sbf));                  \
            }                                                                                                            \
        });                                                                                                              \
                                                                                                                         \
        mostSigOpTask.succeed(generalizeTask);                                                                           \
                                                                                                                         \
        tf::Task leastSigOpTask = subflow.emplace([&multiply, &otherGeneralized](tf::Subflow& sbf) {                     \
            const auto& otherBinaryExpression = dynamic_cast<const Derived<Expression>&>(*otherGeneralized);             \
            if (otherBinaryExpression.HasLeastSigOp()) {                                                                 \
                multiply.SetLeastSigOp(*SecondOp::Specialize(otherBinaryExpression.GetLeastSigOp(), sbf));               \
            }                                                                                                            \
        });                                                                                                              \
                                                                                                                         \
        leastSigOpTask.succeed(generalizeTask);                                                                          \
                                                                                                                         \
        subflow.join();                                                                                                  \
                                                                                                                         \
        return std::make_unique<Derived<FirstOp, SecondOp>>(multiply);                                                   \
    }
} // Oasis

#endif // OASIS_BINARYEXPRESSION_HPP

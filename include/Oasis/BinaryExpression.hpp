//
// Created by Matthew McCall on 7/2/23.
//

#ifndef OASIS_BINARYEXPRESSION_HPP
#define OASIS_BINARYEXPRESSION_HPP

#include <list>

#include "taskflow/taskflow.hpp"

#include "Expression.hpp"

namespace Oasis {
/**
 * A concept for an operand of a binary expression.
 * @tparam MostSigOpT The type of the most significant operand.
 * @tparam LeastSigOpT The type of the least significant operand.
 * @tparam T The type to check.
 */
template <typename MostSigOpT, typename LeastSigOpT, typename T>
concept IOperand = std::is_same_v<T, MostSigOpT> || std::is_same_v<T, LeastSigOpT>;

template <template <typename, typename> typename T>
concept IAssociativeAndCommutative = IExpression<T<Expression, Expression>> && ((T<Expression, Expression>::GetStaticCategory() & (Associative | Commutative)) == (Associative | Commutative));

template <typename T, typename... U>
concept IsAnyOf = (std::same_as<T, U> || ...);

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
class BinaryExpression : public Expression {

    using DerivedSpecialized = DerivedT<MostSigOpT, LeastSigOpT>;
    using DerivedGeneralized = DerivedT<Expression, Expression>;

public:
    BinaryExpression() = default;
    BinaryExpression(const BinaryExpression& other)
    {
        if (other.HasMostSigOp()) {
            SetMostSigOp(other.GetMostSigOp());
        }

        if (other.HasLeastSigOp()) {
            SetLeastSigOp(other.GetLeastSigOp());
        }
    }

    BinaryExpression(const MostSigOpT& mostSigOp, const LeastSigOpT& leastSigOp)
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

    [[nodiscard]] auto StructurallyEquivalent(const Expression& other) const -> bool override
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
    auto SetMostSigOp(const MostSigOpT& op) -> void
    {
        if constexpr (std::same_as<MostSigOpT, Expression>) {
            this->mostSigOp = op.Copy();
        } else {
            this->mostSigOp = std::make_unique<MostSigOpT>(op);
        }
    }

    /**
     * Sets the least significant operand of this expression.
     * @param op The operand to set.
     */
    auto SetLeastSigOp(const LeastSigOpT& op) -> void
    {
        if constexpr (std::same_as<LeastSigOpT, Expression>) {
            this->leastSigOp = op.Copy();
        } else {
            this->leastSigOp = std::make_unique<LeastSigOpT>(op);
        }
    }

    template <typename T>
        requires IsAnyOf<T, MostSigOpT, Expression>
    auto SetMostSigOp(std::unique_ptr<T>&& op) -> void
    {
        if constexpr (std::same_as<T, Expression>) {
            auto specializedOp = MostSigOpT::Specialize(*op);
            assert(specializedOp);
            this->mostSigOp = std::move(specializedOp);
        } else {
            this->mostSigOp = std::move(op);
        }
    }

    template <typename T>
        requires IsAnyOf<T, LeastSigOpT, Expression>
    auto SetLeastSigOp(std::unique_ptr<T>&& op) -> void
    {
        if constexpr (std::same_as<T, Expression>) {
            auto specializedOp = LeastSigOpT::Specialize(*op);
            assert(specializedOp);
            this->leastSigOp = std::move(specializedOp);
        } else {
            this->leastSigOp = std::move(op);
        }
    }

    template <typename T>
        requires IsAnyOf<T, MostSigOpT, Expression>
    auto SetMostSigOp(std::unique_ptr<T>&& op, tf::Subflow& subflow) -> void
    {
        if constexpr (std::same_as<T, Expression>) {
            auto specializedOp = MostSigOpT::Specialize(*op, subflow);
            assert(specializedOp);
            this->mostSigOp = std::move(specializedOp);
        } else {
            this->mostSigOp = std::move(op);
        }
    }

    template <typename T>
        requires IsAnyOf<T, LeastSigOpT, Expression>
    auto SetLeastSigOp(std::unique_ptr<T>&& op, tf::Subflow& subflow) -> void
    {
        if constexpr (std::same_as<T, Expression>) {
            auto specializedOp = LeastSigOpT::Specialize(*op, subflow);
            assert(specializedOp);
            this->leastSigOp = std::move(specializedOp);
        } else {
            this->leastSigOp = std::move(op);
        }
    }

    /**
     * Swaps the operands of this expression.
     * @return A new expression with the operands swapped.
     */
    auto SwapOperands() const -> DerivedT<LeastSigOpT, MostSigOpT>
    {
        return DerivedT { *this->leastSigOp, *this->mostSigOp };
    }

    auto operator=(const BinaryExpression& other) -> BinaryExpression& = default;

protected:
    std::unique_ptr<MostSigOpT> mostSigOp;
    std::unique_ptr<LeastSigOpT> leastSigOp;
};

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

    std::list<std::unique_ptr<Expression>> opsList;
    opsList.resize(ops.size());

    std::transform(ops.begin(), ops.end(), opsList.begin(), [](const auto& op) { return op->Copy(); });

    while (std::next(opsList.begin()) != opsList.end()) {
        for (auto i = opsList.begin(); i != opsList.end() && std::next(i) != opsList.end();) {
            auto node = std::make_unique<GeneralizedT>(**i, **std::next(i));
            opsList.insert(i, std::move(node));
            i = opsList.erase(i, std::next(i, 2));
        }
    }

    return std::move(opsList.front());
}

#define IMPL_SPECIALIZE(Derived, FirstOp, SecondOp)                                                                      \
    static auto Specialize(const Expression& other) -> std::unique_ptr<Derived<FirstOp, SecondOp>>                       \
    {                                                                                                                    \
        if (!other.Is<Oasis::Derived>()) {                                                                               \
            return nullptr;                                                                                              \
        }                                                                                                                \
                                                                                                                         \
        auto specialized = std::make_unique<Derived<FirstOp, SecondOp>>();                                               \
                                                                                                                         \
        std::unique_ptr<Expression> otherGeneralized = other.Generalize();                                               \
        const auto& otherBinaryExpression = static_cast<const Derived<Expression>&>(*otherGeneralized);                  \
                                                                                                                         \
        bool leftOperandSpecialized = true, rightOperandSpecialized = true;                                              \
                                                                                                                         \
        if (otherBinaryExpression.HasMostSigOp()) {                                                                      \
            specialized->SetMostSigOp(FirstOp::Specialize(otherBinaryExpression.GetMostSigOp()));                        \
            leftOperandSpecialized = specialized->HasMostSigOp();                                                        \
        }                                                                                                                \
                                                                                                                         \
        if (otherBinaryExpression.HasLeastSigOp()) {                                                                     \
            specialized->SetLeastSigOp(SecondOp::Specialize(otherBinaryExpression.GetLeastSigOp()));                     \
            rightOperandSpecialized = specialized->HasLeastSigOp();                                                      \
        }                                                                                                                \
                                                                                                                         \
        if (leftOperandSpecialized && rightOperandSpecialized) {                                                         \
            return specialized;                                                                                          \
        }                                                                                                                \
                                                                                                                         \
        if (!(other.GetCategory() & Commutative)) {                                                                      \
            return nullptr;                                                                                              \
        }                                                                                                                \
                                                                                                                         \
        leftOperandSpecialized = true, rightOperandSpecialized = true;                                                   \
        specialized = std::make_unique<Derived<FirstOp, SecondOp>>();                                                    \
                                                                                                                         \
        auto otherWithSwappedOps                                                                                         \
            = otherBinaryExpression.SwapOperands();                                                                      \
        if (otherWithSwappedOps.HasMostSigOp()) {                                                                        \
            specialized->SetMostSigOp(FirstOp::Specialize(otherWithSwappedOps.GetMostSigOp()));                          \
            leftOperandSpecialized = specialized->HasMostSigOp();                                                        \
        }                                                                                                                \
                                                                                                                         \
        if (otherWithSwappedOps.HasLeastSigOp()) {                                                                       \
            specialized->SetLeastSigOp(SecondOp::Specialize(otherWithSwappedOps.GetLeastSigOp()));                       \
            rightOperandSpecialized = specialized->HasLeastSigOp();                                                      \
        }                                                                                                                \
                                                                                                                         \
        if (leftOperandSpecialized && rightOperandSpecialized) {                                                         \
            return specialized;                                                                                          \
        }                                                                                                                \
                                                                                                                         \
        return nullptr;                                                                                                  \
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

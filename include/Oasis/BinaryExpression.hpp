//
// Created by Matthew McCall on 7/2/23.
//

#ifndef OASIS_BINARYEXPRESSION_HPP
#define OASIS_BINARYEXPRESSION_HPP

#include <algorithm>
#include <cassert>
#include <functional>
#include <list>

#include "Expression.hpp"
#include "Oasis/SimplifyVisitor.hpp"
#include "RecursiveCast.hpp"
#include "Visit.hpp"

namespace Oasis {
/**
 * A concept for an operand of a binary expression.
 * @note This class is not intended to be used directly by end users.
 *
 * @section Parameters
 * @tparam MostSigOpT The type of the most significant operand.
 * @tparam LeastSigOpT The type of the least significant operand.
 * @tparam T The type to check.
 */
template <typename MostSigOpT, typename LeastSigOpT, typename T>
concept IOperand = std::is_same_v<T, MostSigOpT> || std::is_same_v<T, LeastSigOpT>;

template <template <typename, typename> typename T>
concept IAssociativeAndCommutative = IExpression<T<Expression, Expression>> && ((T<Expression, Expression>::GetStaticCategory() & (Associative | Commutative)) == (Associative | Commutative));

/**
 * Builds a reasonably balanced binary expression from a vector of operands.
 * @tparam T The type of the binary expression, e.g. Add or Multiply.
 * @param ops The vector of operands. Must have a minimum of 2 operands.
 * @return A binary expression with the operands in the vector, or a nullptr if ops.size() <=1.
 */
template <template <typename, typename> typename T>
    requires IAssociativeAndCommutative<T>
auto BuildFromVector(const std::vector<std::unique_ptr<Expression>>& ops) -> std::unique_ptr<T<Expression, Expression>>
{
    if (ops.size() <= 1) {
        return nullptr;
    }

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

    auto* result = dynamic_cast<GeneralizedT*>(opsList.front().release());
    return std::unique_ptr<GeneralizedT>(result);
}

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

    template <IExpression Op1T, IExpression Op2T, IExpression... OpsT>
    BinaryExpression(const Op1T& op1, const Op2T& op2, const OpsT&... ops)
    {
        static_assert(IAssociativeAndCommutative<DerivedT>, "List initializer only supported for associative and commutative expressions");
        static_assert(std::is_same_v<DerivedGeneralized, DerivedSpecialized>, "List initializer only supported for generalized expressions");

        std::vector<std::unique_ptr<Expression>> opsVec;

        for (auto opWrapper : std::vector<std::reference_wrapper<const Expression>> { static_cast<const Expression&>(op1), static_cast<const Expression&>(op2), (static_cast<const Expression&>(ops))... }) {
            const Expression& operand = opWrapper.get();
            opsVec.emplace_back(operand.Copy());
        }

        // build expression from vector
        auto generalized = BuildFromVector<DerivedT>(opsVec);

        SetLeastSigOp(generalized->GetLeastSigOp());
        SetMostSigOp(generalized->GetMostSigOp());
    }

    [[nodiscard]] auto Copy() const -> std::unique_ptr<Expression> final
    {
        return std::make_unique<DerivedSpecialized>(*static_cast<const DerivedSpecialized*>(this));
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

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> override
    {
        SimplifyVisitor simplifyVisitor {};
        auto e = Generalize();
        auto s = e->Accept(simplifyVisitor);
        if (!s) {
            return e;
        }
        return std::move(s).value();
    }

    [[nodiscard]] auto Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression> override
    {
        return Generalize()->Integrate(integrationVariable);
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
        if (mostSigOp) {
            if (this->mostSigOp->template Is<DerivedGeneralized>()) {
                auto generalizedMostSigOp = this->mostSigOp->Generalize();
                const auto& mostSigOp = static_cast<const DerivedGeneralized&>(*generalizedMostSigOp);
                mostSigOp.Flatten(out);
            } else {
                out.push_back(this->mostSigOp->Copy());
            }
        }

        if (leastSigOp) {
            if (this->leastSigOp->template Is<DerivedGeneralized>()) {
                auto generalizedLeastSigOp = this->leastSigOp->Generalize();
                const auto& leastSigOp = static_cast<const DerivedGeneralized&>(*generalizedLeastSigOp);
                leastSigOp.Flatten(out);
            } else {
                out.push_back(this->leastSigOp->Copy());
            }
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
        // TODO: FIX WITH VISITOR?
        std::unique_ptr<Expression> left = ((GetMostSigOp()).Copy())->Substitute(var, val);
        std::unique_ptr<Expression> right = ((GetLeastSigOp().Copy())->Substitute(var, val));
        DerivedT<Expression, Expression> comb = DerivedT<Expression, Expression> { *left, *right };

        Oasis::SimplifyVisitor simplifyVisitor {};
        auto simplified = comb.Accept(simplifyVisitor);
        if (!simplified) {
            return comb.Generalize();
        }
        return std::move(simplified.value());
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

    auto AcceptInternal(Visitor& visitor) const -> any override
    {
        const auto generalized = Generalize();
        const auto& derivedGeneralized = dynamic_cast<const DerivedGeneralized&>(*generalized);
        return visitor.Visit(derivedGeneralized);
    }

    std::unique_ptr<MostSigOpT> mostSigOp;
    std::unique_ptr<LeastSigOpT> leastSigOp;
};

} // Oasis

#endif // OASIS_BINARYEXPRESSION_HPP

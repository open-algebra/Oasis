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

template <>
class BinaryExpressionBase<Expression, Expression> : public Expression {
public:
    BinaryExpressionBase() = default;
    BinaryExpressionBase(const BinaryExpressionBase& other);

    BinaryExpressionBase(const Expression& mostSigOp, const Expression& leastSigOp);

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

// concept to check if type is one of two operand types
template <typename FirstOpT, typename SecondOpT, typename T>
concept IOperand = std::is_same_v<T, FirstOpT> || std::is_same_v<T, SecondOpT>;

template <IExpression MostSigOpT = Expression, IExpression LeastSigOpT = MostSigOpT>
class BinaryExpressionBase : public Expression {
public:
    BinaryExpressionBase() = default;
    BinaryExpressionBase(const BinaryExpressionBase<MostSigOpT, LeastSigOpT>& other)
    {
        if (other.mostSigOp) {
            mostSigOp = std::make_unique<MostSigOpT>(*other.mostSigOp);
        }

        if (other.leastSigOp) {
            leastSigOp = std::make_unique<LeastSigOpT>(*other.leastSigOp);
        }
    }

    BinaryExpressionBase(const MostSigOpT& mostSigOp, const LeastSigOpT& leastSigOp)
        : mostSigOp(std::make_unique<MostSigOpT>(mostSigOp))
        , leastSigOp(std::make_unique<LeastSigOpT>(leastSigOp))
    { }

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

        std::unique_ptr<Expression> otherNormalized = other.Generalize();
        const auto& otherBinary = static_cast<const BinaryExpressionBase&>(*otherNormalized);

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

        leastSigOp = operand->Copy();
        return true;
    }

    auto SetMostSigOp(const MostSigOpT& op) -> void
    {
        mostSigOp = std::make_unique<MostSigOpT>(op);
    }

    auto SetLeastSigOp(const LeastSigOpT& op) -> void
    {
        leastSigOp = std::make_unique<LeastSigOpT>(op);
    }

    [[nodiscard]] auto HasMostSigOp() const -> bool
    {
        return mostSigOp != nullptr;
    }

    [[nodiscard]] auto HasLeastSigOp() const -> bool
    {
        return leastSigOp != nullptr;
    }

    auto GetMostSigOp() const -> const MostSigOpT&
    {
        assert(mostSigOp != nullptr);
        return *mostSigOp;
    }

    auto GetLeastSigOp() const -> const LeastSigOpT&
    {
        assert(leastSigOp != nullptr);
        return *leastSigOp;
    }

    auto operator=(const BinaryExpressionBase<MostSigOpT, LeastSigOpT>& other) -> BinaryExpressionBase<MostSigOpT, LeastSigOpT>& = default;

protected:
    std::unique_ptr<MostSigOpT> mostSigOp;
    std::unique_ptr<LeastSigOpT> leastSigOp;
};

template <typename Derived, IExpression MostSigOpT = Expression, IExpression LeastSigOpT = MostSigOpT>
class BinaryExpression : public BinaryExpressionBase<MostSigOpT, LeastSigOpT> {
public:
    BinaryExpression() = default;
    BinaryExpression(const BinaryExpression<Derived, MostSigOpT, LeastSigOpT>& other)
        : BinaryExpressionBase<MostSigOpT, LeastSigOpT>(other)
    { }

    BinaryExpression(const MostSigOpT& mostSigOp, const LeastSigOpT& leastSigOp)
        : BinaryExpressionBase<MostSigOpT, LeastSigOpT>(mostSigOp, leastSigOp)
    { }

    [[nodiscard]] auto Copy() const -> std::unique_ptr<Expression> final
    {
        return std::make_unique<Derived>(*static_cast<const Derived*>(this));
    }

    auto Copy(tf::Subflow& subflow) const -> std::unique_ptr<Expression> final
    {
        std::unique_ptr<Derived> copy = std::make_unique<Derived>();

        if (this->mostSigOp) {
            subflow.emplace([this, &copy](tf::Subflow& sbf) {
                copy->SetMostSigOp(*this->mostSigOp);
            });
        }

        if (this->leastSigOp) {
            subflow.emplace([this, &copy](tf::Subflow& sbf) {
                copy->SetLeastSigOp(*this->leastSigOp);
            });
        }

        subflow.join();

        return copy;
    }

    auto operator=(const BinaryExpression& other) -> BinaryExpression& = default;
};

template <typename Derived>
class BinaryExpression<Derived, Expression, Expression> : public BinaryExpressionBase<Expression, Expression> {
public:
    BinaryExpression() = default;
    BinaryExpression(const BinaryExpression& other)
        : BinaryExpressionBase<Expression, Expression>(other)
    { }

    BinaryExpression(const Expression& mostSigOp, const Expression& leastSigOp)
        : BinaryExpressionBase<Expression, Expression>(mostSigOp, leastSigOp)
    { }

    [[nodiscard]] auto Copy() const -> std::unique_ptr<Expression> override
    {
        return std::make_unique<Derived>(*static_cast<const Derived*>(this));
    }

    auto Copy(tf::Subflow& subflow) const -> std::unique_ptr<Expression> final
    {
        std::unique_ptr<Derived> copy = std::make_unique<Derived>();

        if (mostSigOp) {
            subflow.emplace([this, &copy](tf::Subflow& sbf) {
                copy->SetMostSigOp(*mostSigOp);
            });
        }

        if (leastSigOp) {
            subflow.emplace([&](tf::Subflow& sbf) {
                copy->SetLeastSigOp(*leastSigOp);
            });
        }

        subflow.join();

        return copy;
    }

    auto operator=(const BinaryExpression& other) -> BinaryExpression& = default;
};

} // Oasis

#endif // OASIS_BINARYEXPRESSION_HPP

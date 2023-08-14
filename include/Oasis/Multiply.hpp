//
// Created by Matthew McCall on 8/10/23.
//

#ifndef OASIS_MULTIPLY_HPP
#define OASIS_MULTIPLY_HPP

#include "fmt/core.h"

#include "BinaryExpression.hpp"
#include "Real.hpp"

namespace Oasis {

template <IExpression MultiplicandT, IExpression MultiplierT>
class Multiply;

template <>
class Multiply<Expression, Expression> : public BinaryExpression<Multiply<Expression, Expression>> {
public:
    Multiply() = default;
    Multiply(const Multiply<Expression, Expression>& other) = default;
    
    Multiply(const Expression& multiplicand, const Expression& multiplier);

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> final;
    auto Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto ToString() const -> std::string final;

    static auto Specialize(const Expression& other) -> std::unique_ptr<Multiply>;
    static auto Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Multiply>;

    EXPRESSION_TYPE(Multiply)
    EXPRESSION_CATEGORY(Associative | Commutative)
};

template <IExpression MultiplicandT = Expression, IExpression MultiplierT = MultiplicandT>
class Multiply : public BinaryExpression<Multiply<MultiplicandT, MultiplierT>, MultiplicandT, MultiplierT> {
public:
    Multiply() = default;
    Multiply(const Multiply<MultiplicandT, MultiplierT>& other)
        : BinaryExpression<Multiply<MultiplicandT, MultiplierT>, MultiplicandT, MultiplierT>(other)
    { }
    
    Multiply(const MultiplicandT& addend1, const MultiplierT& addend2)
        : BinaryExpression<Multiply<MultiplicandT, MultiplierT>, MultiplicandT, MultiplierT>(addend1, addend2)
    { }

    [[nodiscard]] auto Generalize() const -> std::unique_ptr<Expression> final
    {
        Multiply<Expression> generalizedSubtract;

        if (this->mostSigOp) {
            generalizedSubtract.SetMostSigOp(*this->mostSigOp->Copy());
        }

        if (this->leastSigOp) {
            generalizedSubtract.SetLeastSigOp(*this->leastSigOp->Copy());
        }

        return std::make_unique<Multiply<Expression>>(generalizedSubtract);
    }

    auto Generalize(tf::Subflow& subflow) const -> std::unique_ptr<Expression> final
    {
        Multiply<Expression> generalizedSubtract;

        if (this->mostSigOp) {
            subflow.emplace([this, &generalizedSubtract](tf::Subflow& sbf) {
                generalizedSubtract.SetMostSigOp(*this->mostSigOp->Copy(sbf));
            });
        }

        if (this->leastSigOp) {
            subflow.emplace([this, &generalizedSubtract](tf::Subflow& sbf) {
                generalizedSubtract.SetLeastSigOp(*this->leastSigOp->Copy(sbf));
            });
        }

        subflow.join();

        return std::make_unique<Multiply<Expression>>(generalizedSubtract);
    }

    [[nodiscard]] auto ToString() const -> std::string final
    {
        return fmt::format("({} + {})", this->mostSigOp->ToString(), this->leastSigOp->ToString());
    }

    static auto Specialize(const Expression& other) -> std::unique_ptr<Multiply<MultiplicandT, MultiplierT>>
    {
        Multiply<MultiplicandT, MultiplierT> subtract;

        if (!subtract.StructurallyEquivalent(other)) {
            return nullptr;
        }

        std::unique_ptr<Expression> otherNormalized = other.Generalize();
        const auto& otherBinaryExpression = dynamic_cast<const Multiply<Expression>&>(*otherNormalized);

        if (otherBinaryExpression.HasMostSigOp()) {
            subtract.SetMostSigOp(*MultiplicandT::Specialize(otherBinaryExpression.GetMostSigOp()));
        }

        if (otherBinaryExpression.HasLeastSigOp()) {
            subtract.SetLeastSigOp(*MultiplierT::Specialize(otherBinaryExpression.GetLeastSigOp()));
        }

        return std::make_unique<Multiply<MultiplicandT, MultiplierT>>(subtract);
    }

    static auto Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Multiply<MultiplicandT, MultiplierT>>
    {
        if (!other.Is<Multiply>()) {
            return nullptr;
        }

        Multiply<MultiplicandT, MultiplierT> multiply;

        std::unique_ptr<Expression> otherNormalized;

        tf::Task generalizeTask = subflow.emplace([&other, &otherNormalized](tf::Subflow& sbf) {
            otherNormalized = other.Generalize(sbf);
        });

        tf::Task mostSigOpTask = subflow.emplace([&multiply, &otherNormalized](tf::Subflow& sbf) {
            const auto& otherBinaryExpression = dynamic_cast<const Multiply<Expression>&>(*otherNormalized);
            if (otherBinaryExpression.HasMostSigOp()) {
                multiply.SetMostSigOp(*MultiplicandT::Specialize(otherBinaryExpression.GetMostSigOp(), sbf));
            }
        });

        mostSigOpTask.succeed(generalizeTask);

        tf::Task leastSigOpTask = subflow.emplace([&multiply, &otherNormalized](tf::Subflow& sbf) {
            const auto& otherBinaryExpression = dynamic_cast<const Multiply<Expression>&>(*otherNormalized);
            if (otherBinaryExpression.HasLeastSigOp()) {
                multiply.SetLeastSigOp(*MultiplierT::Specialize(otherBinaryExpression.GetLeastSigOp(), sbf));
            }
        });

        leastSigOpTask.succeed(generalizeTask);

        subflow.join();

        return std::make_unique<Multiply<MultiplicandT, MultiplierT>>(multiply);
    }

    auto operator=(const Multiply& other) -> Multiply& = default;

    EXPRESSION_TYPE(Multiply)
    EXPRESSION_CATEGORY(Associative | Commutative)
};

} // Oasis

#endif // OASIS_MULTIPLY_HPP

//
// Created by Matthew McCall on 8/10/23.
//

#ifndef OASIS_DIVIDE_HPP
#define OASIS_DIVIDE_HPP

#include "fmt/core.h"

#include "BinaryExpression.hpp"
#include "Real.hpp"

namespace Oasis {

template <IExpression DividendT, IExpression DivisorT>
class Divide;

template <>
class Divide<Expression, Expression> : public BinaryExpression<Divide> {
public:
    Divide() = default;
    Divide(const Divide<Expression, Expression>& other) = default;

    Divide(const Expression& dividend, const Expression& divisor);

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> final;
    auto Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto ToString() const -> std::string final;

    static auto Specialize(const Expression& other) -> std::unique_ptr<Divide>;
    static auto Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Divide>;

    EXPRESSION_TYPE(Divide)
    EXPRESSION_CATEGORY(Associative | Commutative)
};

template <IExpression DividendT = Expression, IExpression DivisorT = DividendT>
class Divide : public BinaryExpression<Divide, DividendT, DivisorT> {
public:
    Divide() = default;
    Divide(const Divide<DividendT, DivisorT>& other)
        : BinaryExpression<Divide, DividendT, DivisorT>(other)
    { }

    Divide(const DividendT& addend1, const DivisorT& addend2)
        : BinaryExpression<Divide, DividendT, DivisorT>(addend1, addend2)
    { }

    [[nodiscard]] auto ToString() const -> std::string final
    {
        return fmt::format("({} + {})", this->mostSigOp->ToString(), this->leastSigOp->ToString());
    }

    static auto Specialize(const Expression& other) -> std::unique_ptr<Divide<DividendT, DivisorT>>
    {
        if (!other.Is<Divide>()) {
            return nullptr;
        }

        Divide<DividendT, DivisorT> add;

        std::unique_ptr<Expression> otherNormalized = other.Generalize();
        const auto& otherBinaryExpression = dynamic_cast<const Divide<Expression>&>(*otherNormalized);

        if (otherBinaryExpression.HasMostSigOp()) {
            add.SetMostSigOp(*DividendT::Specialize(otherBinaryExpression.GetMostSigOp()));
        }

        if (otherBinaryExpression.HasLeastSigOp()) {
            add.SetLeastSigOp(*DivisorT::Specialize(otherBinaryExpression.GetLeastSigOp()));
        }

        return std::make_unique<Divide<DividendT, DivisorT>>(add);
    }

    static auto Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Divide<DividendT, DivisorT>>
    {
        if (!other.Is<Divide>()) {
            return nullptr;
        }

        Divide<DividendT, DivisorT> add;

        std::unique_ptr<Expression> otherGeneralized;

        tf::Task generalizeTask = subflow.emplace([&other, &otherGeneralized](tf::Subflow& sbf) {
            otherGeneralized = other.Generalize(sbf);
        });

        tf::Task mostSigOpTask = subflow.emplace([&add, &otherGeneralized](tf::Subflow& sbf) {
            const auto& otherBinaryExpression = dynamic_cast<const Divide<Expression>&>(*otherGeneralized);
            if (otherBinaryExpression.HasMostSigOp()) {
                add.SetMostSigOp(*DividendT::Specialize(otherBinaryExpression.GetMostSigOp(), sbf));
            }
        });

        mostSigOpTask.succeed(generalizeTask);

        tf::Task leastSigOpTask = subflow.emplace([&add, &otherGeneralized](tf::Subflow& sbf) {
            const auto& otherBinaryExpression = dynamic_cast<const Divide<Expression>&>(*otherGeneralized);
            if (otherBinaryExpression.HasLeastSigOp()) {
                add.SetLeastSigOp(*DivisorT::Specialize(otherBinaryExpression.GetLeastSigOp(), sbf));
            }
        });

        leastSigOpTask.succeed(generalizeTask);

        subflow.join();

        return std::make_unique<Divide<DividendT, DivisorT>>(add);
    }

    auto operator=(const Divide& other) -> Divide& = default;

    EXPRESSION_TYPE(Divide)
    EXPRESSION_CATEGORY(Associative | Commutative)
};

} // Oasis

#endif // OASIS_DIVIDE_HPP

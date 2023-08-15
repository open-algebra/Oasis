//
// Created by Matthew McCall on 7/2/23.
//

#ifndef OASIS_ADD_HPP
#define OASIS_ADD_HPP

#include "fmt/core.h"

#include "BinaryExpression.hpp"
#include "Real.hpp"

namespace Oasis {

template <IExpression AugendT, IExpression AddendT>
class Add;

template <>
class Add<Expression, Expression> : public BinaryExpression<Add> {
public:
    Add() = default;
    Add(const Add<Expression, Expression>& other) = default;

    Add(const Expression& addend1, const Expression& addend2);

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> final;
    auto Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto ToString() const -> std::string final;

    static auto Specialize(const Expression& other) -> std::unique_ptr<Add>;
    static auto Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Add>;

    EXPRESSION_TYPE(Add)
    EXPRESSION_CATEGORY(Associative | Commutative)
};

template <IExpression AugendT = Expression, IExpression AddendT = AugendT>
class Add : public BinaryExpression<Add, AugendT, AddendT> {
public:
    Add() = default;
    Add(const Add<AugendT, AddendT>& other)
        : BinaryExpression<Add, AugendT, AddendT>(other)
    { }

    Add(const AugendT& addend1, const AddendT& addend2)
        : BinaryExpression<Add, AugendT, AddendT>(addend1, addend2)
    { }

    [[nodiscard]] auto ToString() const -> std::string final
    {
        return fmt::format("({} + {})", this->mostSigOp->ToString(), this->leastSigOp->ToString());
    }

    static auto Specialize(const Expression& other) -> std::unique_ptr<Add<AugendT, AddendT>>
    {
        if (!other.Is<Add>()) {
            return nullptr;
        }

        Add<AugendT, AddendT> add;

        std::unique_ptr<Expression> otherNormalized = other.Generalize();
        const auto& otherBinaryExpression = dynamic_cast<const Add<Expression>&>(*otherNormalized);

        if (otherBinaryExpression.HasMostSigOp()) {
            add.SetMostSigOp(*AugendT::Specialize(otherBinaryExpression.GetMostSigOp()));
        }

        if (otherBinaryExpression.HasLeastSigOp()) {
            add.SetLeastSigOp(*AddendT::Specialize(otherBinaryExpression.GetLeastSigOp()));
        }

        return std::make_unique<Add<AugendT, AddendT>>(add);
    }

    static auto Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Add<AugendT, AddendT>>
    {
        if (!other.Is<Add>()) {
            return nullptr;
        }

        Add<AugendT, AddendT> add;

        std::unique_ptr<Expression> otherGeneralized;

        tf::Task generalizeTask = subflow.emplace([&other, &otherGeneralized](tf::Subflow& sbf) {
            otherGeneralized = other.Generalize(sbf);
        });

        tf::Task mostSigOpTask = subflow.emplace([&add, &otherGeneralized](tf::Subflow& sbf) {
            const auto& otherBinaryExpression = dynamic_cast<const Add<Expression>&>(*otherGeneralized);
            if (otherBinaryExpression.HasMostSigOp()) {
                add.SetMostSigOp(*AugendT::Specialize(otherBinaryExpression.GetMostSigOp(), sbf));
            }
        });

        mostSigOpTask.succeed(generalizeTask);

        tf::Task leastSigOpTask = subflow.emplace([&add, &otherGeneralized](tf::Subflow& sbf) {
            const auto& otherBinaryExpression = dynamic_cast<const Add<Expression>&>(*otherGeneralized);
            if (otherBinaryExpression.HasLeastSigOp()) {
                add.SetLeastSigOp(*AddendT::Specialize(otherBinaryExpression.GetLeastSigOp(), sbf));
            }
        });

        leastSigOpTask.succeed(generalizeTask);

        subflow.join();

        return std::make_unique<Add<AugendT, AddendT>>(add);
    }

    auto operator=(const Add& other) -> Add& = default;

    EXPRESSION_TYPE(Add)
    EXPRESSION_CATEGORY(Associative | Commutative)
};

} // namespace Oasis

#endif // OASIS_ADD_HPP

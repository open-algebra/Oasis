//
// Created by Matthew McCall on 8/10/23.
//

#ifndef OASIS_SUBTRACT_HPP
#define OASIS_SUBTRACT_HPP

#include "fmt/core.h"

#include "BinaryExpression.hpp"
#include "Real.hpp"

namespace Oasis {

template <IExpression MinuendT, IExpression SubtrahendT>
class Subtract;

template <>
class Subtract<Expression, Expression> : public BinaryExpression<Subtract> {
public:
    Subtract() = default;
    Subtract(const Subtract<Expression, Expression>& other) = default;

    Subtract(const Expression& minuend, const Expression& subtrahend);

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> final;
    auto Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto ToString() const -> std::string final;

    static auto Specialize(const Expression& other) -> std::unique_ptr<Subtract>;
    static auto Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Subtract>;

    EXPRESSION_TYPE(Subtract)
    EXPRESSION_CATEGORY(0)
};

template <IExpression MinuendT = Expression, IExpression SubtrahendT = MinuendT>
class Subtract : public BinaryExpression<Subtract, MinuendT, SubtrahendT> {
public:
    Subtract() = default;
    Subtract(const Subtract<MinuendT, SubtrahendT>& other)
        : BinaryExpression<Subtract, MinuendT, SubtrahendT>(other)
    { }

    Subtract(const MinuendT& addend1, const SubtrahendT& addend2)
        : BinaryExpression<Subtract, MinuendT, SubtrahendT>(addend1, addend2)
    { }

    [[nodiscard]] auto ToString() const -> std::string final
    {
        return fmt::format("({} - {})", this->mostSigOp->ToString(), this->leastSigOp->ToString());
    }

    static auto Specialize(const Expression& other) -> std::unique_ptr<Subtract<MinuendT, SubtrahendT>>
    {
        if (!other.Is<Subtract>()) {
            return nullptr;
        }

        Subtract<MinuendT, SubtrahendT> subtract;

        std::unique_ptr<Expression> otherGeneralized = other.Generalize();
        const auto& otherBinaryExpression = dynamic_cast<const Subtract<Expression>&>(*otherGeneralized);

        if (otherBinaryExpression.HasMostSigOp()) {
            subtract.SetMostSigOp(*MinuendT::Specialize(otherBinaryExpression.GetMostSigOp()));
        }

        if (otherBinaryExpression.HasLeastSigOp()) {
            subtract.SetLeastSigOp(*SubtrahendT::Specialize(otherBinaryExpression.GetLeastSigOp()));
        }

        return std::make_unique<Subtract<MinuendT, SubtrahendT>>(subtract);
    }

    static auto Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Subtract<MinuendT, SubtrahendT>>
    {
        if (!other.Is<Subtract>()) {
            return nullptr;
        }

        Subtract<MinuendT, SubtrahendT> subtract;

        std::unique_ptr<Expression> otherNormalized;

        tf::Task normalizeTask = subflow.emplace([&other, &otherNormalized](tf::Subflow& sbf) {
            otherNormalized = other.Generalize(sbf);
        });

        tf::Task mostSigOpTask = subflow.emplace([&subtract, &otherNormalized](tf::Subflow& sbf) {
            const auto& otherBinaryExpression = dynamic_cast<const Subtract<Expression>&>(*otherNormalized);
            if (otherBinaryExpression.HasMostSigOp()) {
                subtract.SetMostSigOp(*MinuendT::Specialize(otherBinaryExpression.GetMostSigOp(), sbf));
            }
        });

        mostSigOpTask.succeed(normalizeTask);

        tf::Task leastSigOpTask = subflow.emplace([&subtract, &otherNormalized](tf::Subflow& sbf) {
            const auto& otherBinaryExpression = dynamic_cast<const Subtract<Expression>&>(*otherNormalized);
            if (otherBinaryExpression.HasLeastSigOp()) {
                subtract.SetLeastSigOp(*SubtrahendT::Specialize(otherBinaryExpression.GetLeastSigOp(), sbf));
            }
        });

        leastSigOpTask.succeed(normalizeTask);

        subflow.join();

        return std::make_unique<Subtract<MinuendT, SubtrahendT>>(subtract);
    }

    auto operator=(const Subtract& other) -> Subtract& = default;

    EXPRESSION_TYPE(Subtract)
    EXPRESSION_CATEGORY(0)
};

} // Oasis

#endif // OASIS_SUBTRACT_HPP

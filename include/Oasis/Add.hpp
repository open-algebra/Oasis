//
// Created by Matthew McCall on 7/2/23.
//

#ifndef OASIS_ADD_HPP
#define OASIS_ADD_HPP

#include "fmt/core.h"

#include "BinaryExpression.hpp"
#include "Real.hpp"

namespace Oasis {

template <IExpression FirstAddendT, IExpression SecondAddendT>
class Add;

template <>
class Add<Expression, Expression> : public BinaryExpression<Add<Expression, Expression>> {
public:
    Add() = default;
    Add(const Add<Expression, Expression>& other) = default;

    Add(const Expression& addend1, const Expression& addend2);

    [[nodiscard]] auto Generalize() const -> std::unique_ptr<Expression> final;
    auto Generalize(tf::Subflow& subflow) const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> final;
    auto Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto ToString() const -> std::string final;

    static auto Specialize(const Expression& other) -> std::unique_ptr<Add>;
    static auto Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Add>;

    EXPRESSION_TYPE(Add)
    EXPRESSION_CATEGORY(Expression::Associative | Expression::Commutative)
};

template <IExpression FirstAddendT = Expression, IExpression SecondAddendT = FirstAddendT>
class Add : public BinaryExpression<Add<FirstAddendT, SecondAddendT>, FirstAddendT, SecondAddendT> {
public:
    Add() = default;
    Add(const Add<FirstAddendT, SecondAddendT>& other)
        : BinaryExpression<Add<FirstAddendT, SecondAddendT>, FirstAddendT, SecondAddendT>(other)
    { }

    Add(const FirstAddendT& addend1, const SecondAddendT& addend2)
        : BinaryExpression<Add<FirstAddendT, SecondAddendT>, FirstAddendT, SecondAddendT>(addend1, addend2)
    { }

    [[nodiscard]] auto Generalize() const -> std::unique_ptr<Expression> final
    {
        Add<Expression> normalizedAdd;

        if (this->mostSigOp) {
            normalizedAdd.SetMostSigOp(*this->mostSigOp->Copy());
        }

        if (this->leastSigOp) {
            normalizedAdd.SetLeastSigOp(*this->leastSigOp->Copy());
        }

        return std::make_unique<Add<Expression>>(normalizedAdd);
    }

    auto Generalize(tf::Subflow& subflow) const -> std::unique_ptr<Expression> final
    {
        Add<Expression> normalizedAdd;

        if (this->mostSigOp) {
            subflow.emplace([this, &normalizedAdd](tf::Subflow& sbf) {
                normalizedAdd.SetMostSigOp(*this->mostSigOp->Copy(sbf));
            });
        }

        if (this->leastSigOp) {
            subflow.emplace([this, &normalizedAdd](tf::Subflow& sbf) {
                normalizedAdd.SetLeastSigOp(*this->leastSigOp->Copy(sbf));
            });
        }

        subflow.join();

        return std::make_unique<Add<Expression>>(normalizedAdd);
    }

    [[nodiscard]] auto ToString() const -> std::string final
    {
        return fmt::format("({} + {})", this->mostSigOp->ToString(), this->leastSigOp->ToString());
    }

    static auto Specialize(const Expression& other) -> std::unique_ptr<Add<FirstAddendT, SecondAddendT>>
    {
        Add<FirstAddendT, SecondAddendT> add;

        if (!add.StructurallyEquivalent(other)) {
            return nullptr;
        }

        std::unique_ptr<Expression> otherNormalized = other.Generalize();
        const auto& otherBinaryExpression = dynamic_cast<const Add<Expression>&>(*otherNormalized);

        if (otherBinaryExpression.HasMostSigOp()) {
            add.SetMostSigOp(*FirstAddendT::Specialize(otherBinaryExpression.GetMostSigOp()));
        }

        if (otherBinaryExpression.HasLeastSigOp()) {
            add.SetLeastSigOp(*SecondAddendT::Specialize(otherBinaryExpression.GetLeastSigOp()));
        }

        return std::make_unique<Add<FirstAddendT, SecondAddendT>>(add);
    }

    static auto Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Add<FirstAddendT, SecondAddendT>>
    {
        Add<FirstAddendT, SecondAddendT> add;

        if (!add.StructurallyEquivalent(other)) {
            return nullptr;
        }

        std::unique_ptr<Expression> otherNormalized;

        tf::Task generalizeTask = subflow.emplace([&other, &otherNormalized](tf::Subflow& sbf) {
            otherNormalized = other.Generalize(sbf);
        });

        tf::Task mostSigOpTask = subflow.emplace([&add, &otherNormalized](tf::Subflow& sbf) {
            const auto& otherBinaryExpression = dynamic_cast<const Add<Expression>&>(*otherNormalized);
            if (otherBinaryExpression.HasMostSigOp()) {
                add.SetMostSigOp(*FirstAddendT::Specialize(otherBinaryExpression.GetMostSigOp(), sbf));
            }
        });

        mostSigOpTask.succeed(generalizeTask);

        tf::Task leastSigOpTask = subflow.emplace([&add, &otherNormalized](tf::Subflow& sbf) {
            const auto& otherBinaryExpression = dynamic_cast<const Add<Expression>&>(*otherNormalized);
            if (otherBinaryExpression.HasLeastSigOp()) {
                add.SetLeastSigOp(*SecondAddendT::Specialize(otherBinaryExpression.GetLeastSigOp(), sbf));
            }
        });

        leastSigOpTask.succeed(generalizeTask);

        subflow.join();

        return std::make_unique<Add<FirstAddendT, SecondAddendT>>(add);
    }

    auto operator=(const Add& other) -> Add& = default;

    EXPRESSION_TYPE(Add)
    EXPRESSION_CATEGORY(Expression::Associative | Expression::Commutative)
};

} // namespace Oasis

#endif // OASIS_ADD_HPP

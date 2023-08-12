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
class Divide<Expression, Expression> : public BinaryExpression<Divide<Expression, Expression>> {
public:
    Divide() = default;
    Divide(const Divide<Expression, Expression>& other) = default;

    Divide(const Expression& dividend, const Expression& divisor);

    [[nodiscard]] auto Generalize() const -> std::unique_ptr<Expression> final;
    auto Generalize(tf::Subflow& subflow) const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> final;
    auto Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto ToString() const -> std::string final;

    static auto Specialize(const Expression& other) -> std::unique_ptr<Divide>;
    static auto Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Divide>;

    EXPRESSION_TYPE(Divide)
    EXPRESSION_CATEGORY(Associative | Commutative)
};

template <IExpression DividendT = Expression, IExpression DivisorT = DividendT>
class Divide : public BinaryExpression<Divide<DividendT, DivisorT>, DividendT, DivisorT> {
public:
    Divide() = default;
    Divide(const Divide<DividendT, DivisorT>& other)
        : BinaryExpression<Divide<DividendT, DivisorT>, DividendT, DivisorT>(other)
    { }

    Divide(const DividendT& addend1, const DivisorT& addend2)
        : BinaryExpression<Divide<DividendT, DivisorT>, DividendT, DivisorT>(addend1, addend2)
    { }

    [[nodiscard]] auto Generalize() const -> std::unique_ptr<Expression> final
    {
        Divide<Expression> generalizedAdd;

        if (this->mostSigOp) {
            generalizedAdd.SetMostSigOp(*this->mostSigOp->Copy());
        }

        if (this->leastSigOp) {
            generalizedAdd.SetLeastSigOp(*this->leastSigOp->Copy());
        }

        return std::make_unique<Divide<Expression>>(generalizedAdd);
    }

    auto Generalize(tf::Subflow& subflow) const -> std::unique_ptr<Expression> final
    {
        Divide<Expression> generalizedAdd;

        if (this->mostSigOp) {
            subflow.emplace([this, &generalizedAdd](tf::Subflow& sbf) {
                generalizedAdd.SetMostSigOp(*this->mostSigOp->Copy(sbf));
            });
        }

        if (this->leastSigOp) {
            subflow.emplace([this, &generalizedAdd](tf::Subflow& sbf) {
                generalizedAdd.SetLeastSigOp(*this->leastSigOp->Copy(sbf));
            });
        }

        subflow.join();

        return std::make_unique<Divide<Expression>>(generalizedAdd);
    }

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

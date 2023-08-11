//
// Created by Matthew McCall on 7/2/23.
//

#include "taskflow/taskflow.hpp"

#include "Oasis/Add.hpp"

namespace Oasis {

Add<Expression>::Add(const Expression& addend1, const Expression& addend2)
    : BinaryExpression(addend1, addend2)
{
}

auto Add<Expression>::Generalize() const -> std::unique_ptr<Expression>
{
    auto generalizedAdd = std::make_unique<Add>();

    if (mostSigOp) {
        generalizedAdd->SetMostSigOp(*mostSigOp->Copy());
    }

    if (leastSigOp) {
        generalizedAdd->SetLeastSigOp(*leastSigOp->Copy());
    }

    return generalizedAdd;
}

auto Add<Expression>::Generalize(tf::Subflow& subflow) const -> std::unique_ptr<Expression>
{
    auto generalizedAdd = std::make_unique<Add<>>();

    if (mostSigOp) {
        subflow.emplace([this, &generalizedAdd](tf::Subflow& sbf) {
            generalizedAdd->SetMostSigOp(*mostSigOp->Copy(sbf));
        });
    }

    if (leastSigOp) {
        subflow.emplace([this, &generalizedAdd](tf::Subflow& sbf) {
            generalizedAdd->SetLeastSigOp(*leastSigOp->Copy(sbf));
        });
    }

    subflow.join();

    return generalizedAdd;
}

auto Add<Expression>::Simplify() const -> std::unique_ptr<Expression>
{
    auto simplifiedAugend = mostSigOp ? mostSigOp->Simplify() : nullptr;
    auto simplifiedAddend = leastSigOp ? leastSigOp->Simplify() : nullptr;

    Add simplifiedAdd { *simplifiedAugend, *simplifiedAddend };

    if (auto realCase = Add<Real>::Specialize(simplifiedAdd); realCase != nullptr) {
        const Real& firstReal = realCase->GetMostSigOp();
        const Real& secondReal = realCase->GetLeastSigOp();

        return std::make_unique<Real>(firstReal.GetValue() + secondReal.GetValue());
    }

    return simplifiedAdd.Copy();
}

auto Add<Expression>::ToString() const -> std::string
{
    return fmt::format("({} + {})", mostSigOp->ToString(), leastSigOp->ToString());
}

auto Add<Expression>::Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression>
{
    std::unique_ptr<Expression> simplifiedAugend, simplifiedAddend;

    tf::Task leftSimplifyTask = subflow.emplace([this, &simplifiedAugend](tf::Subflow& sbf) {
        simplifiedAugend = mostSigOp ? mostSigOp->Simplify(sbf) : nullptr;
    });

    tf::Task rightSimplifyTask = subflow.emplace([this, &simplifiedAddend](tf::Subflow& sbf) {
        simplifiedAddend = leastSigOp ? leastSigOp->Simplify(sbf) : nullptr;
    });

    Add simplifiedAdd;

    // While this task isn't actually parallelized, it exists as a prerequisite for check possible cases in parallel
    tf::Task simplifyTask = subflow.emplace([&simplifiedAdd, &simplifiedAugend, &simplifiedAddend](tf::Subflow& sbf) {
        simplifiedAdd = Add { *simplifiedAugend, *simplifiedAddend };
    });

    simplifyTask.succeed(leftSimplifyTask, rightSimplifyTask);

    std::unique_ptr<Add<Real>> realCase;

    tf::Task realCaseTask = subflow.emplace([&simplifiedAdd, &realCase](tf::Subflow& sbf) {
        realCase = Add<Real>::Specialize(simplifiedAdd, sbf);
    });

    simplifyTask.precede(realCaseTask);

    subflow.join();

    if (realCase) {
        const Real& firstReal = realCase->GetMostSigOp();
        const Real& secondReal = realCase->GetLeastSigOp();

        return std::make_unique<Real>(firstReal.GetValue() + secondReal.GetValue());
    }

    return simplifiedAdd.Copy();
}

auto Add<Expression>::Specialize(const Expression& other) -> std::unique_ptr<Add<Expression, Expression>>
{
    if (!other.Is<Add>()) {
        return nullptr;
    }

    Add add;

    const std::unique_ptr<Expression> otherGeneralized = other.Generalize();
    const auto& otherAdd = dynamic_cast<const Add<Expression>&>(*otherGeneralized);

    if (otherAdd.mostSigOp) {
        add.SetMostSigOp(otherAdd.GetMostSigOp());
    }

    if (otherAdd.leastSigOp) {
        add.SetLeastSigOp(otherAdd.GetLeastSigOp());
    }

    return std::make_unique<Add>(add);
}

auto Add<Expression>::Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Add>
{
    if (!other.Is<Add>()) {
        return nullptr;
    }

    Add add;

    std::unique_ptr<Expression> otherGeneralized;

    tf::Task generalizeTask = subflow.emplace([&other, &otherGeneralized](tf::Subflow& sbf) {
        otherGeneralized = other.Generalize(sbf);
    });

    tf::Task mostSigOpTask = subflow.emplace([&add, &otherGeneralized](tf::Subflow& sbf) {
        const auto& otherBinaryExpression = dynamic_cast<const Add<Expression>&>(*otherGeneralized);
        if (otherBinaryExpression.HasMostSigOp()) {
            add.SetMostSigOp(otherBinaryExpression.GetMostSigOp());
        }
    });

    mostSigOpTask.succeed(generalizeTask);

    tf::Task leastSigOpTask = subflow.emplace([&add, &otherGeneralized](tf::Subflow& sbf) {
        const auto& otherBinaryExpression = dynamic_cast<const Add<Expression>&>(*otherGeneralized);
        if (otherBinaryExpression.HasLeastSigOp()) {
            add.SetLeastSigOp(otherBinaryExpression.GetLeastSigOp());
        }
    });

    leastSigOpTask.succeed(generalizeTask);

    subflow.join();

    return std::make_unique<Add>(add);
}

} // Oasis
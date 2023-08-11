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
    auto normalizedAdd = std::make_unique<Add>();

    if (mostSigOp) {
        normalizedAdd->SetMostSigOp(*mostSigOp->Copy());
    }

    if (leastSigOp) {
        normalizedAdd->SetLeastSigOp(*leastSigOp->Copy());
    }

    return normalizedAdd;
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
    if (mostSigOp == nullptr || leastSigOp == nullptr) {
        return Copy();
    }

    auto simplifiedFirstAddend = mostSigOp->Simplify();
    auto simplifiedSecondAddend = leastSigOp->Simplify();

    Add simplifiedAdd { *simplifiedFirstAddend, *simplifiedSecondAddend };

    if (auto realCase = Add<Real>::Specialize(simplifiedAdd); realCase != nullptr) {
        const Real& firstReal = realCase->GetMostSigOp();
        const Real& secondReal = realCase->GetLeastSigOp();

        return std::make_unique<Real>(firstReal.GetValue() + secondReal.GetValue());
    }

    return Copy();
}

auto Add<Expression>::ToString() const -> std::string
{
    return fmt::format("({} + {})", mostSigOp->ToString(), leastSigOp->ToString());
}

auto Add<Expression>::Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression>
{
    if (mostSigOp == nullptr || leastSigOp == nullptr) {
        return Copy();
    }

    std::unique_ptr<Expression> simplifiedFirstAddend, simplifiedSecondAddend;

    tf::Task leftSimplifyTask = subflow.emplace([this, &simplifiedFirstAddend](tf::Subflow& sbf) {
        assert(simplifiedFirstAddend == nullptr);
        simplifiedFirstAddend = mostSigOp->Simplify(sbf);
    });

    tf::Task rightSimplifyTask = subflow.emplace([this, &simplifiedSecondAddend](tf::Subflow& sbf) {
        assert(simplifiedSecondAddend == nullptr);
        simplifiedSecondAddend = leastSigOp->Simplify(sbf);
    });

    Add simplifiedAdd;

    // While this task isn't actually parallelized, it exists as a prerequisite for check possible cases in parallel
    tf::Task simplifyTask = subflow.emplace([&simplifiedAdd, &simplifiedFirstAddend, &simplifiedSecondAddend](tf::Subflow& sbf) {
        simplifiedAdd = Add { *simplifiedFirstAddend, *simplifiedSecondAddend };
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

    return Copy();
}

auto Add<Expression>::Specialize(const Expression& other) -> std::unique_ptr<Add<Expression, Expression>>
{
    auto add = std::make_unique<Add<Expression, Expression>>();

    if (!add->StructurallyEquivalent(other)) {
        return nullptr;
    }

    const std::unique_ptr<Expression> otherNormalized = other.Generalize();
    const auto& otherAdd = dynamic_cast<const Add<Expression>&>(*otherNormalized);

    if (otherAdd.mostSigOp) {
        add->SetMostSigOp(otherAdd.GetMostSigOp());
    }

    if (otherAdd.leastSigOp) {
        add->SetLeastSigOp(otherAdd.GetLeastSigOp());
    }

    return add;
}

auto Add<Expression>::Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Add>
{
    Add add;

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
            add.SetMostSigOp(otherBinaryExpression.GetMostSigOp());
        }
    });

    mostSigOpTask.succeed(generalizeTask);

    tf::Task leastSigOpTask = subflow.emplace([&add, &otherNormalized](tf::Subflow& sbf) {
        const auto& otherBinaryExpression = dynamic_cast<const Add<Expression>&>(*otherNormalized);
        if (otherBinaryExpression.HasLeastSigOp()) {
            add.SetLeastSigOp(otherBinaryExpression.GetLeastSigOp());
        }
    });

    leastSigOpTask.succeed(generalizeTask);

    subflow.join();

    return std::make_unique<Add<Expression>>(add);
}

} // Oasis
//
// Created by Matthew McCall on 10/14/24.
//

#ifndef OASIS_RECURSIVECAST_HPP
#define OASIS_RECURSIVECAST_HPP

#include "Oasis/Add.hpp"
#include "Oasis/BinaryExpression.hpp"

namespace Oasis {

// Primary template
template<typename T>
struct original_template;

// Specialization for MyTemplate
template <template <typename, typename > class Template, typename T, typename U>
struct original_template<Template<T, U>> {
    template <typename V, typename W>
    using type = Template<V, W>;
};

template <template <typename, typename> typename T>
concept DerivedFromBinaryExpression = std::derived_from<T<Expression, Expression>, BinaryExpression<T>>;

template <typename SpecializedT, typename MostSigOpT, typename LeastSigOpT>
concept DerivedFromSpecializedBinaryExpression = std::derived_from<SpecializedT, BinaryExpression<original_template<SpecializedT>::template type, MostSigOpT, LeastSigOpT>> && IExpression<MostSigOpT> && IExpression<LeastSigOpT>;

template <IExpression DerivedT, IExpression MostSigOpT, IExpression LeastSigOpT> // requires DerivedFromSpecializedBinaryExpression<DerivedT, MostSigOpT, LeastSigOpT>
auto RecursiveCast(const Expression& other) -> std::unique_ptr<DerivedT>
{
    if (!other.Is<DerivedT>()) {
        return nullptr;
    }
    auto specialized = std::make_unique<DerivedT>();

    const std::unique_ptr<Expression> otherGeneralized = other.Generalize();
    const auto& otherBinaryExpression = static_cast<const DerivedT&>(*otherGeneralized);

    bool leftOperandSpecialized = true, rightOperandSpecialized = true;

    if (otherBinaryExpression.HasMostSigOp()) {
        specialized->SetMostSigOp(RecursiveCast<MostSigOpT>(otherBinaryExpression.GetMostSigOp()));
        leftOperandSpecialized = specialized->HasMostSigOp();
    }

    if (otherBinaryExpression.HasLeastSigOp()) {
        specialized->SetLeastSigOp(RecursiveCast<LeastSigOpT>(otherBinaryExpression.GetLeastSigOp()));
        rightOperandSpecialized = specialized->HasLeastSigOp();
    }

    if (leftOperandSpecialized && rightOperandSpecialized) {
        return specialized;
    }

    if (!(other.GetCategory() & Commutative)) {
        return nullptr;
    }

    leftOperandSpecialized = true, rightOperandSpecialized = true;
    specialized = std::make_unique<DerivedT>();

    auto otherWithSwappedOps
        = otherBinaryExpression.SwapOperands();
    if (otherWithSwappedOps.HasMostSigOp()) {
        specialized->SetMostSigOp(RecursiveCast<MostSigOpT>(otherWithSwappedOps.GetMostSigOp()));
        leftOperandSpecialized = specialized->HasMostSigOp();
    }

    if (otherWithSwappedOps.HasLeastSigOp()) {
        specialized->SetLeastSigOp(RecursiveCast<LeastSigOpT>(otherWithSwappedOps.GetLeastSigOp()));
        rightOperandSpecialized = specialized->HasLeastSigOp();
    }

    if (leftOperandSpecialized && rightOperandSpecialized) {
        return specialized;
    }

    return nullptr;
}

template <IExpression T>
auto RecursiveCast(const T& other) -> std::unique_ptr<T>
{
    return other.template Is<T>() ? std::make_unique<T>() : nullptr;
}

}

#endif // OASIS_RECURSIVECAST_HPP

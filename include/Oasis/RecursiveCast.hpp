//
// Created by Matthew McCall on 10/14/24.
//

#ifndef OASIS_RECURSIVECAST_HPP
#define OASIS_RECURSIVECAST_HPP

#include "Expression.hpp"

namespace Oasis {

template <template <IExpression, IExpression> class DerivedT, IExpression MostSigOpT, IExpression LeastSigOpT>
class BinaryExpression;

template<class Derived>
concept DerivedFromBinaryExpression = requires (Derived& d) {
    []<template <typename, typename> typename D, typename T, typename U>(BinaryExpression<D, T, U>&) {}(d);
};

template <IExpression T> requires DerivedFromBinaryExpression<T>
auto RecursiveCast(const Expression& other) -> std::unique_ptr<T>
{
    T dummy;
    return [&other]<template <typename, typename> typename DerivedT, typename MostSigOpT, typename LeastSigOpT>(BinaryExpression<DerivedT, MostSigOpT, LeastSigOpT>&) -> std::unique_ptr<T> {
        if (!other.Is<DerivedT>()) {
            return nullptr;
        }

        const std::unique_ptr<Expression> otherGeneralized = other.Generalize();
        const auto& otherBinaryExpression = static_cast<const DerivedT<Expression, Expression>&>(*otherGeneralized);

        auto specializedMostSigOp = RecursiveCast<MostSigOpT>(otherBinaryExpression.GetMostSigOp());
        auto specializedLeastSigOp = RecursiveCast<LeastSigOpT>(otherBinaryExpression.GetLeastSigOp());

        if (specializedMostSigOp && specializedLeastSigOp) {
            return std::make_unique<T>(*specializedMostSigOp, *specializedLeastSigOp);
        }

        if (!(other.GetCategory() & Commutative)) {
            return nullptr;
        }

        auto otherWithSwappedOps
            = otherBinaryExpression.SwapOperands();

        specializedMostSigOp = RecursiveCast<MostSigOpT>(otherWithSwappedOps.GetMostSigOp());
        specializedLeastSigOp = RecursiveCast<LeastSigOpT>(otherWithSwappedOps.GetLeastSigOp());

        if (specializedMostSigOp && specializedLeastSigOp) {
            return std::make_unique<T>(*specializedMostSigOp, *specializedLeastSigOp);
        }

        return nullptr;
    }(dummy);
}

template <IExpression T>
auto RecursiveCast(const Expression& other) -> std::unique_ptr<T>
{
    if constexpr (std::is_same_v<T, Expression>)
        return other.Copy();
    else
        return other.Is<T>() ? std::make_unique<T>(dynamic_cast<const T&>(other)) : nullptr;
}

}

#endif // OASIS_RECURSIVECAST_HPP

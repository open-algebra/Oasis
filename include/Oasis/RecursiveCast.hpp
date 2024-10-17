//
// Created by Matthew McCall on 10/14/24.
//

#ifndef OASIS_RECURSIVECAST_HPP
#define OASIS_RECURSIVECAST_HPP

#include "Oasis/Add.hpp"
#include "Oasis/BinaryExpression.hpp"

namespace Oasis {

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

        auto specialized = std::make_unique<T>();

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
        specialized = std::make_unique<T>();

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
    }(dummy);
}

template <IExpression T>
auto RecursiveCast(const Expression& other) -> std::unique_ptr<T>
{
    return other.Is<T>() ? std::make_unique<T>(dynamic_cast<const T&>(other)) : nullptr;
}

}

#endif // OASIS_RECURSIVECAST_HPP

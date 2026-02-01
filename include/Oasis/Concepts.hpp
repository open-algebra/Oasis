//
// Created by Matthew McCall on 10/20/24.
//

#ifndef OASIS_CONCEPTS_HPP
#define OASIS_CONCEPTS_HPP

#include <expected>

namespace Oasis {

class Expression;
class Visitor;
enum class ExpressionType;

/**
 * An expression concept.
 * @note This class is not intended to be used directly by end users.
 *
 * An expression concept is a type that satisfies the following requirements:
 * - It is derived from `Expression`.
 * - It has a static `Specialize` function that returns a `std::unique_ptr<T>` and takes a `const Expression&` as an argument.
 * - It has a static `Specialize` function that returns a `std::unique_ptr<T>` and takes a `const Expression&` and a `tf::Subflow&` as arguments.
 * - It has a static `GetStaticCategory` function that returns a `uint32_t`.
 * - It has a static `GetStaticType` function that returns an `ExpressionType`.
 *
 * @tparam T The type to check.
 */
template <typename T>
concept IExpression = (requires(T, const Expression& other) {
    {
        T::GetStaticCategory()
    } -> std::same_as<uint32_t>;
    {
        T::GetStaticType()
    } -> std::same_as<ExpressionType>;
} && std::derived_from<T, Expression>) || std::is_same_v<T, Expression>;

template <template <IExpression, IExpression> class DerivedT, IExpression MostSigOpT, IExpression LeastSigOpT>
class BinaryExpression;

template <template <IExpression> class DerivedT, IExpression OpT>
class UnaryExpression;

/**
 * Checks if type T is same as any of the provided types in U.
 *
 * @tparam T The type to compare against.
 * @tparam U The comparision types.
 * @return true if T is same as any type in U, false otherwise.
 */
template <typename T, typename... U>
concept IsAnyOf = (std::same_as<T, U> || ...);

template <typename Derived>
concept DerivedFromBinaryExpression = requires(Derived& d) {
    []<template <typename, typename> typename D, IExpression T, IExpression U>(BinaryExpression<D, T, U>&) { }(d);
};

template <typename Derived>
concept DerivedFromUnaryExpression = requires(Derived& d) {
    []<template <typename> typename D, IExpression T>(UnaryExpression<D, T>&) { }(d);
};

template <typename T>
concept IVisitor = requires {
    typename T::RetT; // Checks for the type alias
    requires std::is_base_of_v<Visitor, T>; // Ensures T derives from BaseClass
};

template <typename T>
concept ExpectedWithString = requires {
    typename T::unexpected_type;
    requires std::same_as<typename T::unexpected_type, std::unexpected<std::string>>;
};

}

#endif // OASIS_CONCEPTS_HPP

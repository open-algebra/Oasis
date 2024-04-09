#ifndef OASIS_EXPRESSION_HPP
#define OASIS_EXPRESSION_HPP

#include <concepts>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace tf {
class Subflow;
}

namespace Oasis {

class Expression;

/**
 * The type of an expression.
 */
enum class ExpressionType {
    None,
    Real,
    Imaginary,
    Variable,
    Add,
    Subtract,
    Multiply,
    Divide,
    Exponent,
    Log,
    Negate,
    Sqrt,
};

/**
 * The category of an expression.
 */
enum ExpressionCategory : uint32_t {
    None = 0,
    Associative = 1,
    Commutative = 1 << 1,
};

// clang-format off
/**
 * An expression concept.
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
concept IExpression = (requires(T, const Expression& other, tf::Subflow& subflow) {
    { T::Specialize(other) } -> std::same_as<std::unique_ptr<T>>;
    { T::Specialize(other, subflow) } -> std::same_as<std::unique_ptr<T>>;
    { T::GetStaticCategory() } -> std::same_as<uint32_t>;
    { T::GetStaticType() } -> std::same_as<ExpressionType>;
} && std::derived_from<T, Expression>) || std::is_same_v<T, Expression>;
// clang-format on

/**
 * An expression.
 *
 * Expressions are a tree-like structure that represent mathematical expressions. They can be
 * evaluated, simplified, and compared for equality. They are immutable, and all operations on them
 * return a new expression.
 */
class Expression {
public:
    /**
     * Copies this expression.
     * @return A copy of this expression.
     */
    [[nodiscard]] virtual auto Copy() const -> std::unique_ptr<Expression> = 0;

    /**
     * Copies this expression.
     * @param subflow The invoking subflow.
     * @return A copy of this expression.
     */
    virtual auto Copy(tf::Subflow& subflow) const -> std::unique_ptr<Expression> = 0;

    /**
     * Compares this expression to another expression for equality.
     *
     * Two expressions are equal if they are structurally equivalent and have the same value.
     * While this method considers the associativity and commutativity of expressions, it does not
     * simplify the expressions before comparing them. For example, `Add<Real>(Real(1), Real(2))`
     * and `Add<Real>(Real(2), Real(1))` are not equal, despite being structurally equivalent.
     *
     * @param other The other expression.
     * @return Whether the two expressions are equal.
     */
    [[nodiscard]] virtual auto Equals(const Expression& other) const -> bool = 0;

    /**
     * The FindZeros function finds all rational real zeros, and up to 4 irrational/complex zeros of a polynomial. Currently assumes an expression of the form a+bx+cx^2+dx^3+... where a, b, c, d are a integers.
     *
     * @return a vector of Expressions giving the zeros.
     */
    auto FindZeros() const -> std::vector<std::unique_ptr<Expression>>;

    /**
     * Gets the category of this expression.
     * @return The category of this expression.
     */
    [[nodiscard]] virtual auto GetCategory() const -> uint32_t;

    /**
     * Gets the type of this expression.
     * @return The type of this expression.
     */
    [[nodiscard]] virtual auto GetType() const -> ExpressionType;

    /**
     * Converts this expression to a more general expression.
     *
     * Some expressions may explicitly specify the type of their operands. For example, a
     * `Divide<Real>` expression may only accept `Real` operands. This function converts the
     * expression to a more general expression, such as `Divide<Expression>`, which accepts any
     * expression as an operand.
     *
     * @return The generalized expression.
     */
    [[nodiscard]] virtual auto Generalize() const -> std::unique_ptr<Expression>;

    /**
     * Converts this expression to a more general expression asynchronously.
     *
     * Some expressions may explicitly specify the type of their operands. For example, a
     * `Divide<Real>` expression may only accept `Real` operands. This function converts the
     * expression to a more general expression, such as `Divide<Expression>`, which accepts any
     * expression as an operand.
     *
     * @param subflow The invoking subflow.
     * @return The generalized expression.
     */
    virtual auto Generalize(tf::Subflow& subflow) const -> std::unique_ptr<Expression>;

    /**
     * Attempts to specialize this expression to a more specific expression.
     *
     * Some expressions may explicitly specify the type of their operands. For example, a
     * `Divide<Real>` expression may only accept `Real` operands. This function attempts to
     * specialize the expression to a more specific expression, such as `Divide<Real>`, which
     * accepts only `Real` operands. If the expression cannot be specialized, this function returns
     * `nullptr`.
     *
     * @param other The other expression to specialize against.
     * @return The specialized expression, or `nullptr` if the expression cannot be specialized.
     */
    static auto Specialize(const Expression& other) -> std::unique_ptr<Expression>;

    /**
     * Attempts to specialize this expression to a more specific expression asynchronously.
     *
     * Some expressions may explicitly specify the type of their operands. For example, a
     * `Divide<Real>` expression may only accept `Real` operands. This function attempts to
     * specialize the expression to a more specific expression, such as `Divide<Real>`, which
     * accepts only `Real` operands. If the expression cannot be specialized, this function returns
     * `nullptr`.
     *
     * @param other The other expression to specialize against.
     * @param subflow The invoking subflow.
     * @return The specialized expression, or `nullptr` if the expression cannot be specialized.
     */
    static auto Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Expression>;

    /**
     * Gets whether this expression is of a specific type.
     *
     * @tparam T The type to check against.
     * @return true if this expression is of type T, false otherwise.
     */
    template <IExpression T>
    [[nodiscard]] bool Is() const
    {
        return GetType() == T::GetStaticType();
    }

    template <template <typename, typename> typename T>
    [[nodiscard]] bool Is() const
    {
        return GetType() == T<Expression, Expression>::GetStaticType();
    }

    /**
     * Simplifies this expression.
     * @return The simplified expression.
     */
    [[nodiscard]] virtual auto Simplify() const -> std::unique_ptr<Expression>;

    /**
     * Simplifies this expression asynchronously.
     *
     * @note You probably want to use `SimplifyAsync` instead. This is an internal function that
     *       should only be used by the expression simplification system.
     * @param subflow The invoking subflow.
     * @return The simplified expression.
     */
    virtual auto Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression>;

    /**
     * Simplifies this expression asynchronously.
     * @return The simplified expression.
     */
    [[nodiscard]] auto SimplifyAsync() const -> std::unique_ptr<Expression>;

    /**
     * Checks whether this expression is structurally equivalent to another expression.
     *
     * Two expressions are structurally equivalent if the share the same tree structure. For
     * example, `Add<Real>(Real(1), Real(2))` and `Add<Real>(Real(2), Real(1))` are structurally equivalent
     * despite having different values.
     *
     * @param other The other expression.
     * @return Whether the two expressions are structurally equivalent.
     */
    [[nodiscard]] virtual auto StructurallyEquivalent(const Expression& other) const -> bool = 0;

    /**
     * Checks whether this expression is structurally equivalent to another expression asynchronously.
     *
     * Two expressions are structurally equivalent if the share the same tree structure. For
     * example, `Add<Real>(Real(1), Real(2))` and `Add<Real>(Real(2), Real(1))` are structurally equivalent
     * despite having different values.
     *
     * @note You probably want to use `StructurallyEquivalentAsync` instead. This is an internal function that
     *       should only be used by `StructurallyEquivalentAsync`.
     *
     * @param other The other expression.
     * @param subflow The invoking subflow.
     * @return Whether the two expressions are structurally equivalent.
     */
    virtual auto StructurallyEquivalent(const Expression& other, tf::Subflow& subflow) const -> bool = 0;

    /**
     * Checks whether this expression is structurally equivalent to another expression asynchronously.
     *
     * Two expressions are structurally equivalent if the share the same tree structure. For
     * example, `Add<Real>(Real(1), Real(2))` and `Add<Real>(Real(2), Real(1))` are structurally equivalent
     * despite having different values.
     *
     * @param other The other expression.
     * @return Whether the two expressions are structurally equivalent.
     */
    [[nodiscard]] auto StructurallyEquivalentAsync(const Expression& other) const -> bool;

    /**
     * The SubstituteVariable function substitues every instantce of a given variable with a given expression.
     *
     * @tparam var The variable to substitute.
     * @tparam exp The Expression to subsitute the varible with.
     * @return The expression with the variable subsituted with the given expression.
     */
    [[nodiscard]] virtual auto SubstituteVariable(const Expression& var, const Expression& exp) const -> std::unique_ptr<Expression> = 0;

    /**
     * Converts this expression to a string.
     * @return The string representation of this expression.
     */
    [[nodiscard]] virtual std::string ToString() const = 0;

    virtual ~Expression() = default;
};

#define EXPRESSION_TYPE(type)                       \
    auto GetType() const -> ExpressionType override \
    {                                               \
        return ExpressionType::type;                \
    }                                               \
                                                    \
    static auto GetStaticType() -> ExpressionType   \
    {                                               \
        return ExpressionType::type;                \
    }

#define EXPRESSION_CATEGORY(category)                     \
    auto GetCategory() const -> uint32_t override         \
    {                                                     \
        return category;                                  \
    }                                                     \
                                                          \
    constexpr static auto GetStaticCategory() -> uint32_t \
    {                                                     \
        return category;                                  \
    }

#define DECL_SPECIALIZE(type)                                                 \
    static auto Specialize(const Expression& other) -> std::unique_ptr<type>; \
    static auto Specialize(const Expression& other, tf::Subflow&) -> std::unique_ptr<type>;

} // namespace Oasis

std::unique_ptr<Oasis::Expression> operator+(const std::unique_ptr<Oasis::Expression>& lhs, const std::unique_ptr<Oasis::Expression>& rhs);
std::unique_ptr<Oasis::Expression> operator-(const std::unique_ptr<Oasis::Expression>& lhs, const std::unique_ptr<Oasis::Expression>& rhs);
std::unique_ptr<Oasis::Expression> operator*(const std::unique_ptr<Oasis::Expression>& lhs, const std::unique_ptr<Oasis::Expression>& rhs);
std::unique_ptr<Oasis::Expression> operator/(const std::unique_ptr<Oasis::Expression>& lhs, const std::unique_ptr<Oasis::Expression>& rhs);

#endif // OASIS_EXPRESSION_HPP
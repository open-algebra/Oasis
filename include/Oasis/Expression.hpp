#ifndef OASIS_EXPRESSION_HPP
#define OASIS_EXPRESSION_HPP

#include <concepts>
#include <memory>

namespace tf {
class Subflow;
}

namespace Oasis {

class Expression {
public:
    enum class Type {
        None,
        Real,
        Add
    };

    enum Category : uint32_t {
        Associative = 1,
        Commutative = 1 << 1,
    };

    [[nodiscard]] virtual auto Copy() const -> std::unique_ptr<Expression> = 0;
    virtual auto Copy(tf::Subflow& subflow) const -> std::unique_ptr<Expression> = 0;

    [[nodiscard]] virtual auto GetCategory() const -> uint32_t;
    [[nodiscard]] virtual auto GetType() const -> Type;

    [[nodiscard]] virtual auto Generalize() const -> std::unique_ptr<Expression>;
    virtual auto Generalize(tf::Subflow& subflow) const -> std::unique_ptr<Expression>;

    [[nodiscard]] virtual auto Simplify() const -> std::unique_ptr<Expression>;
    virtual auto Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression>;
    [[nodiscard]] auto SimplifyAsync() const -> std::unique_ptr<Expression>;

    [[nodiscard]] virtual auto StructurallyEquivalent(const Expression& other) const -> bool = 0;
    virtual auto StructurallyEquivalent(const Expression& other, tf::Subflow& subflow) const -> bool = 0;
    [[nodiscard]] auto StructurallyEquivalentAsync(const Expression& other) const -> bool;

    [[nodiscard]] virtual std::string ToString() const = 0;

    virtual ~Expression() = default;
};

// clang-format off
template <typename T>
concept IExpression = (requires(T, const Expression& other, tf::Subflow& subflow) {
    { T::Specialize(other) } -> std::same_as<std::unique_ptr<T>>;
    { T::Specialize(other, subflow) } -> std::same_as<std::unique_ptr<T>>;
    { T::GetStaticCategory() } -> std::same_as<uint32_t>;
    { T::GetStaticType() } -> std::same_as<Expression::Type>;
} && std::derived_from<T, Expression>) || std::is_same_v<T, Expression>;
// clang-format on

#define EXPRESSION_TYPE(type)                       \
    auto GetType() const->Expression::Type override \
    {                                               \
        return Expression::Type::type;              \
    }                                               \
                                                    \
    static auto GetStaticType()->Expression::Type   \
    {                                               \
        return Expression::Type::type;              \
    }

#define EXPRESSION_CATEGORY(category)           \
    auto GetCategory() const->uint32_t override \
    {                                           \
        return category;                        \
    }                                           \
                                                \
    static auto GetStaticCategory()->uint32_t   \
    {                                           \
        return category;                        \
    }

} // namespace Oasis

#endif // OASIS_EXPRESSION_HPP

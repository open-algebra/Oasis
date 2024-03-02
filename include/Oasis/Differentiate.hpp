//
// Created by bachia on 2/27/2024.
//

#ifndef OASIS_DIFFERENTIATE_HPP
#define OASIS_DIFFERENTIATE_HPP

#include "fmt/core.h"

#include "BinaryExpression.hpp"
#include "Real.hpp"

namespace Oasis {

    template <IExpression Exp, IExpression Var>
    class Differentiate;

/// @cond
    template <>
    class Differentiate<Expression, Expression> : public BinaryExpression<Differentiate> {
    public:
        Differentiate() = default;
        Differentiate(const Differentiate<Expression, Expression>& other) = default;

        Differentiate(const Expression& Exp, const Expression& Var);

        [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> final;
        auto Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> final;

        [[nodiscard]] auto ToString() const -> std::string final;

        static auto Specialize(const Expression& other) -> std::unique_ptr<Differentiate>;
        static auto Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Differentiate>;

        EXPRESSION_TYPE(Differentiate)
        EXPRESSION_CATEGORY(0)
    };
/// @endcond

/**
 * The Differentiate expression differentiates the expression given
 *
 * @tparam Exp The expression to be differentiated.
 * @tparam Var The variable to be differentiated on.
 */
    template <IExpression Exp = Expression, IExpression Var = Exp>
    class Differentiate : public BinaryExpression<Differentiate, Exp, Var> {
    public:
        Differentiate() = default;
        Differentiate(const Differentiate<Exp, Var>& other)
                : BinaryExpression<Differentiate, Exp, Var>(other)
        {
        }

        Differentiate(const Exp& exp, const Var& var)
                : BinaryExpression<Differentiate, Exp, Var>(exp, var)
        {
        }

        [[nodiscard]] auto ToString() const -> std::string final
        {
            return fmt::format("(d/d{}({}))", this->leastSigOp->ToString(), this->mostSigOp->ToString());
        }

        IMPL_SPECIALIZE(Differentiate, Exp, Var)

        auto operator=(const Differentiate& other) -> Differentiate& = default;

        EXPRESSION_TYPE(Add)
        EXPRESSION_CATEGORY(0)
    };

} // namespace Oasis

#endif //OASIS_DIFFERENTIATE_HPP

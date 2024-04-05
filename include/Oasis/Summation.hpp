//
// Created by Andrew Choi on 3/15/24
//
#ifndef OASIS_SUMMATION_HPP
#define OASIS_SUMMATION_HPP
#include "fmt/core.h"

#include "BinaryExpression.hpp"

namespace Oasis {

template <IExpression lowBound, IExpression upperBound, IExpression exp>
class Summation;

///@cond
template<>
class Summation<Expression Expression, Expression> : public BinaryExpression<Summation> {

    Summation() = default;

    Summation(const Summation<Expression, Expression, Expression>n& other)  = default;

    Summation(const Expression& lowBound, const Expression& upperBound, const Expression& exp);

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> final;
    auto Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto ToString() const -> std::string final;

    static auto Specialize(const Expression& other) -> std::unique_ptr<Summation>;
    static auto Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Summation>;

    EXPRESSION_TYPE(Summation)
    EXPRESSION_CATEGORY(None)

};
/// @endcond

/**
*The Summation expression represents the sum of an expression from the lower Bound to the upper Bound
*
*@tparam lowBound is the lowerBound for the summation
*@tparam upperBound is the upper Bound for the summation
*@tparam exp is the expression that is being summed
*/

template<IExpression lowBound = Expression, IExpression upperBound = Expression, IExpression exp = Expression>
class Summation : public BinaryExpression<Summation, lowBound, upperBound, exp> {
    Summation(const Summation<lowBound, upperBound, exp>& other) : 
    BinaryExpression<Summation, lowBound, upperBound, exp>(other) {};

    Summation(const Expression& lowBound, const Expression& upperBound, const Expression& exp) :
    BinaryExpression<Summation, lowBound, upperBound, exp>(lowBound, upperBound, exp) {};

    [[nodiscard]] auto ToString() const -> std::string final
    {
        return fmt::format("∑^{}_{}({})", this->lowBound, this->upperBound, this->exp);
    }

    auto operator=(const Summation& other) -> Summation& = default;

    EXPRESSION_TYPE(Summation);
    EXPRESSION_CATEGORY(None);
};

}
#endif
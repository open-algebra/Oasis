#ifndef OASIS_LINEAREQUATION_HPP
#define OASIS_LINEAREQUATION_HPP

#include "Expression.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Subtract.hpp"


namespace Oasis {

/**
*A linear equation.
*A linear equation is an equation with in the form y = mx + b
*/

template <typename MostSigOpT, typename LeastSigOpT, typename T>
concept IOperand = std::is_same_v<T, MostSigOpT> || std::is_same_v<T, LeastSigOpT>;

template <template <typename, typename> typename T>
concept IAssociativeAndCommutative = IExpression<T<Expression, Expression>> && requires { (T<Expression, Expression>::GetStaticCategory() & (Associative | Commutative)) != 0; };

template <typename T, typename... U>
concept IsAnyOf = (std::same_as<T, U> || ...);

class LinearEquation : public Expression {
    using DerivedSpecialized = DerivedT<MostSigOpT, LeastSigOpT>;
    using DerivedGeneralized = DerivedT<Expression, Expression>;
public: 

    LinearEquation() = default;
    LinearEquation(const LinearEquation& other)
    {
        if (other.HasMostSigOp()) {
            SetMostSigOp(other.GetMostSigOp());
        }

        if (other.HasLeastSigOp()) {
            SetLeastSigOp(other.GetLeastSigOp());
        }
    }


};






}; //Oasis

#endif

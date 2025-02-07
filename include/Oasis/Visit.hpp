//
// Created by Matthew McCall on 4/28/24.
//

#ifndef OASIS_SERIALIZATION_HPP
#define OASIS_SERIALIZATION_HPP

#include <any>

#include "Expression.hpp"

namespace Oasis {

class Real;
class Imaginary;
class Matrix;
class Variable;
class Undefined;
class EulerNumber;
class Pi;

template <IExpression, IExpression>
class Add;

template <IExpression MinuendT, IExpression SubtrahendT>
class Subtract;

template <IExpression MultiplicandT, IExpression MultiplierT>
class Multiply;

template <IExpression DividendT, IExpression DivisorT>
class Divide;

template <IExpression BaseT, IExpression PowerT>
class Exponent;

template <IExpression BaseT, IExpression ArgumentT>
class Log;

template <typename OperandT>
class Negate;

template <typename OperandT>
class Magnitude;

template <IExpression, IExpression>
class Derivative;

template <IExpression Integrand, IExpression Differential>
class Integral;

class Visitor {
public:
    virtual std::any Visit(const Real& real) = 0;
    virtual std::any Visit(const Imaginary& imaginary) = 0;
    virtual std::any Visit(const Matrix& matrix) = 0;
    virtual std::any Visit(const Variable& variable) = 0;
    virtual std::any Visit(const Undefined& undefined) = 0;
    virtual std::any Visit(const EulerNumber&) = 0;
    virtual std::any Visit(const Pi&) = 0;
    virtual std::any Visit(const Add<Expression, Expression>& add) = 0;
    virtual std::any Visit(const Subtract<Expression, Expression>& subtract) = 0;
    virtual std::any Visit(const Multiply<Expression, Expression>& multiply) = 0;
    virtual std::any Visit(const Divide<Expression, Expression>& divide) = 0;
    virtual std::any Visit(const Exponent<Expression, Expression>& exponent) = 0;
    virtual std::any Visit(const Log<Expression, Expression>& log) = 0;
    virtual std::any Visit(const Negate<Expression>& negate) = 0;
    virtual std::any Visit(const Magnitude<Expression>& magnitude) = 0;
    virtual std::any Visit(const Derivative<Expression, Expression>& derivative) = 0;
    virtual std::any Visit(const Integral<Expression, Expression>& integral) = 0;

    virtual ~Visitor() = default;
};

}

#endif // OASIS_SERIALIZATION_HPP

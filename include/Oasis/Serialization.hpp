//
// Created by Matthew McCall on 4/28/24.
//

#ifndef OASIS_SERIALIZATION_HPP
#define OASIS_SERIALIZATION_HPP

#include "Expression.hpp"

namespace Oasis {

class Real;
class Imaginary;
class Variable;
class Undefined;

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

template <IExpression, IExpression>
class Derivative;

template <IExpression Integrand, IExpression Differential>
class Integral;

class SerializationVisitor {
public:
    virtual void Serialize(const Real& real) = 0;
    virtual void Serialize(const Imaginary& imaginary) = 0;
    virtual void Serialize(const Variable& variable) = 0;
    virtual void Serialize(const Undefined& undefined) = 0;
    virtual void Serialize(const Add<Expression, Expression>& add) = 0;
    virtual void Serialize(const Subtract<Expression, Expression>& subtract) = 0;
    virtual void Serialize(const Multiply<Expression, Expression>& multiply) = 0;
    virtual void Serialize(const Divide<Expression, Expression>& divide) = 0;
    virtual void Serialize(const Exponent<Expression, Expression>& exponent) = 0;
    virtual void Serialize(const Log<Expression, Expression>& log) = 0;
    virtual void Serialize(const Negate<Expression>& negate) = 0;
    virtual void Serialize(const Derivative<Expression, Expression>& derivative) = 0;
    virtual void Serialize(const Integral<Expression, Expression>& integral) = 0;

    virtual ~SerializationVisitor() = default;
};

}

#endif // OASIS_SERIALIZATION_HPP

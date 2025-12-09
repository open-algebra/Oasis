//
// Created by Matthew McCall on 12/9/25.
//

#ifndef OASIS_FWDDECLS_HPP
#define OASIS_FWDDECLS_HPP

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
}

#endif // OASIS_FWDDECLS_HPP

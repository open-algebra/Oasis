//
// Created by Matthew McCall on 12/9/25.
//

#ifndef OASIS_FWDDECLS_HPP
#define OASIS_FWDDECLS_HPP

#include "Concepts.hpp"

namespace Oasis {
class Real;
class Imaginary;
class Matrix;
class Variable;
class Undefined;
class EulerNumber;
class Pi;

template <typename, typename>
class Add;

template <typename MinuendT, typename SubtrahendT>
class Subtract;

template <typename MultiplicandT, typename MultiplierT>
class Multiply;

template <typename DividendT, typename DivisorT>
class Divide;

template <typename BaseT, typename PowerT>
class Exponent;

template <typename BaseT, typename ArgumentT>
class Log;

template <typename OperandT>
class Negate;

template <typename OperandT>
class Magnitude;

template <typename, typename>
class Derivative;

template <typename Integrand, typename Differential>
class Integral;
}

#endif // OASIS_FWDDECLS_HPP

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

template <typename, typename>
class Subtract;

template <typename, typename>
class Multiply;

template <typename, typename>
class Divide;

template <typename, typename>
class Exponent;

template <typename, typename>
class Log;

template <typename>
class Negate;

template <typename>
class Magnitude;

template <typename>
class Sine;

template <typename, typename>
class Derivative;

template <typename, typename>
class Integral;
}

#endif // OASIS_FWDDECLS_HPP

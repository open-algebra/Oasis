//
// Created by Andrew Nazareth on 9/27/24.
//

#include <fmt/format.h>

#include <utility>

#include <utility>

#include <utility>

#include "Oasis/MathMLSerializer.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Derivative.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Matrix.hpp"
#include "Oasis/Negate.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"
#include "Oasis/EulerNumber.hpp"
#include "Oasis/Pi.hpp"
#include "Oasis/Magnitude.hpp"
#include "Oasis/LatexSerializer.hpp"

namespace Oasis{
Oasis::LatexSerializer::LatexSerializer(std::string output)
    : output(std::move(output))
{
}

void Oasis::LatexSerializer::Serialize(const Real& real)
{
    result = fmt::format("{:.5}", real.GetValue());
    output.append(result);
}

void Oasis::LatexSerializer::Serialize(const Imaginary& imaginary)
{
    result = "i";
    output.append(result);
}

void Oasis::LatexSerializer::Serialize(const Matrix& matrix)
{
}

void Oasis::LatexSerializer::Serialize(const Variable& variable)
{
    result = variable.GetName();
    output.append(result);
}

void Oasis::LatexSerializer::Serialize(const Undefined& undefined)
{
    result = "Undefined";
    output.append(result);
}

void Oasis::LatexSerializer::Serialize(const Pi&)
{
    result = "\\pi";
    output.append(result);
}

void Oasis::LatexSerializer::Serialize(const EulerNumber&)
{
    result = "e";
    output.append(result);
}

void Oasis::LatexSerializer::Serialize(const Add<Expression, Expression>& add)
{
}

void Oasis::LatexSerializer::Serialize(const Subtract<Expression, Expression>& subtract)
{
}

void Oasis::LatexSerializer::Serialize(const Multiply<Expression, Expression>& multiply)
{
}

void Oasis::LatexSerializer::Serialize(const Divide<Expression, Expression>& divide)
{
}

void Oasis::LatexSerializer::Serialize(const Exponent<Expression, Expression>& exponent)
{
}

void Oasis::LatexSerializer::Serialize(const Log<Expression, Expression>& log)
{
}

void Oasis::LatexSerializer::Serialize(const Negate<Expression>& negate)
{
}

void Oasis::LatexSerializer::Serialize(const Magnitude<Expression>& magnitude)
{
}

void Oasis::LatexSerializer::Serialize(const Derivative<Expression, Expression>& derivative)
{
}

void Oasis::LatexSerializer::Serialize(const Integral<Expression, Expression>& integral)
{
}

std::string Oasis::LatexSerializer::GetOutput() const {
    return output;
}

std::string Oasis::LatexSerializer::GetResult() const
{
    return result;
}

}
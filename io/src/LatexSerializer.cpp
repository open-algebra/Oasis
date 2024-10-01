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

LatexSerializer::LatexSerializer(){
    latexOptions = LatexOptions{NO_SPACING};
}

LatexSerializer::LatexSerializer(LatexOptions& options)
{
    latexOptions = options;
}

void LatexSerializer::Serialize(const Real& real)
{
    result = fmt::format("{:.5}", real.GetValue());
}

void LatexSerializer::Serialize(const Imaginary& imaginary)
{
    result = "i";
}

void LatexSerializer::Serialize(const Matrix& matrix)
{
    // TODO: Implement
}

void LatexSerializer::Serialize(const Variable& variable)
{
    result = variable.GetName();
}

void LatexSerializer::Serialize(const Undefined& undefined)
{
    result = "Undefined";
}

void LatexSerializer::Serialize(const Pi&)
{
    result = "\\pi";
}

void LatexSerializer::Serialize(const EulerNumber&)
{
    result = "e";
}

void LatexSerializer::Serialize(const Add<Expression, Expression>& add)
{
    add.GetMostSigOp().Serialize(*this);
    const auto mostSigOpStr = getResult();

    add.GetLeastSigOp().Serialize(*this);
    const auto leastSigOpStr = getResult();
    if (latexOptions.spacing == NO_SPACING)
        result = fmt::format("\\left({}+{}\\right)", mostSigOpStr, leastSigOpStr);
    else if (latexOptions.spacing == SPACING)
        result = fmt::format("\\left({} + {}\\right)", mostSigOpStr, leastSigOpStr);
}

void LatexSerializer::Serialize(const Subtract<Expression, Expression>& subtract)
{
    subtract.GetMostSigOp().Serialize(*this);
    const auto mostSigOpStr = getResult();

    subtract.GetLeastSigOp().Serialize(*this);
    const auto leastSigOpStr = getResult();

    if (latexOptions.spacing == NO_SPACING)
        result = fmt::format("\\left({}-{}\\right)", mostSigOpStr, leastSigOpStr);
    else if (latexOptions.spacing == SPACING)
        result = fmt::format("\\left({} - {}\\right)", mostSigOpStr, leastSigOpStr);
}

void LatexSerializer::Serialize(const Multiply<Expression, Expression>& multiply)
{
    multiply.GetMostSigOp().Serialize(*this);
    const auto mostSigOpStr = getResult();

    multiply.GetLeastSigOp().Serialize(*this);
    const auto leastSigOpStr = getResult();

    if (latexOptions.spacing == NO_SPACING)
        result = fmt::format("\\left({}*{}\\right)", mostSigOpStr, leastSigOpStr);
    else if (latexOptions.spacing == SPACING)
        result = fmt::format("\\left({} * {}\\right)", mostSigOpStr, leastSigOpStr);
}

void LatexSerializer::Serialize(const Divide<Expression, Expression>& divide)
{
    divide.GetMostSigOp().Serialize(*this);
    const auto mostSigOpStr = getResult();

    divide.GetLeastSigOp().Serialize(*this);
    const auto leastSigOpStr = getResult();

    result = "\\left(\\frac{" + mostSigOpStr + "}{" + leastSigOpStr + "}\\right)";
}

void LatexSerializer::Serialize(const Exponent<Expression, Expression>& exponent)
{
    exponent.GetMostSigOp().Serialize(*this);
    const auto mostSigOpStr = getResult();

    exponent.GetLeastSigOp().Serialize(*this);
    const auto leastSigOpStr = getResult();

    result = "\\left("+ mostSigOpStr +"\\right)^{"+leastSigOpStr+"}";
}

void LatexSerializer::Serialize(const Log<Expression, Expression>& log)
{
    // if (log.GetMostSigOp())
    log.GetMostSigOp().Serialize(*this);
    const auto mostSigOpStr = getResult();

    log.GetLeastSigOp().Serialize(*this);
    const auto leastSigOpStr = getResult();
}

void LatexSerializer::Serialize(const Negate<Expression>& negate)
{
}

void LatexSerializer::Serialize(const Magnitude<Expression>& magnitude)
{
}

void LatexSerializer::Serialize(const Derivative<Expression, Expression>& derivative)
{
}

void LatexSerializer::Serialize(const Integral<Expression, Expression>& integral)
{
}

std::string LatexSerializer::getResult() const
{
    return result;
}

}
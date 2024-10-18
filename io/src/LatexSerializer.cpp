//
// Created by Andrew Nazareth on 9/27/24.
//

#include <fmt/format.h>

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
    result = "\\begin{bmatrix}\n";
    MatrixXXD mat = matrix.GetMatrix();
    std::string row{};
    for (int r = 0; r < matrix.GetRows(); r++){
        row = "";
        for (int c = 0; c < matrix.GetCols(); c++){
            row += fmt::format("{:.5}", mat(r,c));
            if (c < matrix.GetCols()-1){
                row += "&";
            }
        }
        row += "\\\\\n";
        result += row;
    }
    result += "\\end{bmatrix}\n";
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

    result = "\\log_{"+mostSigOpStr+"}\\left("+leastSigOpStr+"\\right)";
}

void LatexSerializer::Serialize(const Negate<Expression>& negate)
{
    negate.GetOperand().Serialize(*this);
    const auto op = getResult();

    result = "\\left(-"+op+"\\right)";
}

void LatexSerializer::Serialize(const Magnitude<Expression>& magnitude)
{
    magnitude.GetOperand().Serialize(*this);
    const auto op = getResult();

    result = "\\left|"+op+"\\right|";
}

void LatexSerializer::Serialize(const Derivative<Expression, Expression>& derivative)
{
    derivative.GetMostSigOp().Serialize(*this);
    const auto MostSigOpStr = getResult();

    derivative.GetLeastSigOp().Serialize(*this);
    const auto LeastSigOpStr = getResult();

    result = "\\frac{d}{d"+LeastSigOpStr+"}\\left("+MostSigOpStr+"\\right)";
}

void LatexSerializer::Serialize(const Integral<Expression, Expression>& integral)
{
    integral.GetMostSigOp().Serialize(*this);
    const auto MostSigOpStr = getResult();

    integral.GetLeastSigOp().Serialize(*this);
    const auto LeastSigOpStr = getResult();

    result = "\\int\\left("+MostSigOpStr+"\\right)d"+LeastSigOpStr;
}

std::string LatexSerializer::getResult() const
{
    return result;
}

}
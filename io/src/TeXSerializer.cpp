//
// Created by Andrew Nazareth on 9/27/24.
//

#include <fmt/format.h>

#include "Oasis/MathMLSerializer.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Derivative.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/EulerNumber.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Magnitude.hpp"
#include "Oasis/Matrix.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Negate.hpp"
#include "Oasis/Pi.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/TeXSerializer.hpp"
#include "Oasis/Variable.hpp"

namespace Oasis{

void TeXSerializer::SetImaginaryCharacter(ImaginaryCharacter character)
{
    latexOptions.character = character;
}

void TeXSerializer::SetNumPlaces(uint8_t num)
{
    latexOptions.numPlaces = num;
}

void TeXSerializer::SetSpacing(Spacing sp)
{
    latexOptions.spacing = sp;
}
DivisionType TeXSerializer::GetDivType()
{
    return latexOptions.divType;
}

void TeXSerializer::SetDivType(DivisionType dv)
{
    latexOptions.divType = dv;
}
ImaginaryCharacter TeXSerializer::GetImaginaryCharacter()
{
    return latexOptions.character;
}
Spacing TeXSerializer::GetSpacing()
{
    return latexOptions.spacing;
}
uint8_t TeXSerializer::GetNumPlaces()
{
    return latexOptions.numPlaces;
}

void TeXSerializer::SetTeXDialect(TeXDialect dt)
{
    latexOptions.dialect = dt;
}
TeXDialect TeXSerializer::GetTeXDialect()
{
    return latexOptions.dialect;
}
void TeXSerializer::AddTeXPackage(SupportedPackages package)
{
    latexOptions.packages.insert(package);
}
void TeXSerializer::RemoveTeXPackage(SupportedPackages package)
{
    latexOptions.packages.erase(package);
}

void TeXSerializer::Serialize(const Real& real)
{
    result = fmt::format("{:.{}}", real.GetValue(), latexOptions.numPlaces+1);
}

void TeXSerializer::Serialize(const Imaginary& imaginary)
{
    if (latexOptions.character == CHARACTER_J) result = "j";
    else result = "i";
}

void TeXSerializer::Serialize(const Matrix& matrix)
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

void TeXSerializer::Serialize(const Variable& variable)
{
    result = variable.GetName();
}

void TeXSerializer::Serialize(const Undefined& undefined)
{
    result = "Undefined";
}

void TeXSerializer::Serialize(const Pi&)
{
    result = "\\pi";
}

void TeXSerializer::Serialize(const EulerNumber&)
{
    result = "e";
}

void TeXSerializer::Serialize(const Add<Expression, Expression>& add)
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

void TeXSerializer::Serialize(const Subtract<Expression, Expression>& subtract)
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

void TeXSerializer::Serialize(const Multiply<Expression, Expression>& multiply)
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

void TeXSerializer::Serialize(const Divide<Expression, Expression>& divide)
{
    divide.GetMostSigOp().Serialize(*this);
    const auto mostSigOpStr = getResult();

    divide.GetLeastSigOp().Serialize(*this);
    const auto leastSigOpStr = getResult();

    if (latexOptions.divType == DIV){
        result = fmt::format("\\left({{{}}}\\div{{{}}}\\right)", mostSigOpStr, leastSigOpStr);
    } else {
        result = fmt::format("\\left(\\frac{{{}}}{{{}}}\\right)", mostSigOpStr, leastSigOpStr);
    }
}

void TeXSerializer::Serialize(const Exponent<Expression, Expression>& exponent)
{
    exponent.GetMostSigOp().Serialize(*this);
    const auto mostSigOpStr = getResult();

    exponent.GetLeastSigOp().Serialize(*this);
    const auto leastSigOpStr = getResult();

    result = fmt::format("\\left({}\\right)^{{{}}}", mostSigOpStr, leastSigOpStr);
}

void TeXSerializer::Serialize(const Log<Expression, Expression>& log)
{
    // if (log.GetMostSigOp())
    log.GetMostSigOp().Serialize(*this);
    const auto mostSigOpStr = getResult();

    log.GetLeastSigOp().Serialize(*this);
    const auto leastSigOpStr = getResult();

    result = fmt::format("\\log_{{{}}}\\left({}\\right)", mostSigOpStr, leastSigOpStr);
}

void TeXSerializer::Serialize(const Negate<Expression>& negate)
{
    negate.GetOperand().Serialize(*this);
    const auto op = getResult();

    result = fmt::format("\\left(-{}\\right)", op);
}

void TeXSerializer::Serialize(const Magnitude<Expression>& magnitude)
{
    magnitude.GetOperand().Serialize(*this);
    const auto op = getResult();

    result = fmt::format("\\left|{}\\right|", op);
}

void TeXSerializer::Serialize(const Derivative<Expression, Expression>& derivative)
{
    derivative.GetMostSigOp().Serialize(*this);
    const auto MostSigOpStr = getResult();

    derivative.GetLeastSigOp().Serialize(*this);
    const auto LeastSigOpStr = getResult();

    result = fmt::format("\\frac{{d}}{{d{}}}\\left({}\\right)", LeastSigOpStr, MostSigOpStr);
}

void TeXSerializer::Serialize(const Integral<Expression, Expression>& integral)
{
    integral.GetMostSigOp().Serialize(*this);
    const auto MostSigOpStr = getResult();

    integral.GetLeastSigOp().Serialize(*this);
    const auto LeastSigOpStr = getResult();

    result = fmt::format("\\int\\left({}\\right)d{}", MostSigOpStr, LeastSigOpStr);
}

std::string TeXSerializer::getResult() const
{
    return result;
}

}
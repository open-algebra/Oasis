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

std::any TeXSerializer::Visit(const Real& real)
{
    result = fmt::format("{:.{}}", real.GetValue(), latexOptions.numPlaces+1);
    return result;
}

std::any TeXSerializer::Visit(const Imaginary& imaginary)
{
    if (latexOptions.character == CHARACTER_J) result = "j";
    else result = "i";
    return result;
}

std::any TeXSerializer::Visit(const Matrix& matrix)
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
    return result;
}

std::any TeXSerializer::Visit(const Variable& variable)
{
    result = variable.GetName();
    return result;
}

std::any TeXSerializer::Visit(const Undefined& undefined)
{
    result = "Undefined";
    return result;
}

std::any TeXSerializer::Visit(const Pi&)
{
    result = "\\pi";
    return result;
}

std::any TeXSerializer::Visit(const EulerNumber&)
{
    result = "e";
    return result;
}

std::any TeXSerializer::Visit(const Add<Expression, Expression>& add)
{
    const auto mostSigOpStr = std::any_cast<std::string>(add.GetMostSigOp().Accept(*this));
    const auto leastSigOpStr = std::any_cast<std::string>(add.GetLeastSigOp().Accept(*this));
    if (latexOptions.spacing == NO_SPACING)
        result = fmt::format("\\left({}+{}\\right)", mostSigOpStr, leastSigOpStr);
    else if (latexOptions.spacing == SPACING)
        result = fmt::format("\\left({} + {}\\right)", mostSigOpStr, leastSigOpStr);

    return result;
}

std::any TeXSerializer::Visit(const Subtract<Expression, Expression>& subtract)
{
    const auto mostSigOpStr = std::any_cast<std::string>(subtract.GetMostSigOp().Accept(*this));
    const auto leastSigOpStr = std::any_cast<std::string>(subtract.GetLeastSigOp().Accept(*this));

    if (latexOptions.spacing == NO_SPACING)
        result = fmt::format("\\left({}-{}\\right)", mostSigOpStr, leastSigOpStr);
    else if (latexOptions.spacing == SPACING)
        result = fmt::format("\\left({} - {}\\right)", mostSigOpStr, leastSigOpStr);

    return result;
}

std::any TeXSerializer::Visit(const Multiply<Expression, Expression>& multiply)
{
    const auto mostSigOpStr = std::any_cast<std::string>(multiply.GetMostSigOp().Accept(*this));
    const auto leastSigOpStr = std::any_cast<std::string>(multiply.GetLeastSigOp().Accept(*this));

    if (latexOptions.spacing == NO_SPACING)
        result = fmt::format("\\left({}*{}\\right)", mostSigOpStr, leastSigOpStr);
    else if (latexOptions.spacing == SPACING)
        result = fmt::format("\\left({} * {}\\right)", mostSigOpStr, leastSigOpStr);

    return result;
}

std::any TeXSerializer::Visit(const Divide<Expression, Expression>& divide)
{
    const auto mostSigOpStr = std::any_cast<std::string>(divide.GetMostSigOp().Accept(*this));
    const auto leastSigOpStr = std::any_cast<std::string>(divide.GetLeastSigOp().Accept(*this));

    if (latexOptions.divType == DIV){
        result = fmt::format("\\left({{{}}}\\div{{{}}}\\right)", mostSigOpStr, leastSigOpStr);
    } else {
        result = fmt::format("\\left(\\frac{{{}}}{{{}}}\\right)", mostSigOpStr, leastSigOpStr);
    }

    return result;
}

std::any TeXSerializer::Visit(const Exponent<Expression, Expression>& exponent)
{
    const auto mostSigOpStr = std::any_cast<std::string>(exponent.GetMostSigOp().Accept(*this));
    const auto leastSigOpStr = std::any_cast<std::string>(exponent.GetLeastSigOp().Accept(*this));

    result = fmt::format("\\left({}\\right)^{{{}}}", mostSigOpStr, leastSigOpStr);
    return result;
}

std::any TeXSerializer::Visit(const Log<Expression, Expression>& log)
{
    // if (log.GetMostSigOp())
    const auto mostSigOpStr = std::any_cast<std::string>(log.GetMostSigOp().Accept(*this));
    const auto leastSigOpStr = std::any_cast<std::string>(log.GetLeastSigOp().Accept(*this));

    result = fmt::format("\\log_{{{}}}\\left({}\\right)", mostSigOpStr, leastSigOpStr);
    return result;
}

std::any TeXSerializer::Visit(const Negate<Expression>& negate)
{
    const auto op = std::any_cast<std::string>(negate.GetOperand().Accept(*this));

    result = fmt::format("\\left(-{}\\right)", op);
    return result;
}

std::any TeXSerializer::Visit(const Magnitude<Expression>& magnitude)
{
    const auto op = std::any_cast<std::string>(magnitude.GetOperand().Accept(*this));

    result = fmt::format("\\left|{}\\right|", op);
    return result;
}

std::any TeXSerializer::Visit(const Derivative<Expression, Expression>& derivative)
{
    const auto MostSigOpStr = std::any_cast<std::string>(derivative.GetMostSigOp().Accept(*this));
    const auto LeastSigOpStr =     std::any_cast<std::string>(derivative.GetLeastSigOp().Accept(*this));

    result = fmt::format("\\frac{{d}}{{d{}}}\\left({}\\right)", LeastSigOpStr, MostSigOpStr);
    return result;
}

std::any TeXSerializer::Visit(const Integral<Expression, Expression>& integral)
{
    const auto MostSigOpStr = std::any_cast<std::string>(integral.GetMostSigOp().Accept(*this));
    const auto LeastSigOpStr = std::any_cast<std::string>(integral.GetLeastSigOp().Accept(*this));

    result = fmt::format("\\int\\left({}\\right)d{}", MostSigOpStr, LeastSigOpStr);
    return result;
}

}
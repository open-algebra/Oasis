//
// Created by Andrew Nazareth on 9/27/24.
//

#include <format>

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

namespace Oasis {

void TeXSerializer::SetImaginaryCharacter(TeXOpts::ImgSym character)
{
    latexOptions.character = character;
}

void TeXSerializer::SetNumPlaces(uint8_t num)
{
    latexOptions.numPlaces = num;
}

void TeXSerializer::SetSpacing(TeXOpts::Spacing sp)
{
    latexOptions.spacing = sp;
}
TeXOpts::DivType TeXSerializer::GetDivType()
{
    return latexOptions.divType;
}

void TeXSerializer::SetDivType(TeXOpts::DivType dv)
{
    latexOptions.divType = dv;
}
TeXOpts::ImgSym TeXSerializer::GetImaginaryCharacter()
{
    return latexOptions.character;
}
TeXOpts::Spacing TeXSerializer::GetSpacing()
{
    return latexOptions.spacing;
}
uint8_t TeXSerializer::GetNumPlaces()
{
    return latexOptions.numPlaces;
}

void TeXSerializer::SetTeXDialect(TeXOpts::Dialect dt)
{
    latexOptions.dialect = dt;
}
TeXOpts::Dialect TeXSerializer::GetTeXDialect()
{
    return latexOptions.dialect;
}

void TeXSerializer::AddTeXPackage(TeXOpts::Pkgs package)
{
    latexOptions.packages.insert(package);
}

void TeXSerializer::RemoveTeXPackage(TeXOpts::Pkgs package)
{
    latexOptions.packages.erase(package);
}

std::any TeXSerializer::Visit(const Real& real)
{
    auto result = std::format("{:.{}}", real.GetValue(), latexOptions.numPlaces + 1);
    return result;
}

std::any TeXSerializer::Visit(const Imaginary&)
{
    switch (latexOptions.character) {
    case TeXOpts::ImgSym::J:
        return std::string { "j" };
    case TeXOpts::ImgSym::I:
        return std::string { "i" };
    }

    return {};
}

std::any TeXSerializer::Visit(const Matrix& matrix)
{
    std::string result = "\\begin{bmatrix}\n";
    MatrixXXD mat = matrix.GetMatrix();
    std::string row {};
    for (size_t r = 0; r < matrix.GetRows(); r++) {
        row = "";
        for (size_t c = 0; c < matrix.GetCols(); c++) {
            row += std::format("{:.5}", mat(r, c));
            if (c < matrix.GetCols() - 1) {
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
    return variable.GetName();
}

std::any TeXSerializer::Visit(const Undefined&)
{
    return std::string { "Undefined" };
}

std::any TeXSerializer::Visit(const Pi&)
{
    return std::string { "\\pi" };
}

std::any TeXSerializer::Visit(const EulerNumber&)
{
    return std::string { "e" };
}

std::any TeXSerializer::Visit(const Add<Expression, Expression>& add)
{
    auto result = SerializeArithBinExp(add, "+");
    return result ? result.value() : std::any {};
}

std::any TeXSerializer::Visit(const Subtract<Expression, Expression>& subtract)
{
    auto result = SerializeArithBinExp(subtract, "-");
    return result ? result.value() : std::any {};
}

std::any TeXSerializer::Visit(const Multiply<Expression, Expression>& multiply)
{
    auto result = SerializeArithBinExp(multiply, "*");
    return result ? result.value() : std::any {};
}

std::any TeXSerializer::Visit(const Divide<Expression, Expression>& divide)
{
    auto ops = GetOpsOfBinExp(divide);
    if (!ops)
        return {};

    const auto& [mostSigOpStr, leastSigOpStr] = ops.value();

    if (latexOptions.divType == TeXOpts::DivType::DIV)
        return std::format("\\left({{{}}}\\div{{{}}}\\right)", mostSigOpStr, leastSigOpStr);

    if (latexOptions.divType == TeXOpts::DivType::FRAC)
        return std::format("\\left(\\frac{{{}}}{{{}}}\\right)", mostSigOpStr, leastSigOpStr);

    return {};
}

std::any TeXSerializer::Visit(const Exponent<Expression, Expression>& exponent)
{
    auto ops = GetOpsOfBinExp(exponent);
    if (!ops)
        return {};

    const auto& [mostSigOpStr, leastSigOpStr] = ops.value();
    return std::format("\\left({}\\right)^{{{}}}", mostSigOpStr, leastSigOpStr);
}

std::any TeXSerializer::Visit(const Log<Expression, Expression>& log)
{
    // if (log.GetMostSigOp())
    auto ops = GetOpsOfBinExp(log);
    if (!ops)
        return {};

    const auto& [mostSigOpStr, leastSigOpStr] = ops.value();
    return std::format("\\log_{{{}}}\\left({}\\right)", mostSigOpStr, leastSigOpStr);
}

std::any TeXSerializer::Visit(const Negate<Expression>& negate)
{
    const auto op = negate.GetOperand().Accept(*this);
    return op ? std::format("\\left(-{}\\right)", op.value()) : std::any {};
}

std::any TeXSerializer::Visit(const Magnitude<Expression>& magnitude)
{
    const auto op = magnitude.GetOperand().Accept(*this);
    return op ? std::format("\\left|{}\\right|", op.value()) : std::any {};
}

std::any TeXSerializer::Visit(const Derivative<Expression, Expression>& derivative)
{
    auto ops = GetOpsOfBinExp(derivative);
    if (!ops)
        return {};

    const auto& [mostSigOpStr, leastSigOpStr] = ops.value();
    return std::format("\\frac{{d}}{{d{}}}\\left({}\\right)", mostSigOpStr, leastSigOpStr);
}

std::any TeXSerializer::Visit(const Integral<Expression, Expression>& integral)
{
    auto ops = GetOpsOfBinExp(integral);
    if (!ops)
        return {};

    const auto& [mostSigOpStr, leastSigOpStr] = ops.value();
    return std::format("\\int\\left({}\\right)d{}", mostSigOpStr, leastSigOpStr);
}

}
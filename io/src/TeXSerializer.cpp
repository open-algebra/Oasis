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
#include "Oasis/Sine.hpp"
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

auto TeXSerializer::TypedVisit(const Real& real) -> RetT
{
    auto result = std::format("{:.{}}", real.GetValue(), latexOptions.numPlaces + 1);
    return result;
}

auto TeXSerializer::TypedVisit(const Imaginary&) -> RetT
{
    switch (latexOptions.character) {
    case TeXOpts::ImgSym::J:
        return std::string { "j" };
    case TeXOpts::ImgSym::I:
        return std::string { "i" };
    }

    return {};
}

auto TeXSerializer::TypedVisit(const Matrix& matrix) -> RetT
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

auto TeXSerializer::TypedVisit(const Variable& variable) -> RetT
{
    return variable.GetName();
}

auto TeXSerializer::TypedVisit(const Undefined&) -> RetT
{
    return std::string { "Undefined" };
}

auto TeXSerializer::TypedVisit(const Pi&) -> RetT
{
    return std::string { "\\pi" };
}

auto TeXSerializer::TypedVisit(const EulerNumber&) -> RetT
{
    return std::string { "e" };
}

auto TeXSerializer::TypedVisit(const Add<Expression, Expression>& add) -> RetT
{
    return SerializeArithBinExp(add, "+");
}

auto TeXSerializer::TypedVisit(const Subtract<Expression, Expression>& subtract) -> RetT
{
    return SerializeArithBinExp(subtract, "-");
}

auto TeXSerializer::TypedVisit(const Multiply<Expression, Expression>& multiply) -> RetT
{
    return SerializeArithBinExp(multiply, "*");
}

auto TeXSerializer::TypedVisit(const Divide<Expression, Expression>& divide) -> RetT
{
    return GetOpsOfBinExp(divide).and_then([this](const auto& ops) -> RetT {
        const auto& [mostSigOpStr, leastSigOpStr] = ops;
        if (latexOptions.divType == TeXOpts::DivType::DIV)
            return std::format("\\left({{{}}}\\div{{{}}}\\right)", mostSigOpStr, leastSigOpStr);
        if (latexOptions.divType == TeXOpts::DivType::FRAC)
            return std::format("\\left(\\frac{{{}}}{{{}}}\\right)", mostSigOpStr, leastSigOpStr);
        return std::unexpected { "Invalid divide option" };
    });
}

auto TeXSerializer::TypedVisit(const Exponent<Expression, Expression>& exponent) -> RetT
{
    return GetOpsOfBinExp(exponent).transform([](const std::pair<std::string, std::string>& ops) {
        return std::format("\\left({}\\right)^{{{}}}", ops.first, ops.second);
    });
}

auto TeXSerializer::TypedVisit(const Log<Expression, Expression>& log) -> RetT
{
    return GetOpsOfBinExp(log).transform([](const std::pair<std::string, std::string>& ops) {
        return std::format("\\log_{{{}}}\\left({}\\right)", ops.first, ops.second);
    });
}

auto TeXSerializer::TypedVisit(const Negate<Expression>& negate) -> RetT
{
    return negate.GetOperand().Accept(*this).transform([](const std::string& str) {
        return std::format("-\\left({}\\right)", str);
    });
}

auto TeXSerializer::TypedVisit(const Sine<Expression>& sine) -> RetT
{
    return sine.GetOperand().Accept(*this).transform([](const std::string& str) {
        return std::format("sine\\left({}\\right)", str);
    });
}

auto TeXSerializer::TypedVisit(const Magnitude<Expression>& magnitude) -> RetT
{
    return magnitude.GetOperand().Accept(*this).transform([](const std::string& str) {
        return std::format("\\left|{}\\right|", str);
    });
}

auto TeXSerializer::TypedVisit(const Derivative<Expression, Expression>& derivative) -> RetT
{
    return GetOpsOfBinExp(derivative).transform([](const std::pair<std::string, std::string>& ops) {
        return std::format("\\frac{{d}}{{d{}}}\\left({}\\right)", ops.first, ops.second);
    });
}

auto TeXSerializer::TypedVisit(const Integral<Expression, Expression>& integral) -> RetT
{
    return GetOpsOfBinExp(integral).transform([](const std::pair<std::string, std::string>& ops) {
        return std::format("\\int\\left({}\\right)d{}", ops.first, ops.second);
    });
}

}
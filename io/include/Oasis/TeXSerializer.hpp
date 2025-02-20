//
// Created by Andrew Nazareth on 9/27/24.
//

#ifndef LATEXSERIALIZER_HPP
#define LATEXSERIALIZER_HPP

#include <format>
#include <set>
#include <string>

#include "Oasis/BinaryExpression.hpp"
#include "Oasis/Visit.hpp"

namespace Oasis {

struct TeXOpts {
    enum class Dialect {
        LATEX
    } dialect
        = Dialect::LATEX;

    enum class Spacing {
        MINIMAL,
        REGULAR
    } spacing
        = Spacing::MINIMAL;

    enum class ImgSym {
        I,
        J
    } character
        = ImgSym::I;

    uint8_t numPlaces = 2;

    enum class DivType {
        FRAC,
        DIV
    } divType
        = DivType::FRAC;

    enum class Pkgs {
        ESDIFF,
    };

    std::set<Pkgs> packages = { Pkgs::ESDIFF };
};

class TeXSerializer final : public Visitor {
public:
    TeXSerializer()
        : TeXSerializer(TeXOpts {})
    {
    }
    explicit TeXSerializer(TeXOpts options)
        : latexOptions(options)
    {
    }

    using RetT = std::string;

    void SetTeXDialect(TeXOpts::Dialect dt);
    void SetImaginaryCharacter(TeXOpts::ImgSym character);
    void SetNumPlaces(uint8_t num);
    void SetSpacing(TeXOpts::Spacing sp);
    void SetDivType(TeXOpts::DivType dv);

    TeXOpts::Dialect GetTeXDialect();
    TeXOpts::DivType GetDivType();
    TeXOpts::Spacing GetSpacing();
    uint8_t GetNumPlaces();
    TeXOpts::ImgSym GetImaginaryCharacter();

    void AddTeXPackage(TeXOpts::Pkgs package);
    void RemoveTeXPackage(TeXOpts::Pkgs package);

    auto Visit(const Real& real) -> any override;
    auto Visit(const Imaginary& imaginary) -> any override;
    auto Visit(const Matrix& matrix) -> any override;
    auto Visit(const Variable& variable) -> any override;
    auto Visit(const Undefined& undefined) -> any override;
    auto Visit(const Pi&) -> any override;
    auto Visit(const EulerNumber&) -> any override;
    auto Visit(const Add<Expression, Expression>& add) -> any override;
    auto Visit(const Subtract<Expression, Expression>& subtract) -> any override;
    auto Visit(const Multiply<Expression, Expression>& multiply) -> any override;
    auto Visit(const Divide<Expression, Expression>& divide) -> any override;
    auto Visit(const Exponent<Expression, Expression>& exponent) -> any override;
    auto Visit(const Log<Expression, Expression>& log) -> any override;
    auto Visit(const Negate<Expression>& negate) -> any override;
    auto Visit(const Magnitude<Expression>& magnitude) -> any override;
    auto Visit(const Derivative<Expression, Expression>& derivative) -> any override;
    auto Visit(const Integral<Expression, Expression>& integral) -> any override;

private:
    TeXOpts latexOptions {};

    auto GetOpsOfBinExp(const DerivedFromBinaryExpression auto& visited) -> std::optional<std::pair<std::string, std::string>>;
    auto SerializeArithBinExp(const DerivedFromBinaryExpression auto& visited, const std::string& op) -> std::optional<std::string>;
};

auto TeXSerializer::GetOpsOfBinExp(const DerivedFromBinaryExpression auto& visited) -> std::optional<std::pair<std::string, std::string>>
{
    const auto mostSigOpStr = visited.GetMostSigOp().Accept(*this);
    const auto leastSigOpStr = visited.GetLeastSigOp().Accept(*this);
    if (!mostSigOpStr || !leastSigOpStr)
        return {};

    return std::pair { mostSigOpStr.value(), leastSigOpStr.value() };
}

auto TeXSerializer::SerializeArithBinExp(const DerivedFromBinaryExpression auto& visited, const std::string& op) -> std::optional<std::string>
{
    auto ops = GetOpsOfBinExp(visited);
    if (!ops)
        return {};

    const auto& [mostSigOpStr, leastSigOpStr] = ops.value();

    if (latexOptions.spacing == TeXOpts::Spacing::MINIMAL)
        return std::format("\\left({}{}{}\\right)", mostSigOpStr, op, leastSigOpStr);

    if (latexOptions.spacing == TeXOpts::Spacing::REGULAR)
        return std::format("\\left({} {} {}\\right)", mostSigOpStr, op, leastSigOpStr);

    return {};
}

}

#endif // LATEXSERIALIZER_HPP

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

class TeXSerializer final : public TypedVisitor<std::expected<std::string, std::string>> {
public:
    TeXSerializer()
        : TeXSerializer(TeXOpts {})
    {
    }
    explicit TeXSerializer(TeXOpts options)
        : latexOptions(options)
    {
    }

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

    auto TypedVisit(const Real& real) -> RetT override;
    auto TypedVisit(const Imaginary& imaginary) -> RetT override;
    auto TypedVisit(const Matrix& matrix) -> RetT override;
    auto TypedVisit(const Variable& variable) -> RetT override;
    auto TypedVisit(const Undefined& undefined) -> RetT override;
    auto TypedVisit(const Pi&) -> RetT override;
    auto TypedVisit(const EulerNumber&) -> RetT override;
    auto TypedVisit(const Add<Expression, Expression>& add) -> RetT override;
    auto TypedVisit(const Subtract<Expression, Expression>& subtract) -> RetT override;
    auto TypedVisit(const Multiply<Expression, Expression>& multiply) -> RetT override;
    auto TypedVisit(const Divide<Expression, Expression>& divide) -> RetT override;
    auto TypedVisit(const Exponent<Expression, Expression>& exponent) -> RetT override;
    auto TypedVisit(const Log<Expression, Expression>& log) -> RetT override;
    auto TypedVisit(const Negate<Expression>& negate) -> RetT override;
    auto TypedVisit(const Sine<Expression>& sine) -> RetT override;
    auto TypedVisit(const Magnitude<Expression>& magnitude) -> RetT override;
    auto TypedVisit(const Derivative<Expression, Expression>& derivative) -> RetT override;
    auto TypedVisit(const Integral<Expression, Expression>& integral) -> RetT override;

private:
    TeXOpts latexOptions {};

    auto GetOpsOfBinExp(const DerivedFromBinaryExpression auto& visited) -> std::expected<std::pair<std::string, std::string>, std::string>;
    auto SerializeArithBinExp(const DerivedFromBinaryExpression auto& visited, const std::string& op) -> std::expected<std::string, std::string>;
};

auto TeXSerializer::GetOpsOfBinExp(const DerivedFromBinaryExpression auto& visited) -> std::expected<std::pair<std::string, std::string>, std::string>
{
    TeXSerializer& thisSerializer = *this;
    return visited.GetMostSigOp().Accept(thisSerializer).and_then([&thisSerializer, &visited](const std::string& mostSigOpStr) {
        return visited.GetLeastSigOp().Accept(thisSerializer).transform([&mostSigOpStr](const std::string& leastSigOpStr) {
            return std::pair { mostSigOpStr, leastSigOpStr };
        });
    });
}

auto TeXSerializer::SerializeArithBinExp(const DerivedFromBinaryExpression auto& visited, const std::string& op) -> std::expected<std::string, std::string>
{
    return GetOpsOfBinExp(visited).and_then([&op, this](const std::pair<std::string, std::string>& ops) -> std::expected<std::string, std::string> {
        const auto& [mostSigOpStr, leastSigOpStr] = ops;
        if (latexOptions.spacing == TeXOpts::Spacing::MINIMAL)
            return std::format("\\left({}{}{}\\right)", mostSigOpStr, op, leastSigOpStr);

        if (latexOptions.spacing == TeXOpts::Spacing::REGULAR)
            return std::format("\\left({} {} {}\\right)", mostSigOpStr, op, leastSigOpStr);

        return std::unexpected { "Invalid spacing option" };
    });
}

}

#endif // LATEXSERIALIZER_HPP

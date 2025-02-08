//
// Created by Andrew Nazareth on 9/27/24.
//

#ifndef LATEXSERIALIZER_HPP
#define LATEXSERIALIZER_HPP

#include <string>
#include <set>

#include "Oasis/BinaryExpression.hpp"
#include "Oasis/Visit.hpp"

namespace Oasis {

struct TeXOpts {
    enum class Dialect {
        LATEX
    } dialect = Dialect::LATEX;

    enum class Spacing {
        MINIMAL,
        REGULAR
    } spacing = Spacing::MINIMAL;

    enum class ImgSym {
        I,
        J
    } character = ImgSym::I;

    uint8_t numPlaces = 2;

    enum class DivType {
        FRAC,
        DIV
    } divType = DivType::FRAC;

    enum class Pkgs {
        ESDIFF,
    };

    std::set<Pkgs> packages = { Pkgs::ESDIFF };
};

class TeXSerializer final : public Visitor {
public:
    TeXSerializer() : TeXSerializer(TeXOpts {}) {}
    explicit TeXSerializer(TeXOpts options) : latexOptions(options) {}

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

    std::any Visit(const Real& real) override;
    std::any Visit(const Imaginary& imaginary) override;
    std::any Visit(const Matrix& matrix) override;
    std::any Visit(const Variable& variable) override;
    std::any Visit(const Undefined& undefined) override;
    std::any Visit(const Pi&) override;
    std::any Visit(const EulerNumber&) override;
    std::any Visit(const Add<Expression, Expression>& add) override;
    std::any Visit(const Subtract<Expression, Expression>& subtract) override;
    std::any Visit(const Multiply<Expression, Expression>& multiply) override;
    std::any Visit(const Divide<Expression, Expression>& divide) override;
    std::any Visit(const Exponent<Expression, Expression>& exponent) override;
    std::any Visit(const Log<Expression, Expression>& log) override;
    std::any Visit(const Negate<Expression>& negate) override;
    std::any Visit(const Magnitude<Expression>& magnitude) override;
    std::any Visit(const Derivative<Expression, Expression>& derivative) override;
    std::any Visit(const Integral<Expression, Expression>& integral) override;

private:
    std::string result;
    TeXOpts latexOptions {};
};

}

#endif // LATEXSERIALIZER_HPP

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

enum Spacing{
    NO_SPACING,
    SPACING
};

enum ImaginaryCharacter{
    CHARACTER_I,
    CHARACTER_J
};

enum DivisionType{
    FRAC,
    DIV
};

enum TeXDialect{
    LATEX
};

enum SupportedPackages{
    ESDIFF,
};

struct TexOptions {
    TeXDialect dialect;
    Spacing spacing;
    ImaginaryCharacter character;
    uint8_t numPlaces;
    DivisionType divType;
    std::set<SupportedPackages> packages;

    explicit TexOptions(ImaginaryCharacter ch) : TexOptions(LATEX, ch) {}
    explicit TexOptions(uint8_t num) : TexOptions(LATEX, CHARACTER_I, num, FRAC) {}
    explicit TexOptions(Spacing spacing) : TexOptions(LATEX, CHARACTER_I, 6, FRAC, spacing) {}
    explicit TexOptions(DivisionType dv) : TexOptions(LATEX, CHARACTER_I, 6, dv) {}

    TexOptions(TeXDialect dt = LATEX, ImaginaryCharacter ch = CHARACTER_I, uint8_t num = 6,
                  DivisionType dv = FRAC, Spacing sp = NO_SPACING)
        : dialect(dt), spacing(sp), character(ch), numPlaces(num), divType(dv) {}
};

class TeXSerializer final : public Visitor {
public:
    TeXSerializer() : TeXSerializer(TexOptions {}) {}
    explicit TeXSerializer(TexOptions options) : latexOptions(options) {}

    void SetTeXDialect(TeXDialect dt);
    void SetImaginaryCharacter(ImaginaryCharacter character);
    void SetNumPlaces(uint8_t num);
    void SetSpacing(Spacing sp);
    void SetDivType(DivisionType dv);

    TeXDialect GetTeXDialect();
    DivisionType GetDivType();
    Spacing GetSpacing();
    uint8_t GetNumPlaces();
    ImaginaryCharacter GetImaginaryCharacter();

    void AddTeXPackage(SupportedPackages package);
    void RemoveTeXPackage(SupportedPackages package);

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
    TexOptions latexOptions{};
};

}

#endif // LATEXSERIALIZER_HPP

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

    void Visit(const Real& real) override;
    void Visit(const Imaginary& imaginary) override;
    void Visit(const Matrix& matrix) override;
    void Visit(const Variable& variable) override;
    void Visit(const Undefined& undefined) override;
    void Visit(const Pi&) override;
    void Visit(const EulerNumber&) override;
    void Visit(const Add<Expression, Expression>& add) override;
    void Visit(const Subtract<Expression, Expression>& subtract) override;
    void Visit(const Multiply<Expression, Expression>& multiply) override;
    void Visit(const Divide<Expression, Expression>& divide) override;
    void Visit(const Exponent<Expression, Expression>& exponent) override;
    void Visit(const Log<Expression, Expression>& log) override;
    void Visit(const Negate<Expression>& negate) override;
    void Visit(const Magnitude<Expression>& magnitude) override;
    void Visit(const Derivative<Expression, Expression>& derivative) override;
    void Visit(const Integral<Expression, Expression>& integral) override;

    [[nodiscard]] std::string getResult() const;

private:
    std::string result;
    TexOptions latexOptions{};
};

}

#endif // LATEXSERIALIZER_HPP

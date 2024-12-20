//
// Created by Andrew Nazareth on 9/27/24.
//

#ifndef LATEXSERIALIZER_HPP
#define LATEXSERIALIZER_HPP

#include "Oasis/BinaryExpression.hpp"
#include "Oasis/Serialization.hpp"
#include <string>
#include <set>

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

class TeXSerializer final : public SerializationVisitor {
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

    void Serialize(const Real& real) override;
    void Serialize(const Imaginary& imaginary) override;
    void Serialize(const Matrix& matrix) override;
    void Serialize(const Variable& variable) override;
    void Serialize(const Undefined& undefined) override;
    void Serialize(const Pi&) override;
    void Serialize(const EulerNumber&) override;
    void Serialize(const Add<Expression, Expression>& add) override;
    void Serialize(const Subtract<Expression, Expression>& subtract) override;
    void Serialize(const Multiply<Expression, Expression>& multiply) override;
    void Serialize(const Divide<Expression, Expression>& divide) override;
    void Serialize(const Exponent<Expression, Expression>& exponent) override;
    void Serialize(const Log<Expression, Expression>& log) override;
    void Serialize(const Negate<Expression>& negate) override;
    void Serialize(const Magnitude<Expression>& magnitude) override;
    void Serialize(const Derivative<Expression, Expression>& derivative) override;
    void Serialize(const Integral<Expression, Expression>& integral) override;

    [[nodiscard]] std::string getResult() const;

private:
    std::string result;
    TexOptions latexOptions{};
};

}

#endif // LATEXSERIALIZER_HPP

//
// Created by Andrew Nazareth on 9/27/24.
//

#ifndef LATEXSERIALIZER_HPP
#define LATEXSERIALIZER_HPP

#include "Oasis/BinaryExpression.hpp"
#include "Oasis/Serialization.hpp"
#include <string>

namespace Oasis {

enum Spacing{
    NO_SPACING,
    SPACING
};

enum ImaginaryCharacter{
    CHARACTER_I = 'i',
    CHARACTER_J = 'j'
};

enum DivisionType{
    FRAC,
    DIV
};

struct LatexOptions{
public:
    Spacing spacing;
    ImaginaryCharacter character;
    uint8_t numPlaces;
    DivisionType divType;

    explicit LatexOptions(uint8_t num) : LatexOptions(CHARACTER_I, num, FRAC) {}
    explicit LatexOptions(Spacing spacing) : LatexOptions(CHARACTER_I, 6, FRAC, spacing) {}
    explicit LatexOptions(DivisionType dv) : LatexOptions(CHARACTER_I, 6, dv) {}

    LatexOptions(ImaginaryCharacter ch = CHARACTER_I, uint8_t num = 6, DivisionType dv = FRAC, Spacing sp = NO_SPACING)
        : spacing(sp), character(ch), numPlaces(num) {}
};

class LatexSerializer final : public SerializationVisitor {
public:
    LatexSerializer() : LatexSerializer(LatexOptions{}) {}
    explicit LatexSerializer(LatexOptions options) : latexOptions(options) {}

    void SetImaginaryCharacter(ImaginaryCharacter character);
    void SetNumPlaces(uint8_t num);
    void SetSpacing(Spacing sp);

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
    LatexOptions latexOptions{};
};

}

#endif // LATEXSERIALIZER_HPP

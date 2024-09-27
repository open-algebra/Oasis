//
// Created by Andrew Nazareth on 9/27/24.
//

#ifndef LATEXSERIALIZER_HPP
#define LATEXSERIALIZER_HPP

#include "Oasis/BinaryExpression.hpp"
#include "Oasis/Serialization.hpp"
#include <string>

//template <template <typename, typename> typename T>
// concept DerivedFromBinaryExpression = std::derived_from<T<Oasis::Expression, Oasis::Expression>, Oasis::BinaryExpression<T>>;

namespace Oasis {

class LatexSerializer final : public SerializationVisitor {
public:
    explicit LatexSerializer(std::string output);

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

    [[nodiscard]] std::string GetOutput() const;
    [[nodiscard]] std::string GetResult() const;

private:

    std::string output;
    std::string result = NULL;
};

}

#endif // LATEXSERIALIZER_HPP

//
// Created by Matthew McCall on 4/28/24.
//

#ifndef MATHMLSERIALIZER_HPP
#define MATHMLSERIALIZER_HPP

#include <tinyxml2.h>

#include "Oasis/BinaryExpression.hpp"
#include "Oasis/Serialization.hpp"

template <template <typename, typename> typename T>
concept DerivedFromBinaryExpression = std::derived_from<T<Oasis::Expression, Oasis::Expression>, Oasis::BinaryExpression<T>>;

namespace Oasis {

class MathMLSerializer final : public SerializationVisitor {
public:
    explicit MathMLSerializer(tinyxml2::XMLDocument& doc);

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
    void Serialize(const Sine<Expression>& sine) override;

    [[nodiscard]] tinyxml2::XMLDocument& GetDocument() const;
    [[nodiscard]] tinyxml2::XMLElement* GetResult() const;

private:
    [[nodiscard]] tinyxml2::XMLElement* CreatePlaceholder() const;

    template <template <typename, typename> typename T>
        requires DerivedFromBinaryExpression<T>
    auto GetOpsAsMathMLPair(const T<Expression, Expression>& binexp) -> std::pair<tinyxml2::XMLElement*, tinyxml2::XMLElement*>
    {
        tinyxml2::XMLElement* mostSig = CreatePlaceholder();

        if (binexp.HasMostSigOp()) {
            binexp.GetMostSigOp().Serialize(*this);
            mostSig = GetResult();
        }

        tinyxml2::XMLElement* leastSig = CreatePlaceholder();

        if (binexp.HasLeastSigOp()) {
            binexp.GetLeastSigOp().Serialize(*this);
            leastSig = GetResult();
        }

        return { mostSig, leastSig };
    }

    tinyxml2::XMLDocument& doc;
    tinyxml2::XMLElement* result = nullptr;
};

}

#endif // MATHMLSERIALIZER_HPP

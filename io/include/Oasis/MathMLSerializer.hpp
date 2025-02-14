//
// Created by Matthew McCall on 4/28/24.
//

#ifndef MATHMLSERIALIZER_HPP
#define MATHMLSERIALIZER_HPP

#include <tinyxml2.h>

#include "Oasis/BinaryExpression.hpp"
#include "Oasis/RecursiveCast.hpp"
#include "Oasis/Visit.hpp"

namespace Oasis {

class MathMLSerializer final : public Visitor {
public:
    explicit MathMLSerializer(tinyxml2::XMLDocument& doc);
    using RetT = tinyxml2::XMLElement*;

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

    [[nodiscard]] tinyxml2::XMLDocument& GetDocument() const;

private:
    [[nodiscard]] tinyxml2::XMLElement* CreatePlaceholder() const;
    auto GetOpsAsMathMLPair(const DerivedFromBinaryExpression auto& binexp) -> std::pair<tinyxml2::XMLElement*, tinyxml2::XMLElement*>;

    tinyxml2::XMLDocument& doc;
};

auto MathMLSerializer::GetOpsAsMathMLPair(const DerivedFromBinaryExpression auto& binexp) -> std::pair<tinyxml2::XMLElement*, tinyxml2::XMLElement*>
{
    tinyxml2::XMLElement* mostSig = CreatePlaceholder();
    if (binexp.HasMostSigOp())
        mostSig = binexp.GetMostSigOp().Accept(*this).value();

    tinyxml2::XMLElement* leastSig = CreatePlaceholder();
    if (binexp.HasLeastSigOp())
        leastSig = binexp.GetLeastSigOp().Accept(*this).value();

    return { mostSig, leastSig };
}

}

#endif // MATHMLSERIALIZER_HPP

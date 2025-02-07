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

    [[nodiscard]] tinyxml2::XMLDocument& GetDocument() const;
    [[nodiscard]] tinyxml2::XMLElement* GetResult() const;

private:
    [[nodiscard]] tinyxml2::XMLElement* CreatePlaceholder() const;
    auto GetOpsAsMathMLPair(const DerivedFromBinaryExpression auto& binexp) -> std::pair<tinyxml2::XMLElement*, tinyxml2::XMLElement*>;

    tinyxml2::XMLDocument& doc;
    tinyxml2::XMLElement* result = nullptr;
};

auto MathMLSerializer::GetOpsAsMathMLPair(const DerivedFromBinaryExpression auto& binexp) -> std::pair<tinyxml2::XMLElement*, tinyxml2::XMLElement*>
{
    tinyxml2::XMLElement* mostSig = CreatePlaceholder();
    if (binexp.HasMostSigOp()) mostSig = std::any_cast<tinyxml2::XMLElement*>(binexp.GetMostSigOp().Accept(*this));

    tinyxml2::XMLElement* leastSig = CreatePlaceholder();
    if (binexp.HasLeastSigOp()) leastSig = std::any_cast<tinyxml2::XMLElement*>(binexp.GetLeastSigOp().Accept(*this));

    return { mostSig, leastSig };
}

}

#endif // MATHMLSERIALIZER_HPP

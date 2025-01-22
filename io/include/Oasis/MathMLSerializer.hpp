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

    [[nodiscard]] tinyxml2::XMLDocument& GetDocument() const;
    [[nodiscard]] tinyxml2::XMLElement* GetResult() const;

private:
    [[nodiscard]] tinyxml2::XMLElement* CreatePlaceholder() const;

    auto GetOpsAsMathMLPair(const DerivedFromBinaryExpression auto& binexp) -> std::pair<tinyxml2::XMLElement*, tinyxml2::XMLElement*>
    {
        tinyxml2::XMLElement* mostSig = CreatePlaceholder();

        if (binexp.HasMostSigOp()) {
            binexp.GetMostSigOp().Accept(*this);
            mostSig = GetResult();
        }

        tinyxml2::XMLElement* leastSig = CreatePlaceholder();

        if (binexp.HasLeastSigOp()) {
            binexp.GetLeastSigOp().Accept(*this);
            leastSig = GetResult();
        }

        return { mostSig, leastSig };
    }

    tinyxml2::XMLDocument& doc;
    tinyxml2::XMLElement* result = nullptr;
};

}

#endif // MATHMLSERIALIZER_HPP

//
// Created by Matthew McCall on 4/28/24.
//

#ifndef MATHMLSERIALIZER_HPP
#define MATHMLSERIALIZER_HPP

#include <gsl/gsl-lite.hpp>
#include <tinyxml2.h>

#include "Oasis/BinaryExpression.hpp"
#include "Oasis/RecursiveCast.hpp"
#include "Oasis/Visit.hpp"

namespace Oasis {

class MathMLSerializer final : public TypedVisitor<std::expected<gsl::not_null<tinyxml2::XMLElement*>, std::string>> {
public:
    explicit MathMLSerializer(tinyxml2::XMLDocument& doc);

    auto TypedVisit(const Real& real) -> RetT override;
    auto TypedVisit(const Imaginary& imaginary) -> RetT override;
    auto TypedVisit(const Matrix& matrix) -> RetT override;
    auto TypedVisit(const Variable& variable) -> RetT override;
    auto TypedVisit(const Undefined& undefined) -> RetT override;
    auto TypedVisit(const Pi&) -> RetT override;
    auto TypedVisit(const EulerNumber&) -> RetT override;
    auto TypedVisit(const Add<Expression, Expression>& add) -> RetT override;
    auto TypedVisit(const Subtract<Expression, Expression>& subtract) -> RetT override;
    auto TypedVisit(const Multiply<Expression, Expression>& multiply) -> RetT override;
    auto TypedVisit(const Divide<Expression, Expression>& divide) -> RetT override;
    auto TypedVisit(const Exponent<Expression, Expression>& exponent) -> RetT override;
    auto TypedVisit(const Log<Expression, Expression>& log) -> RetT override;
    auto TypedVisit(const Negate<Expression>& negate) -> RetT override;
    auto TypedVisit(const Sine<Expression>& sine) -> RetT override;
    auto TypedVisit(const Magnitude<Expression>& magnitude) -> RetT override;
    auto TypedVisit(const Derivative<Expression, Expression>& derivative) -> RetT override;
    auto TypedVisit(const Integral<Expression, Expression>& integral) -> RetT override;

    [[nodiscard]] tinyxml2::XMLDocument& GetDocument() const;

private:
    auto GetOpsAsMathMLPair(const DerivedFromBinaryExpression auto& binexp) -> std::expected<std::pair<gsl::not_null<tinyxml2::XMLElement*>, gsl::not_null<tinyxml2::XMLElement*>>, std::string>;
    tinyxml2::XMLDocument& doc;
};

auto MathMLSerializer::GetOpsAsMathMLPair(const DerivedFromBinaryExpression auto& binexp) -> std::expected<std::pair<gsl::not_null<tinyxml2::XMLElement*>, gsl::not_null<tinyxml2::XMLElement*>>, std::string>
{
    MathMLSerializer& thisSerializer = *this;
    return binexp.GetMostSigOp().Accept(thisSerializer).and_then([&binexp, &thisSerializer](gsl::not_null<tinyxml2::XMLElement*> mostSigOp) {
        return binexp.GetLeastSigOp().Accept(thisSerializer).transform([&mostSigOp](gsl::not_null<tinyxml2::XMLElement*> leastSigOp) {
            return std::pair { mostSigOp, leastSigOp };
        });
    });
}

}

#endif // MATHMLSERIALIZER_HPP

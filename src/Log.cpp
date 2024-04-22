//
// Created by Matthew McCall on 10/6/23.
// Modified by Blake Kessler on 10/10/23
//

#include "Oasis/Log.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Expression.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Undefined.hpp"
#include <cmath>

namespace Oasis {
Log<Expression>::Log(const Expression& base, const Expression& argument)
    : BinaryExpression(base, argument)
{
}

auto Log<Expression>::Simplify() const -> std::unique_ptr<Expression>
{
    const auto simplifiedBase = mostSigOp ? mostSigOp->Simplify() : nullptr;
    const auto simplifiedArgument = leastSigOp ? leastSigOp->Simplify() : nullptr;

    if (!simplifiedBase || !simplifiedArgument) {
        return nullptr;
    }

    const Log simplifiedLog { *simplifiedBase, *simplifiedArgument };

    if (const auto realBaseCase = Log<Real, Expression>::Specialize(simplifiedLog); realBaseCase != nullptr) {
        if (const Real& b = realBaseCase->GetMostSigOp(); b.GetValue() <= 0.0 || b.GetValue() == 1) {
            return std::make_unique<Undefined>();
        }
    }

    if (const auto realExponentCase = Log<Expression, Real>::Specialize(simplifiedLog); realExponentCase != nullptr) {
        const Real& argument = realExponentCase->GetLeastSigOp();

        if (argument.GetValue() <= 0.0) {
            return std::make_unique<Undefined>();
        }

        if (argument.GetValue() == 1.0) {
            return std::make_unique<Real>(0.0);
        }
    }

    if (const auto realCase = Log<Real>::Specialize(simplifiedLog); realCase != nullptr) {
        const Real& base = realCase->GetMostSigOp();
        const Real& argument = realCase->GetLeastSigOp();

        return std::make_unique<Real>(log2(argument.GetValue()) * (1 / log2(base.GetValue())));
    }

    // log[a](b^x) = x * log[a](b)
    if (const auto expCase = Log<Expression, Exponent<>>::Specialize(simplifiedLog); expCase != nullptr) {
        const auto exponent = expCase->GetLeastSigOp();
        const IExpression auto& log = Log<Expression>(expCase->GetMostSigOp(), exponent.GetMostSigOp()); // might need to check that it isnt nullptr
        const IExpression auto& factor = exponent.GetLeastSigOp();
        return Oasis::Multiply<Oasis::Expression>(factor, log).Simplify();
    }

    return simplifiedLog.Copy();
}

auto Log<Expression>::Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression>
{
    return Copy(subflow);
}

auto Log<Expression>::ToString() const -> std::string
{
    return fmt::format("log({}, {})", mostSigOp->ToString(), leastSigOp->ToString());
}

tinyxml2::XMLElement* Log<Expression, Expression>::ToMathMLElement(tinyxml2::XMLDocument& doc) const
{
    // mrow
    tinyxml2::XMLElement* const mrow = doc.NewElement("mrow");

    // log
    tinyxml2::XMLElement* const sub = doc.NewElement("msub");

    tinyxml2::XMLElement* const log = doc.NewElement("mi");
    log->SetText("log");

    tinyxml2::XMLElement* const base = leastSigOp->ToMathMLElement(doc);

    sub->InsertFirstChild(log);
    sub->InsertEndChild(base);
    mrow->InsertFirstChild(sub);

    // (
    tinyxml2::XMLElement* const leftParen = doc.NewElement("mo");
    leftParen->SetText("(");

    // )
    tinyxml2::XMLElement* const rightParen = doc.NewElement("mo");
    rightParen->SetText(")");

    mrow->InsertEndChild(leftParen);
    mrow->InsertEndChild(leastSigOp->ToMathMLElement(doc));
    mrow->InsertEndChild(rightParen);

    return mrow;
}

auto Log<Expression>::Specialize(const Expression& other) -> std::unique_ptr<Log>
{
    if (!other.Is<Oasis::Log>()) {
        return nullptr;
    }

    const auto otherGeneralized = other.Generalize();
    return std::make_unique<Log>(dynamic_cast<const Log<Expression>&>(*otherGeneralized));
}

auto Log<Expression>::Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Log>
{
    if (!other.Is<Oasis::Log>()) {
        return nullptr;
    }

    const auto otherGeneralized = other.Generalize(subflow);
    return std::make_unique<Log>(dynamic_cast<const Log<Expression>&>(*otherGeneralized));
}

} // Oasis

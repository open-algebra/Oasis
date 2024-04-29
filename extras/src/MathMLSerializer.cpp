//
// Created by Matthew McCall on 4/28/24.
//

#include <fmt/format.h>

#include "Oasis/MathMLSerializer.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Derivative.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Negate.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"


namespace Oasis {

MathMLSerializer::MathMLSerializer(tinyxml2::XMLDocument& doc) : doc(doc)
{
}

void MathMLSerializer::Serialize(const Real& real)
{
    result = doc.NewElement("mn");
    result->SetText(fmt::format("{:.5}", real.GetValue()).c_str());
}

void MathMLSerializer::Serialize(const Imaginary& imaginary)
{
    result = doc.NewElement("mi");
    result->SetText("i");
}

void MathMLSerializer::Serialize(const Variable& variable)
{
    result = doc.NewElement("mi");
    result->SetText(variable.GetName().c_str());
}

void MathMLSerializer::Serialize(const Undefined& undefined)
{
    tinyxml2::XMLElement* const mtext = doc.NewElement("mtext");
    mtext->SetText("Undefined");
}

void MathMLSerializer::Serialize(const Add<>& add)
{
    tinyxml2::XMLElement* mrow = doc.NewElement("mrow");

    std::vector<std::unique_ptr<Expression>> ops;
    add.Flatten(ops);

    for (const auto& op : ops) {
        op->Serialize(*this);
        tinyxml2::XMLElement* opElement = GetResult();

        mrow->InsertEndChild(opElement);
        // add + mo
        tinyxml2::XMLElement* mo = doc.NewElement("mo");
        mo->SetText("+");
        mrow->InsertEndChild(mo);
    }

    // remove last mo
    mrow->DeleteChild(mrow->LastChild());
    result = mrow;
}

void MathMLSerializer::Serialize(const Subtract<>& subtract)
{
    // mrow
    tinyxml2::XMLElement* const mrow = doc.NewElement("mrow");

    auto [minuendElement, subtrahendElement] = GetOpsAsMathMLPair(subtract);

    mrow->InsertFirstChild(minuendElement);

    // mo
    tinyxml2::XMLElement* const mo = doc.NewElement("mo");
    mo->SetText("-");
    mrow->InsertEndChild(mo);

    // (
    tinyxml2::XMLElement* const leftParen = doc.NewElement("mo");
    leftParen->SetText("(");
    mrow->InsertEndChild(leftParen);

    mrow->InsertEndChild(subtrahendElement);

    // )
    tinyxml2::XMLElement* const rightParen = doc.NewElement("mo");
    rightParen->SetText(")");
    mrow->InsertEndChild(rightParen);

    result = mrow;
}

void MathMLSerializer::Serialize(const Multiply<>& multiply)
{
    tinyxml2::XMLElement* mrow = doc.NewElement("mrow");

    std::vector<std::unique_ptr<Expression>> ops;
    multiply.Flatten(ops);

    bool omitAsterisk = true;

    for (const auto& op : ops) {
        omitAsterisk = op->Is<Real>() || op->Is<Variable>() || op->Is<Exponent>();
        if (!omitAsterisk) break;
    }

    for (const auto& op : ops) {
        op->Serialize(*this);
        tinyxml2::XMLElement* opElement = GetResult();
        mrow->InsertEndChild(opElement);

        if (omitAsterisk) continue;

        tinyxml2::XMLElement* mo = doc.NewElement("mo");
        mo->SetText("*");
        mrow->InsertEndChild(mo);
    }

    if (!omitAsterisk) {
        // remove last mo
        mrow->DeleteChild(mrow->LastChild());
    }

    result = mrow;
}

void MathMLSerializer::Serialize(const Divide<>& divide)
{
    tinyxml2::XMLElement* mfrac = doc.NewElement("mfrac");

    auto [dividendElement, divisorElement] = GetOpsAsMathMLPair(divide);

    mfrac->InsertEndChild(dividendElement);
    mfrac->InsertEndChild(divisorElement);

    result = mfrac;
}

void MathMLSerializer::Serialize(const Exponent<>& exponent)
{
    tinyxml2::XMLElement* msup = doc.NewElement("msup");

    auto [baseElement, powerElement] = GetOpsAsMathMLPair(exponent);

    msup->InsertEndChild(baseElement);
    msup->InsertEndChild(powerElement);

    result = msup;
}

void MathMLSerializer::Serialize(const Log<>& log)
{
    // mrow
    tinyxml2::XMLElement* const mrow = doc.NewElement("mrow");

    // log
    tinyxml2::XMLElement* const msub = doc.NewElement("msub");

    tinyxml2::XMLElement* const logElement = doc.NewElement("mi");
    logElement->SetText("log");

    auto [baseElement, argElement] = GetOpsAsMathMLPair(log);

    msub->InsertFirstChild(logElement);
    msub->InsertEndChild(baseElement);
    mrow->InsertFirstChild(msub);

    // (
    tinyxml2::XMLElement* const leftParen = doc.NewElement("mo");
    leftParen->SetText("(");

    // )
    tinyxml2::XMLElement* const rightParen = doc.NewElement("mo");
    rightParen->SetText(")");

    mrow->InsertEndChild(leftParen);
    mrow->InsertEndChild(argElement);
    mrow->InsertEndChild(rightParen);

    result = mrow;
}

void MathMLSerializer::Serialize(const Negate<Expression>& negate)
{
    // mrow
    tinyxml2::XMLElement* const mrow = doc.NewElement("mrow");

    // mo
    tinyxml2::XMLElement* const mo = doc.NewElement("mo");
    mo->SetText("-");
    mrow->InsertEndChild(mo);

    // (
    tinyxml2::XMLElement* const leftParen = doc.NewElement("mo");
    leftParen->SetText("(");
    mrow->InsertEndChild(leftParen);

    negate.GetOperand().Serialize(*this);
    tinyxml2::XMLElement* operandElement = GetResult();
    mrow->InsertEndChild(operandElement);

    // )
    tinyxml2::XMLElement* const rightParen = doc.NewElement("mo");
    rightParen->SetText(")");
    mrow->InsertEndChild(rightParen);

    result = mrow;
}

void MathMLSerializer::Serialize(const Derivative<>& derivative)
{
    tinyxml2::XMLElement* mrow = doc.NewElement("mrow");

    tinyxml2::XMLElement* mfrac = doc.NewElement("mfrac");

    tinyxml2::XMLElement* dNode = doc.NewElement("mo");
    dNode->SetText("d");

    tinyxml2::XMLElement* dXNode = doc.NewElement("mo");
    dXNode->SetText("d");

    auto [expElement, varElement] = GetOpsAsMathMLPair(derivative);

    tinyxml2::XMLElement* denominator = doc.NewElement("mrow");
    denominator->InsertEndChild(dXNode);
    denominator->InsertEndChild(varElement);

    mfrac->InsertEndChild(dNode);
    mfrac->InsertEndChild(denominator);

    mrow->InsertEndChild(mfrac);

    // (
    tinyxml2::XMLElement* const leftParen = doc.NewElement("mo");
    leftParen->SetText("(");

    // )
    tinyxml2::XMLElement* const rightParen = doc.NewElement("mo");
    rightParen->SetText(")");

    mrow->InsertEndChild(leftParen);
    mrow->InsertEndChild(expElement);
    mrow->InsertEndChild(rightParen);

    result = mrow;
}

tinyxml2::XMLDocument& MathMLSerializer::GetDocument() const
{
    return doc;
}

tinyxml2::XMLElement* MathMLSerializer::GetResult() const
{
    return result;
}

tinyxml2::XMLElement* MathMLSerializer::CreatePlaceholder() const
{
    tinyxml2::XMLElement* mspace = doc.NewElement("mspace");
    mspace->SetAttribute("style", "border: dashed gray 1.5pt;");
    mspace->SetAttribute("width", "1em");
    mspace->SetAttribute("height", "1em");
    return mspace;
}

}
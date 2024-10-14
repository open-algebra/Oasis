//
// Created by Matthew McCall on 4/28/24.
//

#include <fmt/format.h>

#include "Oasis/MathMLSerializer.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Derivative.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Matrix.hpp"
#include "Oasis/Negate.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"
#include "Oasis/EulerNumber.hpp"
#include "Oasis/Pi.hpp"
#include "Oasis/Magnitude.hpp"

namespace Oasis {

MathMLSerializer::MathMLSerializer(tinyxml2::XMLDocument& doc)
    : doc(doc)
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

void MathMLSerializer::Serialize(const Matrix& matrix)
{
    tinyxml2::XMLElement* mrow = doc.NewElement("mrow");

    tinyxml2::XMLElement* openBrace = doc.NewElement("mo");
    openBrace->SetText("[");

    tinyxml2::XMLElement* closeBrace = doc.NewElement("mo");
    closeBrace->SetText("]");

    auto mat = matrix.GetMatrix();

    tinyxml2::XMLElement* table = doc.NewElement("mtable");

    for (int r = 0; r < matrix.GetRows(); r++){
        tinyxml2::XMLElement* row = doc.NewElement("mtr");
        for (int c = 0; c < matrix.GetCols(); c++){
            tinyxml2::XMLElement* mtd = doc.NewElement("mtd");
            tinyxml2::XMLElement* mn = doc.NewElement("mn");
            mn->SetText(mat(r,c));
            mtd->InsertEndChild(mn);
            row->InsertEndChild(mtd);
        }
        table->InsertEndChild(row);
    }
    mrow->InsertEndChild(openBrace);
    mrow->InsertEndChild(table);
    mrow->InsertEndChild(closeBrace);

    result = mrow;
}

void MathMLSerializer::Serialize(const Oasis::Pi&)
{
    result = doc.NewElement("mi");
    result->SetText("&pi;");
}

void MathMLSerializer::Serialize(const Oasis::EulerNumber&)
{
    result = doc.NewElement("mi");
    result->SetText("e");
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

    bool surroundWithParens = true;

    if (subtract.HasLeastSigOp()) {
        if (auto realSubtrahend = Real::Specialize(subtract.GetLeastSigOp()); realSubtrahend != nullptr) {
            if (realSubtrahend->GetValue() >= 0) {
                surroundWithParens = false;
            }
        }
    }

    // mo
    tinyxml2::XMLElement* const mo = doc.NewElement("mo");
    mo->SetText("-");
    mrow->InsertEndChild(mo);

    if (surroundWithParens) {
        // (
        tinyxml2::XMLElement* const leftParen = doc.NewElement("mo");
        leftParen->SetText("(");
        mrow->InsertEndChild(leftParen);
    }

    mrow->InsertEndChild(subtrahendElement);

    if (surroundWithParens) {
        // )
        tinyxml2::XMLElement* const rightParen = doc.NewElement("mo");
        rightParen->SetText(")");
        mrow->InsertEndChild(rightParen);
    }

    result = mrow;
}

void MathMLSerializer::Serialize(const Multiply<>& multiply)
{
    tinyxml2::XMLElement* mrow = doc.NewElement("mrow");

    std::vector<std::unique_ptr<Expression>> ops;
    multiply.Flatten(ops);

    bool surroundWithParens = !ops[0]->Is<Real>() && !ops[0]->Is<Variable>() && !ops[0]->Is<Exponent>() && !ops[0]->Is<Log>();

    if (surroundWithParens) {
        tinyxml2::XMLElement* const leftParen = doc.NewElement("mo");
        leftParen->SetText("(");
        mrow->InsertEndChild(leftParen);
    }

    ops.front()->Serialize(*this);
    tinyxml2::XMLElement* firstOpElement = GetResult();
    mrow->InsertEndChild(firstOpElement);

    if (surroundWithParens) {
        tinyxml2::XMLElement* const rightParen = doc.NewElement("mo");
        rightParen->SetText(")");
        mrow->InsertEndChild(rightParen);
    }

    for (int i = 1; i < ops.size(); ++i) {
        const auto& leftOp = ops[i - 1];
        const auto& rightOp = ops[i];

        tinyxml2::XMLElement* mo = doc.NewElement("mo");
        mo->SetText("\u00D7");

        if (leftOp->Is<Real>() && rightOp->Is<Real>()) {
            mrow->InsertEndChild(mo);
        } else if (leftOp->Is<Variable>() && rightOp->Is<Real>()) {
            doc.DeleteNode(mo);
        } else {
            doc.DeleteNode(mo);
        }

        surroundWithParens = !ops[i]->Is<Real>() && !ops[i]->Is<Variable>() && !ops[i]->Is<Exponent>() && !ops[i]->Is<Log>();

        if (surroundWithParens) {
            tinyxml2::XMLElement* const leftParen = doc.NewElement("mo");
            leftParen->SetText("(");
            mrow->InsertEndChild(leftParen);
        }

        ops[i]->Serialize(*this);
        tinyxml2::XMLElement* opElement = GetResult();
        mrow->InsertEndChild(opElement);

        if (surroundWithParens) {
            tinyxml2::XMLElement* const rightParen = doc.NewElement("mo");
            rightParen->SetText(")");
            mrow->InsertEndChild(rightParen);
        }
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

    if (exponent.HasMostSigOp() && !exponent.GetMostSigOp().Is<Real>() && !exponent.GetMostSigOp().Is<Variable>()) {
        // (
        tinyxml2::XMLElement* const leftParen = doc.NewElement("mo");
        leftParen->SetText("(");

        // )
        tinyxml2::XMLElement* const rightParen = doc.NewElement("mo");
        rightParen->SetText(")");

        tinyxml2::XMLElement* mrow = doc.NewElement("mrow");

        mrow->InsertEndChild(leftParen);
        mrow->InsertEndChild(baseElement);
        mrow->InsertEndChild(rightParen);

        msup->InsertEndChild(mrow);
    } else {
        msup->InsertEndChild(baseElement);
    }

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

void MathMLSerializer::Serialize(const Integral<>& integral)
{
    tinyxml2::XMLElement* mrow = doc.NewElement("mrow");

    // Integral symbol
    tinyxml2::XMLElement* inte = doc.NewElement("mo");
    inte->SetText("\u222B");

    tinyxml2::XMLElement* dNode = doc.NewElement("mo");
    dNode->SetText("d");

    auto [expElement, varElement] = GetOpsAsMathMLPair(integral);

    // d variable
    tinyxml2::XMLElement* dVar = doc.NewElement("mrow");
    dVar->InsertEndChild(dNode);
    dVar->InsertEndChild(varElement);

    mrow->InsertEndChild(inte);
    mrow->InsertEndChild(expElement);
    mrow->InsertEndChild(dVar);

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
void MathMLSerializer::Serialize(const Magnitude<Expression>& magnitude)
{
    // mrow
    tinyxml2::XMLElement* const mrow = doc.NewElement("mrow");

    // (
    tinyxml2::XMLElement* const leftParen = doc.NewElement("mo");
    leftParen->SetText("|");
    mrow->InsertEndChild(leftParen);

    magnitude.GetOperand().Serialize(*this);
    tinyxml2::XMLElement* operandElement = GetResult();
    mrow->InsertEndChild(operandElement);

    // )
    tinyxml2::XMLElement* const rightParen = doc.NewElement("mo");
    rightParen->SetText("|");
    mrow->InsertEndChild(rightParen);

    result = mrow;
}
void MathMLSerializer::Serialize(const Sine<Expression>& sine)
{
    result = nullptr;
}

}
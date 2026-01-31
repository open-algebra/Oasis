//
// Created by Matthew McCall on 4/28/24.
//

#include <format>

#include "Oasis/MathMLSerializer.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Derivative.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/EulerNumber.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Magnitude.hpp"
#include "Oasis/Matrix.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Negate.hpp"
#include "Oasis/Pi.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"

namespace Oasis {

MathMLSerializer::MathMLSerializer(tinyxml2::XMLDocument& doc)
    : doc(doc)
{
}

auto MathMLSerializer::TypedVisit(const Real& real) -> RetT
{
    tinyxml2::XMLElement* result = doc.NewElement("mn");
    result->SetText(std::format("{:.5}", real.GetValue()).c_str());
    return gsl::not_null(result);
}

auto MathMLSerializer::TypedVisit(const Imaginary&) -> RetT
{
    tinyxml2::XMLElement* result = doc.NewElement("mi");
    result->SetText("i");
    return gsl::not_null(result);
}

auto MathMLSerializer::TypedVisit(const Matrix& matrix) -> RetT
{
    tinyxml2::XMLElement* mrow = doc.NewElement("mrow");

    tinyxml2::XMLElement* openBrace = doc.NewElement("mo");
    openBrace->SetText("[");

    tinyxml2::XMLElement* closeBrace = doc.NewElement("mo");
    closeBrace->SetText("]");

    auto mat = matrix.GetMatrix();

    tinyxml2::XMLElement* table = doc.NewElement("mtable");

    for (size_t r = 0; r < matrix.GetRows(); r++) {
        tinyxml2::XMLElement* row = doc.NewElement("mtr");
        for (size_t c = 0; c < matrix.GetCols(); c++) {
            tinyxml2::XMLElement* mtd = doc.NewElement("mtd");
            tinyxml2::XMLElement* mn = doc.NewElement("mn");
            mn->SetText(mat(r, c));
            mtd->InsertEndChild(mn);
            row->InsertEndChild(mtd);
        }
        table->InsertEndChild(row);
    }
    mrow->InsertEndChild(openBrace);
    mrow->InsertEndChild(table);
    mrow->InsertEndChild(closeBrace);

    return gsl::not_null(mrow);
}

auto MathMLSerializer::TypedVisit(const Oasis::Pi&) -> RetT
{
    tinyxml2::XMLElement* result = doc.NewElement("mi");
    result->SetText("&pi;");
    return gsl::not_null(result);
}

auto MathMLSerializer::TypedVisit(const Oasis::EulerNumber&) -> RetT
{
    tinyxml2::XMLElement* result = doc.NewElement("mi");
    result->SetText("e");
    return gsl::not_null(result);
}

auto MathMLSerializer::TypedVisit(const Variable& variable) -> RetT
{
    tinyxml2::XMLElement* result = doc.NewElement("mi");
    result->SetText(variable.GetName().c_str());
    return gsl::not_null(result);
}

auto MathMLSerializer::TypedVisit(const Undefined&) -> RetT
{
    tinyxml2::XMLElement* const mtext = doc.NewElement("mtext");
    mtext->SetText("Undefined");
    return gsl::not_null(mtext);
}

auto MathMLSerializer::TypedVisit(const Add<>& add) -> RetT
{
    tinyxml2::XMLElement* mrow = doc.NewElement("mrow");

    std::vector<std::unique_ptr<Expression>> ops;
    add.Flatten(ops);

    for (const auto& op : ops) {
        const auto opElement = op->Accept(*this).value();

        mrow->InsertEndChild(opElement);
        // add + mo
        tinyxml2::XMLElement* mo = doc.NewElement("mo");
        mo->SetText("+");
        mrow->InsertEndChild(mo);
    }

    // remove last mo
    mrow->DeleteChild(mrow->LastChild());
    return gsl::not_null(mrow);
}

auto MathMLSerializer::TypedVisit(const Subtract<>& subtract) -> RetT
{
    return GetOpsAsMathMLPair(subtract).transform([this, subtract](const auto& ops) {
        // mrow
        tinyxml2::XMLElement* const mrow = doc.NewElement("mrow");
        auto [minuendElement, subtrahendElement] = ops;

        mrow->InsertFirstChild(minuendElement);

        bool surroundWithParens = true;

        if (subtract.HasLeastSigOp()) {
            if (auto realSubtrahend = RecursiveCast<Real>(subtract.GetLeastSigOp()); realSubtrahend != nullptr) {
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

        return gsl::not_null(mrow);
    });
}

auto MathMLSerializer::TypedVisit(const Multiply<>& multiply) -> RetT
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

    const auto firstOpElement = ops.front()->Accept(*this).value();
    mrow->InsertEndChild(firstOpElement);

    if (surroundWithParens) {
        tinyxml2::XMLElement* const rightParen = doc.NewElement("mo");
        rightParen->SetText(")");
        mrow->InsertEndChild(rightParen);
    }

    for (size_t i = 1; i < ops.size(); ++i) {
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

        tinyxml2::XMLElement* opElement = ops[i]->Accept(*this).value();
        mrow->InsertEndChild(opElement);

        if (surroundWithParens) {
            tinyxml2::XMLElement* const rightParen = doc.NewElement("mo");
            rightParen->SetText(")");
            mrow->InsertEndChild(rightParen);
        }
    }

    return gsl::not_null(mrow);
}

auto MathMLSerializer::TypedVisit(const Divide<>& divide) -> RetT
{
    return GetOpsAsMathMLPair(divide).transform([this, divide](const auto& ops) {
        tinyxml2::XMLElement* mfrac = doc.NewElement("mfrac");

        auto [dividendElement, divisorElement] = ops;

        mfrac->InsertEndChild(dividendElement);
        mfrac->InsertEndChild(divisorElement);

        return gsl::not_null(mfrac);
    });
}

auto MathMLSerializer::TypedVisit(const Exponent<>& exponent) -> RetT
{
    return GetOpsAsMathMLPair(exponent).transform([this, exponent](const auto& ops) {
        tinyxml2::XMLElement* msup = doc.NewElement("msup");

        auto [baseElement, powerElement] = ops;

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

        return gsl::not_null(msup);
    });
}

auto MathMLSerializer::TypedVisit(const Log<>& log) -> RetT
{
    return GetOpsAsMathMLPair(log).transform([this, log](const auto& ops) {
        // mrow
        tinyxml2::XMLElement* const mrow = doc.NewElement("mrow");

        // log
        tinyxml2::XMLElement* const msub = doc.NewElement("msub");

        tinyxml2::XMLElement* const logElement = doc.NewElement("mi");
        logElement->SetText("log");

        auto [baseElement, argElement] = ops;

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

        return gsl::not_null(mrow);
    });
}

auto MathMLSerializer::TypedVisit(const Negate<Expression>& negate) -> RetT
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

    const auto operandElement = negate.GetOperand().Accept(*this).value();
    mrow->InsertEndChild(operandElement);

    // )
    tinyxml2::XMLElement* const rightParen = doc.NewElement("mo");
    rightParen->SetText(")");
    mrow->InsertEndChild(rightParen);

    return gsl::not_null(mrow);
}

auto MathMLSerializer::TypedVisit(const Sine<Expression>&) -> RetT
{
    // mrow
    tinyxml2::XMLElement* const mrow = doc.NewElement("mrow");
    return gsl::not_null(mrow);
}

auto MathMLSerializer::TypedVisit(const Derivative<>& derivative) -> RetT
{
    return GetOpsAsMathMLPair(derivative).transform([this, derivative](const auto& ops) {
        tinyxml2::XMLElement* mrow = doc.NewElement("mrow");

        tinyxml2::XMLElement* mfrac = doc.NewElement("mfrac");

        tinyxml2::XMLElement* dNode = doc.NewElement("mo");
        dNode->SetText("d");

        tinyxml2::XMLElement* dXNode = doc.NewElement("mo");
        dXNode->SetText("d");

        auto [expElement, varElement] = ops;

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

        return gsl::not_null(mrow);
    });
}

auto MathMLSerializer::TypedVisit(const Integral<>& integral) -> RetT
{
    return GetOpsAsMathMLPair(integral).transform([this, integral](const auto& ops) {
        tinyxml2::XMLElement* mrow = doc.NewElement("mrow");

        // Integral symbol
        tinyxml2::XMLElement* inte = doc.NewElement("mo");
        inte->SetText("&int;");

        tinyxml2::XMLElement* dNode = doc.NewElement("mo");
        dNode->SetText("d");

        auto [expElement, varElement] = ops;

        // d variable
        tinyxml2::XMLElement* dVar = doc.NewElement("mrow");
        dVar->InsertEndChild(dNode);
        dVar->InsertEndChild(varElement);

        mrow->InsertEndChild(inte);
        mrow->InsertEndChild(expElement);
        mrow->InsertEndChild(dVar);

        return gsl::not_null(mrow);
    });
}

tinyxml2::XMLDocument& MathMLSerializer::GetDocument() const
{
    return doc;
}

auto MathMLSerializer::TypedVisit(const Magnitude<Expression>& magnitude) -> RetT
{
    // mrow
    tinyxml2::XMLElement* const mrow = doc.NewElement("mrow");

    // (
    tinyxml2::XMLElement* const leftParen = doc.NewElement("mo");
    leftParen->SetText("|");
    mrow->InsertEndChild(leftParen);

    const auto operandElement = magnitude.GetOperand().Accept(*this).value();
    mrow->InsertEndChild(operandElement);

    // )
    tinyxml2::XMLElement* const rightParen = doc.NewElement("mo");
    rightParen->SetText("|");
    mrow->InsertEndChild(rightParen);

    return gsl::not_null(mrow);
}

}
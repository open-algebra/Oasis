//
// Created by Matthew McCall on 4/28/24.
//

#include <fmt/core.h>

#include "Oasis/InFixSerializer.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Derivative.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Negate.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"

namespace Oasis {

void InFixSerializer::Visit(const Real& real)
{
    result = fmt::format("{:.5}", real.GetValue());
}

void InFixSerializer::Visit(const Imaginary& imaginary)
{
    result = "i";
}

void InFixSerializer::Visit(const Variable& variable)
{
    result = variable.GetName();
}

void InFixSerializer::Visit(const Undefined& undefined)
{
    result = "Undefined";
}

void InFixSerializer::Visit(const Add<>& add)
{
    add.GetMostSigOp().Accept(*this);
    const auto mostSigOpStr = getResult();

    add.GetLeastSigOp().Accept(*this);
    const auto leastSigOpStr = getResult();

    result = fmt::format("({}+{})", mostSigOpStr, leastSigOpStr);
}

void InFixSerializer::Visit(const Subtract<>& subtract)
{
    subtract.GetMostSigOp().Accept(*this);
    const auto mostSigOpStr = getResult();

    subtract.GetLeastSigOp().Accept(*this);
    const auto leastSigOpStr = getResult();

    result = fmt::format("({}-{})", mostSigOpStr, leastSigOpStr);
}

void InFixSerializer::Visit(const Multiply<>& multiply)
{
    multiply.GetMostSigOp().Accept(*this);
    const auto mostSigOpStr = getResult();

    multiply.GetLeastSigOp().Accept(*this);
    const auto leastSigOpStr = getResult();

    result = fmt::format("({}*{})", mostSigOpStr, leastSigOpStr);
}

void InFixSerializer::Visit(const Divide<>& divide)
{
    divide.GetMostSigOp().Accept(*this);
    const auto mostSigOpStr = getResult();

    divide.GetLeastSigOp().Accept(*this);
    const auto leastSigOpStr = getResult();

    result = fmt::format("({}/{})", mostSigOpStr, leastSigOpStr);
}

void InFixSerializer::Visit(const Exponent<>& exponent)
{
    exponent.GetMostSigOp().Accept(*this);
    const auto mostSigOpStr = getResult();

    exponent.GetLeastSigOp().Accept(*this);
    const auto leastSigOpStr = getResult();

    result = fmt::format("({}^{})", mostSigOpStr, leastSigOpStr);
}

void InFixSerializer::Visit(const Log<>& log)
{
    log.GetMostSigOp().Accept(*this);
    const auto mostSigOpStr = getResult();

    log.GetLeastSigOp().Accept(*this);
    const auto leastSigOpStr = getResult();

    result = fmt::format("log({},{})", mostSigOpStr, leastSigOpStr);
}

void InFixSerializer::Visit(const Negate<Expression>& negate)
{
    negate.GetOperand().Accept(*this);
    const auto opStr = getResult();

    result = fmt::format("-({})", opStr);
}

void InFixSerializer::Visit(const Derivative<>& derivative)
{
    derivative.GetMostSigOp().Accept(*this);
    const auto mostSigOpStr = getResult();

    derivative.GetLeastSigOp().Accept(*this);
    const auto leastSigOpStr = getResult();

    result = fmt::format("dd({},{})", mostSigOpStr, leastSigOpStr);
}

void InFixSerializer::Visit(const Integral<>& integral)
{
    integral.GetMostSigOp().Accept(*this);
    const auto mostSigOpStr = getResult();

    integral.GetLeastSigOp().Accept(*this);
    const auto leastSigOpStr = getResult();

    result = fmt::format("in({},{})", mostSigOpStr, leastSigOpStr);
}

std::string InFixSerializer::getResult() const
{
    return result;
}

} // Oasis
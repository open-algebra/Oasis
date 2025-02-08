//
// Created by Matthew McCall on 4/28/24.
//

#include <format>

#include "Oasis/InFixSerializer.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Derivative.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Magnitude.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Negate.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"

namespace Oasis {

std::any InFixSerializer::Visit(const Real& real)
{
    return std::format("{:.5}", real.GetValue());
}

std::any InFixSerializer::Visit(const Imaginary&)
{
    return "i";
}

std::any InFixSerializer::Visit(const Variable& variable)
{
    return variable.GetName();
}

std::any InFixSerializer::Visit(const Undefined&)
{
    return "Undefined";
}

std::any InFixSerializer::Visit(const Add<>& add)
{
    auto result = SerializeArithBinExp(add, "+");
    return result ? result.value() : std::any {};
}

std::any InFixSerializer::Visit(const Subtract<>& subtract)
{
    auto result = SerializeArithBinExp(subtract, "-");
    return result ? result.value() : std::any {};
}

std::any InFixSerializer::Visit(const Multiply<>& multiply)
{
    auto result = SerializeArithBinExp(multiply, "*");
    return result ? result.value() : std::any {};
}

std::any InFixSerializer::Visit(const Divide<>& divide)
{
    auto result = SerializeArithBinExp(divide, "/");
    return result ? result.value() : std::any {};
}

std::any InFixSerializer::Visit(const Exponent<>& exponent)
{
    auto result = SerializeArithBinExp(exponent, "^");
    return result ? result.value() : std::any {};
}

std::any InFixSerializer::Visit(const Log<>& log)
{
    auto result = SerializeArithBinExp(log, "log");
    return result ? result.value() : std::any {};
}

std::any InFixSerializer::Visit(const Negate<Expression>& negate)
{
    auto opStr = negate.GetOperand().Accept(*this);
    return opStr ? std::format("-({})", opStr.value()) : std::any {};
}

std::any InFixSerializer::Visit(const Derivative<>& derivative)
{
    auto result = SerializeFuncBinExp(derivative, "dd");
    return result ? result.value() : std::any {};
}

std::any InFixSerializer::Visit(const Integral<>& integral)
{
    auto result = SerializeFuncBinExp(integral, "in");
    return result ? result.value() : std::any {};
}

std::any InFixSerializer::Visit(const Matrix&)
{
    return "NOT IMPLEMENTED";
}

std::any InFixSerializer::Visit(const EulerNumber&)
{
    return "e";
}

std::any InFixSerializer::Visit(const Pi&)
{
    return "pi";
}

std::any InFixSerializer::Visit(const Magnitude<Expression>& magnitude)
{
    const auto opStr = magnitude.GetOperand().Accept(*this);
    return opStr ? std::format("|({})|", opStr.value()) : std::any {};
}

} // Oasis
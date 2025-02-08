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
#include "Oasis/Magnitude.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Negate.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"

namespace Oasis {

std::any InFixSerializer::Visit(const Real& real)
{
    return fmt::format("{:.5}", real.GetValue());
}

std::any InFixSerializer::Visit(const Imaginary& imaginary)
{
    return "i";
}

std::any InFixSerializer::Visit(const Variable& variable)
{
    return variable.GetName();
}

std::any InFixSerializer::Visit(const Undefined& undefined)
{
    return "Undefined";
}

std::any InFixSerializer::Visit(const Add<>& add)
{
    return SerializeArithBinExp(add, "+");
}

std::any InFixSerializer::Visit(const Subtract<>& subtract)
{
    return SerializeArithBinExp(subtract, "-");
}

std::any InFixSerializer::Visit(const Multiply<>& multiply)
{
    return SerializeArithBinExp(multiply, "*");
}

std::any InFixSerializer::Visit(const Divide<>& divide)
{
    return SerializeArithBinExp(divide, "/");
}

std::any InFixSerializer::Visit(const Exponent<>& exponent)
{
    return SerializeArithBinExp(exponent, "^");
}

std::any InFixSerializer::Visit(const Log<>& log)
{
    return SerializeArithBinExp(log, "log");
}

std::any InFixSerializer::Visit(const Negate<Expression>& negate)
{
    try {
        const auto opStr = std::any_cast<std::string>(negate.GetOperand().Accept(*this));
        return fmt::format("-({})", opStr);
    } catch (std::bad_any_cast&) {
        return {};
    }
}

std::any InFixSerializer::Visit(const Derivative<>& derivative)
{
    return SerializeFuncBinExp(derivative, "dd");
}

std::any InFixSerializer::Visit(const Integral<>& integral)
{
    return SerializeFuncBinExp(integral, "in");
}

std::any InFixSerializer::Visit(const Matrix& matrix)
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
    const auto opStr = std::any_cast<std::string>(magnitude.GetOperand().Accept(*this));
    return fmt::format("|({})|", opStr);
}

} // Oasis
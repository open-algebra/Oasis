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
#include "Oasis/Sine.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"

namespace Oasis {

auto InFixSerializer::TypedVisit(const Real& real) -> RetT
{
    return std::format("{:.5}", real.GetValue());
}

auto InFixSerializer::TypedVisit(const Imaginary&) -> RetT
{
    return "i";
}

auto InFixSerializer::TypedVisit(const Variable& variable) -> RetT
{
    return variable.GetName();
}

auto InFixSerializer::TypedVisit(const Undefined&) -> RetT
{
    return "Undefined";
}

auto InFixSerializer::TypedVisit(const Add<>& add) -> RetT
{
    return SerializeArithBinExp(add, "+");
}

auto InFixSerializer::TypedVisit(const Subtract<>& subtract) -> RetT
{
    return SerializeArithBinExp(subtract, "-");
}

auto InFixSerializer::TypedVisit(const Multiply<>& multiply) -> RetT
{
    return SerializeArithBinExp(multiply, "*");
}

auto InFixSerializer::TypedVisit(const Divide<>& divide) -> RetT
{
    return SerializeArithBinExp(divide, "/");
}

auto InFixSerializer::TypedVisit(const Exponent<>& exponent) -> RetT
{
    return SerializeArithBinExp(exponent, "^");
}

auto InFixSerializer::TypedVisit(const Log<>& log) -> RetT
{
    return SerializeFuncBinExp(log, "log");
}

auto InFixSerializer::TypedVisit(const Negate<Expression>& negate) -> RetT
{
    return negate.GetOperand().Accept(*this).transform([](const std::string& str) {
        return std::format("-({})", str);
    });
}

auto InFixSerializer::TypedVisit(const Sine<Expression>& sine) -> RetT
{
    return sine.GetOperand().Accept(*this).transform([](const std::string& str) {
        return std::format("sine({})", str);
    });
}

auto InFixSerializer::TypedVisit(const Derivative<>& derivative) -> RetT
{
    return SerializeFuncBinExp(derivative, "dd");
}

auto InFixSerializer::TypedVisit(const Integral<>& integral) -> RetT
{
    return SerializeFuncBinExp(integral, "in");
}

auto InFixSerializer::TypedVisit(const Matrix&) -> RetT
{
    return std::unexpected { "NOT IMPLEMENTED" };
}

auto InFixSerializer::TypedVisit(const EulerNumber&) -> RetT
{
    return "e";
}

auto InFixSerializer::TypedVisit(const Pi&) -> RetT
{
    return "pi";
}

auto InFixSerializer::TypedVisit(const Magnitude<Expression>& magnitude) -> RetT
{
    return magnitude.GetOperand().Accept(*this).transform([](const std::string& str) {
        return std::format("|{}|", str);
    });
}

} // Oasis
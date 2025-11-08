//
// Created by codin on 10/31/25.
//

#include <utility>

#include "Oasis/PALMSerializer.hpp"

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

auto PALMSerializer::TypedVisit(const Real& real) -> RetT
{
    auto realString = std::format("{:.{}}", real.GetValue(), palmOptions.numPlaces + 1);
    return SerializeExpression(ExpressionType::Real, { realString });
}

auto PALMSerializer::TypedVisit(const Imaginary&) -> RetT
{
    return SerializeExpression(ExpressionType::Imaginary);
}

auto PALMSerializer::TypedVisit(const Variable& variable) -> RetT
{
   return SerializeExpression(ExpressionType::Variable, { variable.GetName() });
}

auto PALMSerializer::TypedVisit(const Undefined&) -> RetT
{
   return SerializeExpression(ExpressionType::None);
}

auto PALMSerializer::TypedVisit(const Add<>& add) -> RetT
{
    return SerializeBinaryExpression(add);
}


auto PALMSerializer::TypedVisit(const Subtract<>& subtract) -> RetT
{
    return SerializeBinaryExpression(subtract);
}

auto PALMSerializer::TypedVisit(const Multiply<>& multiply) -> RetT
{
    return SerializeBinaryExpression(multiply);
}

auto PALMSerializer::TypedVisit(const Divide<>& divide) -> RetT
{
    return SerializeBinaryExpression(divide);
}

auto PALMSerializer::TypedVisit(const Exponent<>& exponent) -> RetT
{
    return SerializeBinaryExpression(exponent);
}

auto PALMSerializer::TypedVisit(const Log<>& log) -> RetT
{
    return SerializeBinaryExpression(log);
}

auto PALMSerializer::TypedVisit(const Negate<>& negate) -> RetT
{
    return SerializeUnaryExpression(negate);
}

auto PALMSerializer::TypedVisit(const Derivative<>& derivative) -> RetT
{
    return SerializeBinaryExpression(derivative);
}

auto PALMSerializer::TypedVisit(const Integral<>& integral) -> RetT
{
    return SerializeBinaryExpression(integral);
}

auto PALMSerializer::TypedVisit(const Matrix& matrix) -> RetT
{
    return std::unexpected<std::string> { "Not implemented yet" };
}

auto PALMSerializer::TypedVisit(const EulerNumber&) -> RetT
{
    return SerializeExpression(ExpressionType::EulerNumber);
}

auto PALMSerializer::TypedVisit(const Pi&) -> RetT
{
    return SerializeExpression(ExpressionType::Pi);
}

auto PALMSerializer::TypedVisit(const Magnitude<Expression>& magnitude) -> RetT
{
    return SerializeUnaryExpression(magnitude);
}

}
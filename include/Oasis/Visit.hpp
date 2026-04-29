//
// Created by Matthew McCall on 4/28/24.
//

#ifndef OASIS_SERIALIZATION_HPP
#define OASIS_SERIALIZATION_HPP

#include "Expression.hpp"
#include "FwdDecls.hpp"

namespace Oasis {

class Visitor {
public:
    virtual any Visit(const Real& real) = 0;
    virtual any Visit(const Imaginary& imaginary) = 0;
    virtual any Visit(const Matrix& matrix) = 0;
    virtual any Visit(const Variable& variable) = 0;
    virtual any Visit(const Undefined& undefined) = 0;
    virtual any Visit(const EulerNumber&) = 0;
    virtual any Visit(const Pi&) = 0;
    virtual any Visit(const Add<Expression, Expression>& add) = 0;
    virtual any Visit(const Subtract<Expression, Expression>& subtract) = 0;
    virtual any Visit(const Multiply<Expression, Expression>& multiply) = 0;
    virtual any Visit(const Divide<Expression, Expression>& divide) = 0;
    virtual any Visit(const Exponent<Expression, Expression>& exponent) = 0;
    virtual any Visit(const Log<Expression, Expression>& log) = 0;
    virtual any Visit(const Negate<Expression>& negate) = 0;
    virtual any Visit(const Sine<Expression>& sine) = 0;
    virtual any Visit(const Magnitude<Expression>& magnitude) = 0;
    virtual any Visit(const Derivative<Expression, Expression>& derivative) = 0;
    virtual any Visit(const Integral<Expression, Expression>& integral) = 0;

    virtual ~Visitor() = default;
};

template <typename T>
class TypedVisitor : public Visitor {
public:
    using RetT = T;

    auto Visit(const Real& real) -> any final { return TypedVisit(real); }
    auto Visit(const Imaginary& imaginary) -> any final { return TypedVisit(imaginary); }
    auto Visit(const Matrix& matrix) -> any final { return TypedVisit(matrix); }
    auto Visit(const Variable& variable) -> any final { return TypedVisit(variable); }
    auto Visit(const Undefined& undefined) -> any final { return TypedVisit(undefined); }
    auto Visit(const EulerNumber& e) -> any final { return TypedVisit(e); }
    auto Visit(const Pi& pi) -> any final { return TypedVisit(pi); }
    auto Visit(const Add<Expression, Expression>& add) -> any final { return TypedVisit(add); }
    auto Visit(const Subtract<Expression, Expression>& subtract) -> any final { return TypedVisit(subtract); }
    auto Visit(const Multiply<Expression, Expression>& multiply) -> any final { return TypedVisit(multiply); }
    auto Visit(const Divide<Expression, Expression>& divide) -> any final { return TypedVisit(divide); }
    auto Visit(const Exponent<Expression, Expression>& exponent) -> any final { return TypedVisit(exponent); }
    auto Visit(const Log<Expression, Expression>& log) -> any final { return TypedVisit(log); }
    auto Visit(const Negate<Expression>& negate) -> any final { return TypedVisit(negate); }
    auto Visit(const Sine<Expression>& sine) -> any final { return TypedVisit(sine); }
    auto Visit(const Magnitude<Expression>& magnitude) -> any final { return TypedVisit(magnitude); }
    auto Visit(const Derivative<Expression, Expression>& derivative) -> any final { return TypedVisit(derivative); }
    auto Visit(const Integral<Expression, Expression>& integral) -> any final { return TypedVisit(integral); }

protected:
    virtual auto TypedVisit(const Real& real) -> RetT = 0;
    virtual auto TypedVisit(const Imaginary& imaginary) -> RetT = 0;
    virtual auto TypedVisit(const Matrix& matrix) -> RetT = 0;
    virtual auto TypedVisit(const Variable& variable) -> RetT = 0;
    virtual auto TypedVisit(const Undefined& undefined) -> RetT = 0;
    virtual auto TypedVisit(const EulerNumber&) -> RetT = 0;
    virtual auto TypedVisit(const Pi&) -> RetT = 0;
    virtual auto TypedVisit(const Add<Expression, Expression>& add) -> RetT = 0;
    virtual auto TypedVisit(const Subtract<Expression, Expression>& subtract) -> RetT = 0;
    virtual auto TypedVisit(const Multiply<Expression, Expression>& multiply) -> RetT = 0;
    virtual auto TypedVisit(const Divide<Expression, Expression>& divide) -> RetT = 0;
    virtual auto TypedVisit(const Exponent<Expression, Expression>& exponent) -> RetT = 0;
    virtual auto TypedVisit(const Log<Expression, Expression>& log) -> RetT = 0;
    virtual auto TypedVisit(const Negate<Expression>& negate) -> RetT = 0;
    virtual auto TypedVisit(const Sine<Expression>& sine) -> RetT = 0;
    virtual auto TypedVisit(const Magnitude<Expression>& magnitude) -> RetT = 0;
    virtual auto TypedVisit(const Derivative<Expression, Expression>& derivative) -> RetT = 0;
    virtual auto TypedVisit(const Integral<Expression, Expression>& integral) -> RetT = 0;
};

}

#endif // OASIS_SERIALIZATION_HPP

//
// Created by Andrew Nazareth on 9/23/25.
//

#ifndef DIFFERENTIATEVISITOR_HPP
#define DIFFERENTIATEVISITOR_HPP

#include <format>
#include <string>

#include <gsl/gsl-lite.hpp>

#include "Oasis/Visit.hpp"

namespace Oasis {

struct DifferentiationOpts {
    enum class Multivariate {
        SINGLE_VARIABLE,
        MULTIVARIABLE,
    } multivariate
        = Multivariate::SINGLE_VARIABLE;
};

class DifferentiateVisitor final : public TypedVisitor<std::expected<gsl::not_null<std::unique_ptr<Expression>>, std::string>> {
public:
    explicit DifferentiateVisitor(const std::unique_ptr<Expression>& differentiationVariable);
    explicit DifferentiateVisitor(const std::unique_ptr<Expression>& differentiationVariable, const DifferentiationOpts& opts);

    auto TypedVisit(const Real& real) -> RetT override;
    auto TypedVisit(const Imaginary& imaginary) -> RetT override;
    auto TypedVisit(const Variable& variable) -> RetT override;
    auto TypedVisit(const Undefined& undefined) -> RetT override;
    auto TypedVisit(const Add<Expression, Expression>& add) -> RetT override;
    auto TypedVisit(const Subtract<Expression, Expression>& subtract) -> RetT override;
    auto TypedVisit(const Multiply<Expression, Expression>& multiply) -> RetT override;
    auto TypedVisit(const Divide<Expression, Expression>& divide) -> RetT override;
    auto TypedVisit(const Exponent<Expression, Expression>& exponent) -> RetT override;
    auto TypedVisit(const Log<Expression, Expression>& log) -> RetT override;
    auto TypedVisit(const Negate<Expression>& negate) -> RetT override;
    auto TypedVisit(const Sine<Expression>& sine) -> RetT override;
    auto TypedVisit(const Derivative<Expression, Expression>& derivative) -> RetT override;
    auto TypedVisit(const Integral<Expression, Expression>& integral) -> RetT override;
    auto TypedVisit(const Matrix& matrix) -> RetT override;
    auto TypedVisit(const EulerNumber&) -> RetT override;
    auto TypedVisit(const Pi&) -> RetT override;
    auto TypedVisit(const Magnitude<Expression>& magnitude) -> RetT override;

private:
    std::unique_ptr<Expression> differentiationVariable;
    DifferentiationOpts opts;
};

} // Oasis

#endif // DIFFERENTIATEVISITOR_HPP

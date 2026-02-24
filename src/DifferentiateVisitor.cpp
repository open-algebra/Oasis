//
// Created by Andrew Nazareth on 9/23/25.
//

#include <format>

#include "Oasis/Add.hpp"
#include "Oasis/Derivative.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/DifferentiateVisitor.hpp"
#include "Oasis/EulerNumber.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Magnitude.hpp"
#include "Oasis/MatchCast.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Negate.hpp"
#include "Oasis/Pi.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Sine.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Undefined.hpp"
#include "Oasis/Variable.hpp"

namespace {
constexpr auto EPSILON = std::numeric_limits<float>::epsilon();
}

namespace Oasis {

DifferentiateVisitor::DifferentiateVisitor(const std::unique_ptr<Expression>& differentiationVariable)
{
    this->differentiationVariable = differentiationVariable->Copy();
}
DifferentiateVisitor::DifferentiateVisitor(const std::unique_ptr<Expression>& differentiationVariable, const DifferentiationOpts& opts)
{
    this->differentiationVariable = differentiationVariable->Copy();
    this->opts = opts;
}

auto DifferentiateVisitor::TypedVisit(const Real& real) -> RetT
{
    return gsl::not_null { std::make_unique<Real>(0.0) };
}

auto DifferentiateVisitor::TypedVisit(const Imaginary&) -> RetT
{
    return gsl::not_null { std::make_unique<Real>(0.0) };
}

auto DifferentiateVisitor::TypedVisit(const Variable& var) -> RetT
{
    if (auto variable = RecursiveCast<Variable>(*(this->differentiationVariable)); variable != nullptr) {
        if (variable->GetName() == var.GetName()) {
            return gsl::not_null { std::make_unique<Real>(1.0) };
        }
        else if (this->opts.multivariate == DifferentiationOpts::Multivariate::MULTIVARIABLE ){
            return gsl::not_null { std::make_unique<Derivative<Expression, Expression>>(var, *this->differentiationVariable) };
        } else {
            return gsl::not_null { std::make_unique<Real>(0.0) };
        }
    }
    return gsl::not_null { std::make_unique<Derivative<Expression, Expression>>(var, *this->differentiationVariable) };
}

auto DifferentiateVisitor::TypedVisit(const Undefined&) -> RetT
{
    return gsl::not_null { std::make_unique<Undefined>() };
}

auto DifferentiateVisitor::TypedVisit(const Add<Expression, Expression>& add) -> RetT
{
    if (auto variable = RecursiveCast<Variable>(*(this->differentiationVariable)); variable != nullptr) {
        auto diffedleft = add.mostSigOp->Accept(*this);
        auto diffedright = add.leastSigOp->Accept(*this);

        if (!diffedleft) {
            return std::unexpected { diffedleft.error() };
        }
        if (!diffedleft) {
            return std::unexpected { diffedleft.error() };
        }

        auto left = std::move(diffedleft).value();
        auto right = std::move(diffedright).value();

        SimplifyVisitor simplifyVisitor{};
        auto simplified = Add<Expression> { *left, *right }.Accept(simplifyVisitor);
        if (!simplified) {
            return std::unexpected{simplified.error()};
        }
        return std::move(simplified).value();
    }
    return gsl::not_null<std::unique_ptr<Expression>>(Oasis::Derivative<Expression>{*(add.Copy()), *(this->differentiationVariable)}.Generalize());
}

auto DifferentiateVisitor::TypedVisit(const Subtract<Expression, Expression>& subtract) -> RetT
{
    if (auto variable = RecursiveCast<Variable>(*(this->differentiationVariable)); variable != nullptr) {
        auto diffedleft = subtract.mostSigOp->Accept(*this);
        auto diffedright = subtract.leastSigOp->Accept(*this);

        if (!diffedleft) {
            return std::unexpected { diffedleft.error() };
        }
        if (!diffedleft) {
            return std::unexpected { diffedleft.error() };
        }

        auto left = std::move(diffedleft).value();
        auto right = std::move(diffedright).value();

        SimplifyVisitor simplifyVisitor{};
        auto simplified = Subtract<Expression> { *left, *right }.Accept(simplifyVisitor);
        if (!simplified) {
            return std::unexpected{simplified.error()};
        }
        return std::move(simplified).value();
    }
    return gsl::not_null<std::unique_ptr<Expression>>(Oasis::Derivative<Expression>{*(subtract.Copy()), *(this->differentiationVariable)}.Generalize());
}

auto DifferentiateVisitor::TypedVisit(const Multiply<Expression, Expression>& multiply) -> RetT
{
    if (auto variable = RecursiveCast<Variable>(*(this->differentiationVariable)); variable != nullptr) {
        auto diffedleft = multiply.mostSigOp->Accept(*this);
        auto diffedright = multiply.leastSigOp->Accept(*this);
        if (!diffedleft) {
            return std::unexpected { diffedleft.error() };
        }
        if (!diffedright) {
            return std::unexpected { diffedright.error() };
        }

        auto left = std::move(diffedleft).value();
        auto right = std::move(diffedright).value();

        SimplifyVisitor simplifyVisitor{};
        auto multiplied = Add<Expression> {
            Multiply{*left, *multiply.leastSigOp->Copy()},
            Multiply{*multiply.mostSigOp->Copy(), *right}
        }.Accept(simplifyVisitor);

        if (!multiplied) {
            return std::unexpected { multiplied.error() };
        }
        return std::move(multiplied).value();
    }
    return gsl::not_null<std::unique_ptr<Expression>>(Oasis::Derivative<Expression>{*(multiply.Copy()), *(this->differentiationVariable)}.Generalize());
}

auto DifferentiateVisitor::TypedVisit(const Divide<Expression, Expression>& divide) -> RetT
{
    // TODO: IMPLEMENT
    return gsl::not_null<std::unique_ptr<Expression>>(Oasis::Derivative<Expression>{*(divide.Copy()), *(this->differentiationVariable)}.Generalize());
}

auto DifferentiateVisitor::TypedVisit(const Exponent<Expression, Expression>& exponent) -> RetT
{
    if (auto variable = RecursiveCast<Variable>(*(this->differentiationVariable)); variable != nullptr) {
        SimplifyVisitor simplifyVisitor{};
        auto diffedleft = exponent.mostSigOp->Accept(*this);
        if (!diffedleft) {
            return std::unexpected { diffedleft.error() };
        }

        auto left = std::move(diffedleft).value();

        auto multiplied = Multiply<Expression> {
            Multiply{
                Exponent{
                    *exponent.mostSigOp->Copy(),
                    Subtract{*exponent.leastSigOp->Copy(), Real{1.0}}
                },
                *exponent.leastSigOp->Copy()
            },
            *left
        }.Accept(simplifyVisitor);

        if (!multiplied) {
            return std::unexpected { multiplied.error() };
        }
        return std::move(multiplied).value();
    }
    return gsl::not_null<std::unique_ptr<Expression>>(Oasis::Derivative<Expression>{*(exponent.Copy()), *(this->differentiationVariable)}.Generalize());
}

auto DifferentiateVisitor::TypedVisit(const Log<Expression, Expression>& log) -> RetT
{
    // TODO: IMPLEMENT
    return gsl::not_null<std::unique_ptr<Expression>>(Oasis::Derivative<Expression>{*(log.Copy()), *(this->differentiationVariable)}.Generalize());
}

auto DifferentiateVisitor::TypedVisit(const Negate<Expression>& negate) -> RetT
{
    // TODO: IMPLEMENT
    return gsl::not_null<std::unique_ptr<Expression>>(Oasis::Derivative<Expression>{*(negate.Copy()), *(this->differentiationVariable)}.Generalize());
}

auto DifferentiateVisitor::TypedVisit(const Sine<Expression>& sine) -> RetT
{
    // TODO: IMPLEMENT
    return gsl::not_null<std::unique_ptr<Expression>>(Oasis::Derivative<Expression>{*(sine.Copy()), *(this->differentiationVariable)}.Generalize());
}

auto DifferentiateVisitor::TypedVisit(const Derivative<Expression, Expression>& derivative) -> RetT
{
    // TODO: IMPLEMENT
    return gsl::not_null<std::unique_ptr<Expression>>(Oasis::Derivative<Expression>{*(derivative.Copy()), *(this->differentiationVariable)}.Generalize());
}

auto DifferentiateVisitor::TypedVisit(const Integral<Expression, Expression>& integral) -> RetT
{
    // TODO: IMPLEMENT
    return gsl::not_null<std::unique_ptr<Expression>>(Oasis::Derivative<Expression>{*(integral.Copy()), *(this->differentiationVariable)}.Generalize());
}

auto DifferentiateVisitor::TypedVisit(const Matrix& matrix) -> RetT
{
    // TODO: IMPLEMENT
    return gsl::not_null<std::unique_ptr<Expression>>(Oasis::Derivative<Expression>{*(matrix.Copy()), *(this->differentiationVariable)}.Generalize());
}

auto DifferentiateVisitor::TypedVisit(const EulerNumber&) -> RetT
{
    return gsl::not_null<std::unique_ptr<Expression>>(Real{0.0}.Generalize());
}

auto DifferentiateVisitor::TypedVisit(const Pi&) -> RetT
{
    return gsl::not_null<std::unique_ptr<Expression>>(Real{0.0}.Generalize());
}

auto DifferentiateVisitor::TypedVisit(const Magnitude<Expression>& magnitude) -> RetT
{
    // TODO: IMPLEMENT
    return gsl::not_null<std::unique_ptr<Expression>>(Oasis::Derivative<Expression>{*(magnitude.Copy()), *(this->differentiationVariable)}.Generalize());
}

} // Oasis
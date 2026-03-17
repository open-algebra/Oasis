//
// Created by Andrew Nazareth on 9/23/25.
//

#include <format>

#include "Oasis/Add.hpp"
#include "Oasis/BinaryExpression.hpp"
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

// I do not know why I defined this.
// namespace {
// constexpr auto EPSILON = std::numeric_limits<float>::epsilon();
// }

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

auto DifferentiateVisitor::TypedVisit(const Real&) -> RetT
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
    SimplifyVisitor simplifyVisitor {};
    // Single differentiation variable
    if (auto variable = RecursiveCast<Variable>(*(this->differentiationVariable)); variable != nullptr) {
        auto simplifiedDiv = divide.Accept(simplifyVisitor).value();

        // Constant case - differentiation over a divisor
        if (auto constant = RecursiveCast<Divide<Expression, Real>>(*simplifiedDiv); constant != nullptr) {
            auto exp = constant->GetMostSigOp().Copy();
            auto num = constant->GetLeastSigOp();
            auto differentiate = (*exp).Differentiate(*(this->differentiationVariable));
            if (auto add = RecursiveCast<Expression>(*differentiate); add != nullptr) {
                auto simplifiedAdd = add->Accept(simplifyVisitor).value();
                return std::make_unique<Divide<Expression, Real>>(Divide<Expression, Real> { *simplifiedAdd, Real { num.GetValue() } })->Accept(simplifyVisitor).value();
            }
        }
        // In case of simplify turning divide into mult
        if (auto constant = RecursiveCast<Multiply<Expression, Real>>(*simplifiedDiv); constant != nullptr) {
            auto exp = constant->GetMostSigOp().Copy();
            auto num = constant->GetLeastSigOp();
            auto differentiate = (*exp).Differentiate(*(this->differentiationVariable));
            if (auto add = RecursiveCast<Expression>(*differentiate); add != nullptr) {
                auto simplifiedAdd = (add->Accept(simplifyVisitor).value());
                return std::make_unique<Multiply<Expression, Real>>(Multiply<Expression, Real> { *simplifiedAdd, Real { num.GetValue() } })->Accept(simplifyVisitor).value();
            }
        }
        // Quotient Rule: d/dx (f(x)/g(x)) = (g(x)f'(x)-f(x)g'(x))/(g(x)^2)
        if (auto quotient = RecursiveCast<Divide<Expression, Expression>>(*simplifiedDiv); quotient != nullptr) {
            auto leftexp = quotient->GetMostSigOp().Copy();
            auto rightexp = quotient->GetLeastSigOp().Copy();
            auto leftDiff = leftexp->Differentiate(*(this->differentiationVariable));
            auto rightDiff = rightexp->Differentiate(*(this->differentiationVariable));
            auto mult1 = Multiply<Expression, Expression>(Multiply<Expression, Expression> { *(rightexp->Accept(simplifyVisitor).value()), *(leftDiff->Accept(simplifyVisitor).value()) }).Accept(simplifyVisitor).value()->Accept(simplifyVisitor).value();
            auto mult2 = Multiply<Expression, Expression>(Multiply<Expression, Expression> { *(leftexp->Accept(simplifyVisitor).value()), *(rightDiff->Accept(simplifyVisitor).value()) }).Accept(simplifyVisitor).value()->Accept(simplifyVisitor).value();
            auto numerator = Subtract<Expression, Expression>(Subtract<Expression, Expression> { *mult1, *mult2 }).Accept(simplifyVisitor).value();
            auto denominator = Multiply<Expression, Expression>(Multiply<Expression, Expression> { *(rightexp->Accept(simplifyVisitor).value()), *(rightexp->Accept(simplifyVisitor).value()) }).Accept(simplifyVisitor).value();
            return Divide<Expression, Expression>({ *(numerator->Accept(simplifyVisitor).value()), *(denominator->Accept(simplifyVisitor).value()) }).Accept(simplifyVisitor).value();
        }
    }

    return gsl::not_null<std::unique_ptr<Expression>>(Oasis::Derivative<Expression>{*(divide.Copy()), *(this->differentiationVariable)}.Generalize());
}

auto DifferentiateVisitor::TypedVisit(const Exponent<Expression, Expression>& exponent) -> RetT
{
    // TODO: FIX
    // d/dx (a^{u(x)}) = a^{u(x)} * ln(a) * u'(x)
    // d/dx (x^n) = n*x^(n-1)
    // Need to check exponential vs polynomial
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
    if (auto variable = RecursiveCast<Variable>(*(this->differentiationVariable)); variable != nullptr){
        SimplifyVisitor simplifyVisitor {};
        // d(log_e(6x))/dx = 1/6x * 6
        if (auto lnCase = RecursiveCast<EulerNumber>(*log.mostSigOp); lnCase != nullptr) {
            Divide derivative { Oasis::Real { 1.0 }, *log.leastSigOp };
            Derivative chain { *log.leastSigOp, *(this->differentiationVariable) };

            Multiply result = Multiply<Expression> { derivative, *chain.Differentiate(*(this->differentiationVariable)) };
            auto simp = result.Accept(simplifyVisitor);
            if (!simp) {
                return gsl::not_null<std::unique_ptr<Expression>>{result.Generalize()};
            }
            return std::move(simp).value();
        } else {
            if (auto RealCase = RecursiveCast<Real>(*log.mostSigOp); RealCase != nullptr) {
                Divide derivative { Oasis::Real { 1.0 }, Multiply<Expression> { *log.leastSigOp, Log { EulerNumber {}, *log.mostSigOp } } };
                Derivative chain { *log.leastSigOp, *(this->differentiationVariable) };
                Multiply result = Multiply<Expression> { derivative, *chain.Differentiate(*(this->differentiationVariable)) };
                auto simp = result.Accept(simplifyVisitor);
                if (!simp) {
                    return gsl::not_null<std::unique_ptr<Expression>>{result.Generalize()};
                }
                return std::move(simp).value();
            } else {
                // Use log identity and Quotient rule
                Divide result { Subtract { Multiply { Log { EulerNumber {}, *log.mostSigOp }, Derivative { Log { EulerNumber {}, *log.leastSigOp }, *(this->differentiationVariable) } },
                                    Multiply { Log { EulerNumber {}, *log.leastSigOp }, Derivative { Log { EulerNumber {}, *log.mostSigOp }, *(this->differentiationVariable) } } },
                    Exponent { Log { EulerNumber {}, *log.mostSigOp }, Real { 2 } } };
                auto simp = result.Accept(simplifyVisitor);
                if (!simp) {
                    return gsl::not_null<std::unique_ptr<Expression>>{result.Generalize()};
                }
                return std::move(simp).value();
            }
        }
    }
    return gsl::not_null<std::unique_ptr<Expression>>(Oasis::Derivative<Expression>{*(log.Copy()), *(this->differentiationVariable)}.Generalize());
}

auto DifferentiateVisitor::TypedVisit(const Negate<Expression>& negate) -> RetT
{
    if (auto variable = RecursiveCast<Variable>(*(this->differentiationVariable)); variable != nullptr) {
        const std::unique_ptr<Expression> operandDerivative = negate.GetOperand().Differentiate(*(this->differentiationVariable));
        return gsl::not_null{Negate<Expression> {*operandDerivative}.Generalize()};
    }

    return gsl::not_null<std::unique_ptr<Expression>>(Oasis::Derivative<Expression>{*(negate.Copy()), *(this->differentiationVariable)}.Generalize());
}

auto DifferentiateVisitor::TypedVisit(const Sine<Expression>& sine) -> RetT
{
    // TODO: IMPLEMENT
    return std::unexpected<std::string>{"Not Implemented."};
    return gsl::not_null<std::unique_ptr<Expression>>(Oasis::Derivative<Expression>{*(sine.Copy()), *(this->differentiationVariable)}.Generalize());
}

auto DifferentiateVisitor::TypedVisit(const Derivative<Expression, Expression>& derivative) -> RetT
{
    if (auto variable = RecursiveCast<Variable>(*(this->differentiationVariable)); variable != nullptr) {
        DifferentiateVisitor dv_other{derivative.GetLeastSigOp().Generalize(), this->opts};
        auto diff = derivative.GetMostSigOp().Accept(dv_other);
        if (!diff) {
            return gsl::not_null<std::unique_ptr<Expression>>(Derivative{derivative, *this->differentiationVariable}.Generalize());
        }
        auto res = std::move(diff).value();
        auto result = res->Accept(*this);
        if (!result) {
            return gsl::not_null<std::unique_ptr<Expression>>{Derivative{*res, *this->differentiationVariable}.Generalize()};
        }
        return gsl::not_null<std::unique_ptr<Expression>>{std::move(result).value()};
    }
    return gsl::not_null<std::unique_ptr<Expression>>(Oasis::Derivative<Expression>{*(derivative.Copy()), *(this->differentiationVariable)}.Generalize());
}

auto DifferentiateVisitor::TypedVisit(const Integral<Expression, Expression>& integral) -> RetT
{
    if (auto variable = RecursiveCast<Variable>(*(this->differentiationVariable)); variable != nullptr) {
        auto integral_simp = integral.GetMostSigOp().Integrate(integral.GetLeastSigOp());

        auto result = integral_simp->Accept(*this);
        if (!result) {
            return gsl::not_null<std::unique_ptr<Expression>>{Derivative{*integral_simp, *this->differentiationVariable}.Generalize()};
        }
        return gsl::not_null<std::unique_ptr<Expression>>{std::move(result).value()};
    }
    return gsl::not_null<std::unique_ptr<Expression>>(Oasis::Derivative<Expression>{*(integral.Copy()), *(this->differentiationVariable)}.Generalize());
}

auto DifferentiateVisitor::TypedVisit(const Matrix& matrix) -> RetT
{
    // TODO: IMPLEMENT
    return std::unexpected<std::string>{"Not Implemented."};
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
    return std::unexpected<std::string>{"Not Implemented."};
    return gsl::not_null<std::unique_ptr<Expression>>(Oasis::Derivative<Expression>{*(magnitude.Copy()), *(this->differentiationVariable)}.Generalize());
}

} // Oasis
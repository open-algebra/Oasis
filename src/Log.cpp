//
// Created by Matthew McCall on 10/6/23.
// Modified by Blake Kessler on 10/10/23
//

#include <cmath>

#include "Oasis/Add.hpp"
#include "Oasis/Derivative.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/EulerNumber.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Expression.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Pi.hpp"
#include "Oasis/RecursiveCast.hpp"
#include "Oasis/SimplifyVisitor.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Undefined.hpp"

namespace Oasis {
Log<Expression>::Log(const Expression& base, const Expression& argument)
    : BinaryExpression(base, argument)
{
}

auto Log<Expression>::Simplify() const -> std::unique_ptr<Expression>
{
    const auto simplifiedBase = mostSigOp ? mostSigOp->Simplify() : nullptr;
    const auto simplifiedArgument = leastSigOp ? leastSigOp->Simplify() : nullptr;

    if (!simplifiedBase || !simplifiedArgument) {
        return nullptr;
    }

    const Log simplifiedLog { *simplifiedBase, *simplifiedArgument };

    if (const auto realBaseCase = RecursiveCast<Log<Real, Expression>>(simplifiedLog); realBaseCase != nullptr) {
        if (const Real& b = realBaseCase->GetMostSigOp(); b.GetValue() <= 0.0 || b.GetValue() == 1) {
            return std::make_unique<Undefined>();
        }
    }

    if (const auto realExponentCase = RecursiveCast<Log<Expression, Real>>(simplifiedLog); realExponentCase != nullptr) {
        const Real& argument = realExponentCase->GetLeastSigOp();

        if (argument.GetValue() <= 0.0) {
            return std::make_unique<Undefined>();
        }

        if (argument.GetValue() == 1.0) {
            return std::make_unique<Real>(0.0);
        }
    }

    if (const auto realCase = RecursiveCast<Log<Real>>(simplifiedLog); realCase != nullptr) {
        const Real& base = realCase->GetMostSigOp();
        const Real& argument = realCase->GetLeastSigOp();

        return std::make_unique<Real>(log2(argument.GetValue()) * (1 / log2(base.GetValue())));
    }

    // log(a) with a < 0 log(-a)
    if (auto negCase = RecursiveCast<Log<Expression, Real>>(simplifiedLog); negCase != nullptr) {
        if (negCase->GetLeastSigOp().GetValue() < 0) {
            return Add<Expression> { Log { negCase->GetMostSigOp(), Real { -1 * negCase->GetLeastSigOp().GetValue() } }, Multiply<Expression> { Imaginary {}, Pi {} } }.Generalize();
        }
    }

    // log[a](a) = 1
    if (const auto sameCase = RecursiveCast<Log<Expression, Expression>>(simplifiedLog); sameCase != nullptr) {
        if (sameCase->leastSigOp->Equals(*sameCase->mostSigOp)) {
            return Real { 1 }.Generalize();
        }
    }

    // log[a](b^x) = x * log[a](b)
    if (const auto expCase = RecursiveCast<Log<Expression, Exponent<>>>(simplifiedLog); expCase != nullptr) {
        SimplifyVisitor simplifyVisitor {};
        const auto exponent = expCase->GetLeastSigOp();
        const IExpression auto& log = Log<Expression>(expCase->GetMostSigOp(), exponent.GetMostSigOp()); // might need to check that it isnt nullptr
        const IExpression auto& factor = exponent.GetLeastSigOp();
        auto e = Oasis::Multiply<Oasis::Expression>(factor, log);
        auto s = e.Accept(simplifyVisitor);
        if (!s) {
            return e.Generalize();
        } else {
            return std::move(s).value();
        }
    }

    return simplifiedLog.Copy();
}

auto Log<Expression>::Integrate(const Oasis::Expression& integrationVariable) const -> std::unique_ptr<Expression>
{
    // TODO: Implement with integrate visitor?
    SimplifyVisitor simplifyVisitor {};
    if (this->mostSigOp->Equals(EulerNumber {})) {
        // ln(x)
        if (leastSigOp->Is<Variable>() && RecursiveCast<Variable>(*leastSigOp)->Equals(integrationVariable)) {
            return Add { Multiply { integrationVariable, Subtract { *this, Real { 1 } } }, Variable { "C" } }.Simplify();
            // alternate form
            // return Subtract<Expression> { Multiply<Expression> { integrationVariable, *this }, integrationVariable }.Simplify();
        }
        auto der_Simp = Derivative<Expression> { *leastSigOp, integrationVariable }.Accept(simplifyVisitor);
        if (!der_Simp) {
            return this->Generalize();
        }

        auto derivative = std::move(der_Simp).value();

        if (derivative->Equals(Real { 0 }))
            return Add { Multiply { integrationVariable, *this }, Variable { "C" } }.Simplify();
        // ln(ax + b)
        if (derivative->Is<Real>()) {
            return Add { Divide<Expression> { Multiply { *leastSigOp, Subtract { *this, Real { 1 } } }, *derivative }, Variable { "C" } }.Simplify();
        }

        // Commented out since Issue #178 is leading to incorrect integrals
        //
        // auto divideCase = RecursiveCast<Divide<Expression>>(*leastSigOp);
        // auto multiplyCase = RecursiveCast<Multiply<Expression, Expression>>(*leastSigOp);
        // if (multiplyCase != nullptr || divideCase != nullptr) {
        //     // Use Log identity log(a*b) = log(a) + log(b)
        //     std::unique_ptr<Expression> leftInt;
        //     std::unique_ptr<Expression> rightInt;
        //     if (multiplyCase != nullptr) {
        //         leftInt = Integral { Log { EulerNumber {}, multiplyCase->GetMostSigOp() }, integrationVariable }.Simplify();
        //         rightInt = Integral { Log { EulerNumber {}, multiplyCase->GetLeastSigOp() }, integrationVariable }.Simplify();
        //     } else { // same as multiply case except the second integral will be negative
        //         leftInt = Integral { Log { EulerNumber {}, multiplyCase->GetMostSigOp() }, integrationVariable }.Simplify();
        //         rightInt = Multiply { Real { -1 }, Integral { Log { EulerNumber {}, multiplyCase->GetLeastSigOp() }, integrationVariable } }.Simplify();
        //     }
        //
        //     auto specializedleft = RecursiveCast<Add<Expression, Variable>>(*leftInt);
        //     auto specializedright = RecursiveCast<Add<Expression, Variable>>(*rightInt);
        //
        //     if (specializedleft == nullptr || specializedright == nullptr)
        //         return Add { *leftInt, *rightInt }.Simplify();
        //
        //     Add<Expression> add {
        //         Add<Expression, Expression> {
        //             *(specializedleft->GetMostSigOp().Copy()), *(specializedright->GetMostSigOp().Copy()) },
        //         Variable { "C" }
        //     };
        //     return add.Simplify();
        // }

    } else {
        // Use log identity Log[b](a) = Log(a)/Log(b)
        SimplifyVisitor simplifyVisitor {};
        auto numer = Log<Expression> { EulerNumber {}, *(this->leastSigOp->Generalize()) };
        auto denom = Log<Expression> { EulerNumber {}, *(this->mostSigOp->Generalize()) };

        // TODO: FIX
        if (numer.Equals(denom))
            return Add { integrationVariable, Variable { "C" } }.Generalize();

        // Can only use if logaritm base is a constant
        if (Derivative { denom, integrationVariable }.Simplify()->Equals(Real { 0 })) {
            auto naturalint = Integral { numer, integrationVariable }.Simplify();
            if (auto specialint = RecursiveCast<Add<Expression>>(*naturalint); specialint != nullptr) {
                return Add { Divide { specialint->GetMostSigOp(), denom }, Variable { "C" } }.Simplify();
            }
            return Divide { *naturalint, denom }.Simplify();
        }
    }

    return Integral<Expression> { *this, integrationVariable }.Generalize();
}

auto Log<Expression>::Differentiate(const Oasis::Expression& differentiationVariable) const -> std::unique_ptr<Expression>
{
    SimplifyVisitor simplifyVisitor {};
    // d(log_e(6x))/dx = 1/6x * 6
    if (auto lnCase = RecursiveCast<EulerNumber>(*mostSigOp); lnCase != nullptr) {
        Divide derivative { Oasis::Real { 1.0 }, *leastSigOp };
        Derivative chain { *leastSigOp, differentiationVariable };

        Multiply result = Multiply<Expression> { derivative, *chain.Differentiate(differentiationVariable) };
        auto simp = result.Accept(simplifyVisitor);
        if (!simp) {
            return result.Generalize();
        }
        return std::move(simp).value();
    } else {
        if (auto RealCase = RecursiveCast<Real>(*mostSigOp); RealCase != nullptr) {
            Divide derivative { Oasis::Real { 1.0 }, Multiply<Expression> { *leastSigOp, Log { EulerNumber {}, *mostSigOp } } };
            Derivative chain { *leastSigOp, differentiationVariable };
            Multiply result = Multiply<Expression> { derivative, *chain.Differentiate(differentiationVariable) };
            auto simp = result.Accept(simplifyVisitor);
            if (!simp) {
                return result.Generalize();
            }
            return std::move(simp).value();
        } else { // Use log identity and Quotient rule
            Divide result { Subtract { Multiply { Log { EulerNumber {}, *mostSigOp }, Derivative { Log { EulerNumber {}, *leastSigOp }, differentiationVariable } },
                                Multiply { Log { EulerNumber {}, *leastSigOp }, Derivative { Log { EulerNumber {}, *mostSigOp }, differentiationVariable } } },
                Exponent { Log { EulerNumber {}, *mostSigOp }, Real { 2 } } };
            auto simp = result.Accept(simplifyVisitor);
            if (!simp) {
                return result.Generalize();
            }
            return std::move(simp).value();
        }
    }
}

} // Oasis

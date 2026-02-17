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

auto Log<Expression>::Integrate(const Oasis::Expression& integrationVariable) const -> std::unique_ptr<Expression>
{
    // TODO: Implement with integrate visitor?
    SimplifyVisitor simplifyVisitor {};
    if (this->mostSigOp->Equals(EulerNumber {})) {
        // ln(x)
        if (leastSigOp->Is<Variable>() && RecursiveCast<Variable>(*leastSigOp)->Equals(integrationVariable)) {
            Add add { Multiply { integrationVariable, Subtract { *this, Real { 1 } } }, Variable { "C" } };
            return add.Accept(simplifyVisitor).value();
            // alternate form
            // return Subtract<Expression> { Multiply<Expression> { integrationVariable, *this }, integrationVariable }.Simplify();
        }
        auto der_Simp = Derivative<Expression> { *leastSigOp, integrationVariable }.Accept(simplifyVisitor);
        if (!der_Simp) {
            return this->Generalize();
        }

        auto derivative = std::move(der_Simp).value();

        if (derivative->Equals(Real { 0 })) {
            Add add { Multiply { integrationVariable, *this }, Variable { "C" } };
            return add.Accept(simplifyVisitor).value();
        }
        // ln(ax + b)
        if (derivative->Is<Real>()) {
            Add add { Divide<> { Multiply { *leastSigOp, Subtract { *this, Real { 1 } } }, *derivative }, Variable { "C" } };
            return add.Accept(simplifyVisitor).value();
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
        auto numer = Log<Expression> { EulerNumber {}, *(this->leastSigOp->Generalize()) };
        auto denom = Log<Expression> { EulerNumber {}, *(this->mostSigOp->Generalize()) };

        // TODO: FIX
        if (numer.Equals(denom))
            return Add { integrationVariable, Variable { "C" } }.Generalize();

        Derivative<Log<>, Expression> derivative { denom, integrationVariable };
        // Can only use if logaritm base is a constant
        if (derivative.Accept(simplifyVisitor).value()->Equals(Real { 0 })) {
            Integral<Log<>, Expression> integral { numer, integrationVariable };
            auto naturalint = integral.Accept(simplifyVisitor).value();
            if (auto specialint = RecursiveCast<Add<Expression>>(*naturalint); specialint != nullptr) {
                Add<Divide<Expression, Log<>>, Variable> add { Divide { specialint->GetMostSigOp(), denom }, Variable { "C" } };
                return add.Accept(simplifyVisitor).value();
            }
            Divide<Expression, Log<>> divide { *naturalint, denom };
            return divide.Accept(simplifyVisitor).value();
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
        } else {
            // Use log identity and Quotient rule
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

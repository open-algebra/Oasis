//
// Created by Matthew McCall on 10/6/23.
// Modified by Blake Kessler on 10/10/23
//

#include "Oasis/Log.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Derivative.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/EulerNumber.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Expression.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Pi.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Undefined.hpp"
#include <cmath>

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

    if (const auto realBaseCase = Log<Real, Expression>::Specialize(simplifiedLog); realBaseCase != nullptr) {
        if (const Real& b = realBaseCase->GetMostSigOp(); b.GetValue() <= 0.0 || b.GetValue() == 1) {
            return std::make_unique<Undefined>();
        }
    }

    if (const auto realExponentCase = Log<Expression, Real>::Specialize(simplifiedLog); realExponentCase != nullptr) {
        const Real& argument = realExponentCase->GetLeastSigOp();

        if (argument.GetValue() <= 0.0) {
            return std::make_unique<Undefined>();
        }

        if (argument.GetValue() == 1.0) {
            return std::make_unique<Real>(0.0);
        }
    }

    if (const auto realCase = Log<Real>::Specialize(simplifiedLog); realCase != nullptr) {
        const Real& base = realCase->GetMostSigOp();
        const Real& argument = realCase->GetLeastSigOp();

        return std::make_unique<Real>(log2(argument.GetValue()) * (1 / log2(base.GetValue())));
    }

    // log(a) with a < 0 log(-a)
    if (auto negCase = Log<Expression, Real>::Specialize(simplifiedLog); negCase != nullptr) {
        if (negCase->GetLeastSigOp().GetValue() < 0) {
            return Add<Expression> { Log { negCase->GetMostSigOp(), Real { -1 * negCase->GetLeastSigOp().GetValue() } }, Multiply<Expression> { Imaginary {}, Pi {} } }.Generalize();
        }
    }

    // log[a](b^x) = x * log[a](b)
    if (const auto expCase = Log<Expression, Exponent<>>::Specialize(simplifiedLog); expCase != nullptr) {
        const auto exponent = expCase->GetLeastSigOp();
        const IExpression auto& log = Log<Expression>(expCase->GetMostSigOp(), exponent.GetMostSigOp()); // might need to check that it isnt nullptr
        const IExpression auto& factor = exponent.GetLeastSigOp();
        return Oasis::Multiply<Oasis::Expression>(factor, log).Simplify();
    }

    return simplifiedLog.Copy();
}

auto Log<Expression>::Specialize(const Expression& other) -> std::unique_ptr<Log>
{
    if (!other.Is<Oasis::Log>()) {
        return nullptr;
    }

    const auto otherGeneralized = other.Generalize();
    return std::make_unique<Log>(dynamic_cast<const Log<Expression>&>(*otherGeneralized));
}

auto Log<Expression>::Integrate(const Oasis::Expression& integrationVariable) const -> std::unique_ptr<Expression>
{
    // TODO: Implement
    if (this->mostSigOp->Equals(EulerNumber {})) {
        // ln(x)
        if (leastSigOp->Is<Variable>() && Variable::Specialize(*leastSigOp)->Equals(integrationVariable)) {
            return Subtract<Expression> { Multiply<Expression> { integrationVariable, *this }, integrationVariable }.Simplify();
        }
        if (auto multiplyCase = Multiply<Expression>::Specialize(*leastSigOp); multiplyCase != nullptr) {
            if (multiplyCase->GetLeastSigOp().Equals(integrationVariable)) {
                return Subtract<Expression> { Multiply<Expression> { integrationVariable, *this }, integrationVariable }.Simplify();
            } else {
                return Multiply<Expression> { integrationVariable, *this }.Simplify();
            }
        }
    } else {
        auto numer = Log<Expression> { EulerNumber {}, *(this->leastSigOp->Generalize()) };
        auto denom = Log<Expression> { EulerNumber {}, *(this->mostSigOp->Generalize()) };
        if (numer.Equals(denom))
            return integrationVariable.Generalize();
        return Divide { Integral { numer, integrationVariable }, denom }.Simplify();
    }

    return Integral<Expression> { *this, integrationVariable }.Generalize();
}

auto Log<Expression>::Differentiate(const Oasis::Expression& differentiationVariable) const -> std::unique_ptr<Expression>
{
    // d(log_e(6x))/dx = 1/6x * 6
    if (auto lnCase = EulerNumber::Specialize(*mostSigOp); lnCase != nullptr) {
        Divide derivative { Oasis::Real { 1.0 }, *leastSigOp };
        Derivative chain { *leastSigOp, differentiationVariable };

        Multiply result = Multiply<Expression> { derivative, *chain.Differentiate(differentiationVariable) };
        return result.Simplify();
    } else {
        Divide derivative { Oasis::Real { 1.0 }, Multiply<Expression> { *leastSigOp, Log { EulerNumber {}, *mostSigOp } } };
        Derivative chain { *leastSigOp, differentiationVariable };

        Multiply result = Multiply<Expression> { derivative, *chain.Differentiate(differentiationVariable) };
        return result.Simplify();
    }
}

} // Oasis

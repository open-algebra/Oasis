//
// Created by Andrew Nazareth on 9/19/23.
//

#include <cmath>

#include "Oasis/Add.hpp"
#include "Oasis/Derivative.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/EulerNumber.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/MatchCast.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Negate.hpp"
#include "Oasis/RecursiveCast.hpp"

namespace Oasis {

Exponent<Expression>::Exponent(const Expression& base, const Expression& power)
    : BinaryExpression(base, power)
{
}

auto Exponent<Expression>::Simplify() const -> std::unique_ptr<Expression>
{
    static constexpr auto match_cast = MatchCast<Expression>()
                                           .Case(
                                               [](const Exponent<Expression, Real>& zeroCase) -> bool {
                                                   // TODO: Optimize. We're calling RecursiveCast<Expression<Real, Expression> twice. Perhaps a map of expression types to a vector of lambdas?
                                                   const Real& power = zeroCase.GetLeastSigOp();
                                                   return power.GetValue() == 0.0;
                                               },
                                               [](const Exponent<Expression, Real>&, const void*) -> std::expected<gsl::not_null<std::unique_ptr<Expression>>, std::string_view> {
                                                   return gsl::make_not_null(std::make_unique<Real>(1.0));
                                               })
                                           .Case(
                                               [](const Exponent<Real, Expression>& zeroCase) -> bool {
                                                   const Real& base = zeroCase.GetMostSigOp();
                                                   return base.GetValue() == 0.0;
                                               },
                                               [](const Exponent<Real, Expression>&, const void*) -> std::expected<gsl::not_null<std::unique_ptr<Expression>>, std::string_view> {
                                                   return gsl::make_not_null(std::make_unique<Real>(0.0));
                                               })
                                           .Case(
                                               [](const Exponent<Real>&) { return true; },
                                               [](const Exponent<Real>& realCase, const void*) -> std::expected<gsl::not_null<std::unique_ptr<Expression>>, std::string_view> {
                                                   const Real& base = realCase.GetMostSigOp();
                                                   const Real& power = realCase.GetLeastSigOp();
                                                   return gsl::make_not_null(std::make_unique<Real>(pow(base.GetValue(), power.GetValue())));
                                               })
                                           .Case(
                                               [](const Exponent<Expression, Real>& oneCase) -> bool {
                                                   const Real& power = oneCase.GetLeastSigOp();
                                                   return power.GetValue() == 1.0;
                                               },
                                               [](const Exponent<Expression, Real>& oneCase, const void*) -> std::expected<gsl::not_null<std::unique_ptr<Expression>>, std::string_view> {
                                                   return gsl::make_not_null(oneCase.GetMostSigOp().Copy());
                                               })
                                           .Case(
                                               [](const Exponent<Real, Expression>& oneCase) -> bool {
                                                   const Real& base = oneCase.GetMostSigOp();
                                                   return base.GetValue() == 1.0;
                                               },
                                               [](const Exponent<Real, Expression>&, const void*) -> std::expected<gsl::not_null<std::unique_ptr<Expression>>, std::string_view> {
                                                   return gsl::make_not_null(std::make_unique<Real>(1.0));
                                               })
                                           .Case(
                                               [](const Exponent<Imaginary, Real>&) -> bool { return true; },
                                               [](const Exponent<Imaginary, Real>& ImgCase, const void*) -> std::expected<gsl::not_null<std::unique_ptr<Expression>>, std::string_view> {
                                                   switch (const auto power = std::fmod((ImgCase.GetLeastSigOp()).GetValue(), 4); static_cast<int>(power)) {
                                                   case 0:
                                                       return gsl::make_not_null(std::make_unique<Real>(1));
                                                   case 1:
                                                       return gsl::make_not_null(std::make_unique<Imaginary>());
                                                   case 2:
                                                       return gsl::make_not_null(std::make_unique<Real>(-1));
                                                   case 3:
                                                       return gsl::make_not_null(std::make_unique<Negate<Imaginary>>(Imaginary {}));
                                                   default:
                                                       return std::unexpected { "std::fmod returned an invalid value" };
                                                   }
                                               })
                                           .Case(
                                               [](const Exponent<Multiply<Real, Expression>, Real>& ImgCase) -> bool {
                                                   return ImgCase.GetMostSigOp().GetMostSigOp().GetValue() < 0 && ImgCase.GetLeastSigOp().GetValue() == 0.5;
                                               },
                                               [](const Exponent<Multiply<Real, Expression>, Real>& ImgCase, const void*) -> std::expected<gsl::not_null<std::unique_ptr<Expression>>, std::string_view> {
                                                   Multiply mul {
                                                       Multiply { Real { pow(std::abs(ImgCase.GetMostSigOp().GetMostSigOp().GetValue()), 0.5) },
                                                           Exponent { ImgCase.GetMostSigOp().GetLeastSigOp(), Real { 0.5 } } },
                                                       Imaginary {}
                                                   };

                                                   return gsl::make_not_null(mul.Copy());
                                               })
                                           .Case(
                                               [](const Exponent<Exponent<>, Expression>&) -> bool { return true; },
                                               [](const Exponent<Exponent<>, Expression>& expExpCase, const void*) -> std::expected<gsl::not_null<std::unique_ptr<Expression>>, std::string_view> {
                                                   SimplifyVisitor simplifyVisitor;
                                                   auto e = Multiply { expExpCase.GetMostSigOp().GetLeastSigOp(), expExpCase.GetLeastSigOp() };
                                                   auto s = e.Accept(simplifyVisitor);
                                                   if (!s) {
                                                       Exponent exp { expExpCase.GetMostSigOp().GetMostSigOp(), e };
                                                       return gsl::make_not_null(exp.Copy());
                                                   } else {
                                                       Exponent exp { expExpCase.GetMostSigOp().GetMostSigOp(), *(std::move(s).value()) };
                                                       return gsl::make_not_null(exp.Copy());
                                                   }
                                               })
                                           .Case(
                                               [](const Exponent<Expression, Log<>>& logCase) -> bool {
                                                   return logCase.GetMostSigOp().Equals(logCase.GetLeastSigOp().GetMostSigOp());
                                               },
                                               [](const Exponent<Expression, Log<>>& logCase, const void*) -> std::expected<gsl::not_null<std::unique_ptr<Expression>>, std::string_view> {
                                                   return gsl::make_not_null(logCase.GetLeastSigOp().GetLeastSigOp().Copy());
                                               });

    const auto simplifiedBase = mostSigOp->Simplify();
    const auto simplifiedPower = leastSigOp->Simplify();

    const Exponent simplifiedExponent { *simplifiedBase, *simplifiedPower };
    auto matchResult = match_cast.Execute(simplifiedExponent, nullptr).value();
    return matchResult ? std::move(matchResult) : std::move(simplifiedExponent.Copy());
}

auto Exponent<Expression>::Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression>
{
    SimplifyVisitor simplifyVisitor {};
    // variable integration
    if (auto variable = RecursiveCast<Variable>(integrationVariable); variable != nullptr) {
        auto simplified = this->Accept(simplifyVisitor);
        if (!simplified) {
            return this->Generalize();
        }
        auto simplifiedExponent = std::move(simplified).value();

        std::unique_ptr<Expression> integral;
        // Variable with a constant power
        if (auto realExponent = RecursiveCast<Exponent<Variable, Real>>(*simplifiedExponent); realExponent != nullptr) {
            const Variable& expBase = realExponent->GetMostSigOp();
            const Real& expPow = realExponent->GetLeastSigOp();

            if ((*variable).GetName() == expBase.GetName()) {

                Add adder {
                    Divide {
                        Exponent<Variable, Real> { Variable { (*variable).GetName() }, Real { expPow.GetValue() + 1 } },
                        Real { expPow.GetValue() + 1 } },
                    Variable { "C" }
                };

                return adder.Simplify();
            }
        }
    }

    Integral<Expression, Expression> integral { *(this->Copy()), *(integrationVariable.Copy()) };

    return integral.Copy();
}

auto Exponent<Expression>::Differentiate(const Expression& differentiationVariable) const -> std::unique_ptr<Expression>
{
    // variable diff
    SimplifyVisitor simplifyVisitor {};
    if (auto variable = RecursiveCast<Variable>(differentiationVariable); variable != nullptr) {
        auto simplified = this->Accept(simplifyVisitor);
        if (!simplified) {
            return this->Generalize();
        }
        auto simplifiedExponent = std::move(simplified).value();

        std::unique_ptr<Expression> diff;
        // Variable with a constant power
        if (auto realExponent = RecursiveCast<Exponent<Variable, Real>>(*simplifiedExponent); realExponent != nullptr) {
            const Variable& expBase = realExponent->GetMostSigOp();
            const Real& expPow = realExponent->GetLeastSigOp();

            if (variable->GetName() == expBase.GetName()) {
                return Multiply {
                    Real { expPow.GetValue() },
                    Exponent {
                        Variable { variable->GetName() },
                        Real { expPow.GetValue() - 1 } }
                }
                    .Simplify();
            }
        }

        if (auto natBase = RecursiveCast<Exponent<EulerNumber, Expression>>(*simplifiedExponent); natBase != nullptr) {
            Multiply derivative { Derivative { natBase->GetLeastSigOp(), differentiationVariable }, *simplifiedExponent };
            return derivative.Simplify();
        }

        if (auto realBase = RecursiveCast<Exponent<Real, Expression>>(*simplifiedExponent); realBase != nullptr) {
            Multiply derivative { Multiply { Derivative { realBase->GetLeastSigOp(), differentiationVariable }, *simplifiedExponent }, Log { EulerNumber {}, realBase->GetMostSigOp() } };
            return derivative.Simplify();
        }

        if (auto varBase = RecursiveCast<Exponent<Variable, Expression>>(*simplifiedExponent); varBase != nullptr) {
            const Variable& expBase = varBase->GetMostSigOp();
            if (expBase.GetName() != variable->GetName()) {
                Multiply derivative { Multiply { Derivative { varBase->GetLeastSigOp(), differentiationVariable }, *simplifiedExponent }, Log { EulerNumber {}, varBase->GetMostSigOp() } };
                return derivative.Simplify();
            }
        }

        if (auto generalCase = RecursiveCast<Exponent<Expression, Expression>>(*simplifiedExponent); generalCase != nullptr) {
            Multiply derivative { Multiply { *simplifiedExponent,
                Add { Divide { Multiply { generalCase->GetLeastSigOp(), Derivative { generalCase->GetMostSigOp(), differentiationVariable } }, generalCase->GetMostSigOp() },
                    Multiply { Derivative { generalCase->GetLeastSigOp(), differentiationVariable }, Log { EulerNumber {}, generalCase->GetMostSigOp() } } } } };
            return derivative.Simplify();
        }
    }

    return Derivative { *this, differentiationVariable }.Copy();
}

} // Oasis

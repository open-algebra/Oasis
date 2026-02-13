//
// Created by Matthew McCall on 8/10/23.
//

#include "Oasis/Multiply.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/EulerNumber.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Matrix.hpp"
#include "Oasis/RecursiveCast.hpp"

#include "Oasis/Log.hpp"
#include "Oasis/Subtract.hpp"

#define EPSILON 10E-6

namespace Oasis {
auto Multiply<Expression>::Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression>
{
    SimplifyVisitor simplifyVisitor {};
    // Single integration variable
    if (auto variable = RecursiveCast<Variable>(integrationVariable); variable != nullptr) {
        auto s = this->Accept(simplifyVisitor);
        if (!s) {
            return this->Generalize();
        }
        auto simplifiedMult = std::move(s).value();

        // Constant case - Constant number multiplied by integrand
        if (auto constant = RecursiveCast<Multiply<Real, Expression>>(*simplifiedMult); constant != nullptr) {
            auto exp = constant->GetLeastSigOp().Copy();
            auto num = constant->GetMostSigOp();
            auto integrated = exp->Integrate(integrationVariable);

            if (auto add = RecursiveCast<Add<Expression, Variable>>(*integrated); add != nullptr) {
                Add<Multiply<Real, Expression>, Variable> adder {
                    Multiply<Real, Expression> {
                        Real { num.GetValue() }, add->GetMostSigOp() },
                    Variable { "C" }
                };

                return adder.Accept(simplifyVisitor).value();
            }
        }

        // TODO: Implement integration by parts
        // Detect whether integration by parts is appropriate
        // May need to simplify before and/or after
        else if (auto mult = RecursiveCast<Multiply<Expression, Expression>>(*simplifiedMult); mult != nullptr) {
            std::unique_ptr<Expression> u;
            std::unique_ptr<Expression> dv;

            // Check the rules of LIPET

            // May need to recursively cast u and dv before assigning them
            // so that method overrides can be realized.

            // TODO: change dv to v and muliply by integration variable to correctly attain dv

            // Logarithm
            if (mult->GetMostSigOp().Is<Log<Expression, Expression>>()) {
                u = mult->GetMostSigOp().Copy();
                dv = mult->GetLeastSigOp().Copy();
            } else if (mult->GetLeastSigOp().Is<Log<Expression, Expression>>()) {
                u = mult->GetLeastSigOp().Copy();
                dv = mult->GetMostSigOp().Copy();
            }

            // TODO: Inverse trigonometry
            // Inverse trigonometry is not implemented yet in Oasis

            // Polynomial
            // TODO: Could also be exponent, in the case of (x^2)*sinx
            // TODO: Ensure all polynomial cases are accounted for
            if (mult->GetMostSigOp().Is<Variable>()) {
                u = mult->GetMostSigOp().Copy();
                dv = mult->GetLeastSigOp().Copy();
            } else if (mult->GetLeastSigOp().Is<Variable>()) {
                u = mult->GetLeastSigOp().Copy();
                dv = mult->GetMostSigOp().Copy();
            }

            // Exponential - Euler's Number
            if (mult->GetMostSigOp().Is<Exponent<EulerNumber, Expression>>()) {
                u = mult->GetMostSigOp().Copy();
                dv = mult->GetLeastSigOp().Copy();
            } else if (mult->GetLeastSigOp().Is<Exponent<EulerNumber, Expression>>()) {
                u = mult->GetLeastSigOp().Copy();
                dv = mult->GetMostSigOp().Copy();
            }

            // TODO: Trigonometry
            // Trigonometry is not implemented yet in Oasis

            // Differentiate u and integrate dv
            // TODO: ensure u & dv are casted as the correct types to allow method override
            std::unique_ptr<Expression> du = u->Differentiate(*u);
            std::unique_ptr<Expression> v = dv->Integrate(*dv);

            // Apply the formula: integral(udv) = uv - integral(vdu)
            // Work in Progress
            Subtract<Multiply<Expression, Expression>, Integral<Expression, Expression>> subtractor {
                Multiply<Expression, Expression> {
                    *u,
                    *dv },
                Integral {}
            };



            // test code to base the structure of above tree off of
            // Multiply<Oasis::Expression> subtract {
            //     Multiply<Expression, Expression> {
            //         Real { 1.0 },
            //         Real { 2.0 } },
            //     Real { 3.0 }
            // };






        }

    }
    Integral<Expression, Expression> integral { *(this->Copy()), *(integrationVariable.Copy()) };

    return integral.Copy();
}

auto Multiply<Expression>::Differentiate(const Expression& differentiationVariable) const -> std::unique_ptr<Expression>
{
    SimplifyVisitor simplifyVisitor {};
    // Single integration variable
    if (auto variable = RecursiveCast<Variable>(differentiationVariable); variable != nullptr) {
        auto s = this->Accept(simplifyVisitor);
        if (!s) {
            return this->Generalize();
        }
        auto simplifiedMult = std::move(s).value();

        // Constant case - Constant number multiplied by differentiate
        if (auto constant = RecursiveCast<Multiply<Real, Expression>>(*simplifiedMult); constant != nullptr) {
            auto exp = constant->GetLeastSigOp().Copy();
            auto num = constant->GetMostSigOp();
            auto differentiate = (*exp).Differentiate(differentiationVariable);
            if (auto add = RecursiveCast<Expression>(*differentiate); add != nullptr) {
                return std::make_unique<Multiply<Real, Expression>>(Multiply<Real, Expression> { Real { num.GetValue() }, *(add->Accept(simplifyVisitor).value()) })->Accept(simplifyVisitor).value();
            }

        }
        // Product rule: d/dx (f(x)*g(x)) = f'(x)*g(x) + f(x)*g'(x)
        else if (auto product = RecursiveCast<Multiply<Expression, Expression>>(*simplifiedMult); product != nullptr) {
            auto left = product->GetMostSigOp().Copy();
            auto right = product->GetLeastSigOp().Copy();
            auto ld = left->Differentiate(differentiationVariable);
            auto rd = right->Differentiate(differentiationVariable);
            auto add = RecursiveCast<Expression>(*ld);
            auto add2 = RecursiveCast<Expression>(*rd);
            if ((add != nullptr && add2 != nullptr)) {
                return std::make_unique<Add<Multiply<Expression, Expression>,
                    Multiply<Expression, Expression>>>(Add<Multiply<Expression, Expression>, Multiply<Expression, Expression>> { Multiply<Expression, Expression> { *add,
                                                                                                                                     *right },
                                                           Multiply<Expression, Expression> { *add2, *left } })
                    ->Accept(simplifyVisitor)
                    .value();
            }
        }
    }

    return Copy();
}

} // Oasis
//
// Created by Matthew McCall on 8/10/23.
//
#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"
#include <map>
#include <vector>

namespace Oasis {

Divide<Expression>::Divide(const Expression& dividend, const Expression& divisor)
    : BinaryExpression(dividend, divisor)
{
}

auto Divide<Expression>::Simplify() const -> std::unique_ptr<Expression>
{
    auto simplifiedDividend = mostSigOp->Simplify(); // numerator
    auto simplifiedDivider = leastSigOp->Simplify(); // denominator
    Divide simplifiedDivide { *simplifiedDividend, *simplifiedDivider };

    if (auto realCase = Divide<Real>::Specialize(simplifiedDivide); realCase != nullptr) {
        const Real& dividend = realCase->GetMostSigOp();
        const Real& divisor = realCase->GetLeastSigOp();
        return std::make_unique<Real>(dividend.GetValue() / divisor.GetValue());
    }

    // log(a)/log(b)=log[b](a)
    if (auto logCase = Divide<Log<Expression, Expression>, Log<Expression, Expression>>::Specialize(simplifiedDivide); logCase != nullptr) {
        if (logCase->GetMostSigOp().GetMostSigOp().Equals(logCase->GetLeastSigOp().GetMostSigOp())) {
            const IExpression auto& base = logCase->GetLeastSigOp().GetLeastSigOp();
            const IExpression auto& argument = logCase->GetMostSigOp().GetLeastSigOp();
            return std::make_unique<Log<Expression>>(base, argument);
        }
    }
    // convert the terms in numerator and denominator into a vector to make manipulations easier
    std::vector<std::unique_ptr<Expression>> numerator;
    std::vector<std::unique_ptr<Expression>> denominator;
    std::vector<std::unique_ptr<Expression>> result;
    std::vector<std::unique_ptr<Expression>> numeratorVals;
    std::vector<std::unique_ptr<Expression>> denominatorVals;

    if (auto multipliedNumerator = Multiply<Expression>::Specialize(*simplifiedDividend); multipliedNumerator != nullptr) {
        multipliedNumerator->Flatten(numerator);
    } else {
        numerator.push_back(simplifiedDividend->Copy());
    }

    if (auto multipliedDenominator = Multiply<Expression>::Specialize(*simplifiedDivider); multipliedDenominator != nullptr) {
        multipliedDenominator->Flatten(denominator);
    } else {
        denominator.push_back(simplifiedDivider->Copy());
    }

    // now that we have the terms in a vector, we have to cancel like terms and simplify them
    result.reserve(numerator.size());
    for (const auto& num : numerator) {
        result.push_back(num->Copy());
    }

    for (const auto& denom : denominator) {
        size_t i = 0;
        if (auto real = Real::Specialize(*denom); real != nullptr) {
            for (; i < result.size(); i++) {
                if (auto resI = Real::Specialize(*result[i]); resI != nullptr) {
                    result[i] = Real { resI->GetValue() / real->GetValue() }.Generalize();
                    break;
                }
            }
            if (i >= result.size()) {
                result.push_back(Real { 1 / real->GetValue() }.Generalize());
            }
            continue;
        }
        if (auto img = Imaginary::Specialize(*denom); img != nullptr) {
            for (; i < result.size(); i++) {
                if (auto resI = Imaginary::Specialize(*result[i]); resI != nullptr) {
                    result[i] = Real { 1.0 }.Generalize();
                    break;
                }
            }
            if (i >= result.size()) {
                result.push_back(Exponent<Expression> { Imaginary {}, Real { -1.0 } }.Generalize());
            }
            continue;
        }
        if (auto var = Variable::Specialize(*denom); var != nullptr) {
            for (; i < result.size(); i++) {
                if (auto resIexp = Exponent<Variable, Expression>::Specialize(*result[i]); resIexp != nullptr) {
                    if (resIexp->GetMostSigOp().Equals(*var)) {
                        result[i] = Exponent<Expression> { *var, *(Subtract<Expression> { resIexp->GetLeastSigOp(), Real { 1.0 } }.Simplify()) }.Generalize();
                        break;
                    }
                } else if (auto resI = Variable::Specialize(*result[i]); resI != nullptr) {
                    if (resI->Equals(*var)) {
                        result[i] = Real { 1.0 }.Generalize();
                        break;
                    }
                }
            }
            if (i >= result.size()) {
                result.push_back(Exponent<Expression> { *var, Real { -1.0 } }.Generalize());
            }
            continue;
        }
        if (auto var = Exponent<Variable, Expression>::Specialize(*denom); var != nullptr) {
            for (; i < result.size(); i++) {
                if (auto resIexp = Exponent<Variable, Expression>::Specialize(*result[i]); resIexp != nullptr) {
                    if (resIexp->GetMostSigOp().Equals(var->GetMostSigOp())) {
                        result[i] = Exponent<Expression> { var->GetMostSigOp(), *(Subtract<Expression> { resIexp->GetLeastSigOp(), var->GetLeastSigOp() }.Simplify()) }.Generalize();
                        break;
                    }
                } else if (auto resI = Variable::Specialize(*result[i]); resI != nullptr) {
                    if (resI->Equals(*var)) {
                        result[i] = Exponent<Expression> { var->GetMostSigOp(), *(Subtract<Expression> { Real { 1.0 }, var->GetLeastSigOp() }.Simplify()) }.Generalize();
                    }
                }
            }
            if (i >= result.size()) {
                result.push_back(Exponent<Expression> { var->GetMostSigOp(), Multiply { Real { -1.0 }, var->GetLeastSigOp() } }.Generalize());
            }
            continue;
        }
        if (auto expExpr = Exponent<Expression>::Specialize(*denom); expExpr != nullptr) {
            for (; i < result.size(); i++) {
                if (auto resExpr = Exponent<Expression, Expression>::Specialize(*result[i]); resExpr != nullptr) {
                    if (expExpr->GetMostSigOp().Equals(resExpr->GetMostSigOp())) {
                        result[i] = Exponent { expExpr->GetMostSigOp(), *(Subtract { resExpr->GetLeastSigOp(), expExpr->GetLeastSigOp() }.Simplify()) }.Generalize();
                        break;
                    }
                } else if (result[i]->Equals(expExpr->GetMostSigOp())) {
                    result[i] = Exponent { expExpr->GetMostSigOp(), *(Subtract { Real { 1.0 }, resExpr->GetLeastSigOp() }.Simplify()) }.Generalize();
                    break;
                }
            }
            if (i >= result.size()) {
                result.push_back(Exponent<Expression> { expExpr->GetMostSigOp(), Multiply { Real { -1.0 }, expExpr->GetLeastSigOp() } }.Generalize());
            }
            continue;
        }
        for (; i < result.size(); i++) {
            if (auto resExpr = Exponent<Expression, Expression>::Specialize(*result[i]); resExpr != nullptr) {
                if (denom->Equals(resExpr->GetMostSigOp())) {
                    result[i] = Exponent { *denom, *(Subtract { resExpr->GetLeastSigOp(), Real { 1.0 } }.Simplify()) }.Generalize();
                    break;
                }
            } else if (result[i]->Equals(*denom)) {
                result[i] = Real { 1.0 }.Generalize();
                break;
            }
        }
        if (i >= result.size()) {
            result.push_back(Exponent<Expression> { *denom, Real { -1.0 } }.Generalize());
        }
    }

    // rebuild into tree
    for (const auto& val : result) {
        if (auto valI = Real::Specialize(*val); valI != nullptr) {
            if (valI->GetValue() != 1.0) {
                numeratorVals.push_back(valI->Generalize());
            }
        } else if (auto var = Variable::Specialize(*val); var != nullptr) {
            numeratorVals.push_back(val->Generalize());
        } else if (auto img = Imaginary::Specialize(*val); img != nullptr) {
            numeratorVals.push_back(val->Generalize());
        } else if (auto expR = Exponent<Expression, Real>::Specialize(*val); expR != nullptr) {
            if (expR->GetLeastSigOp().GetValue() < 0.0) {
                denominatorVals.push_back(Exponent { expR->GetMostSigOp(), Real { expR->GetLeastSigOp().GetValue() * -1.0 } }.Generalize());
            } else {
                numeratorVals.push_back(val->Generalize());
            }
        } else if (auto exp = Exponent<Expression, Multiply<Real, Expression>>::Specialize(*val); exp != nullptr) {
            if (exp->GetLeastSigOp().GetMostSigOp().GetValue() < 0.0) {
                denominatorVals.push_back(Exponent { exp->GetMostSigOp(), exp->GetLeastSigOp().GetLeastSigOp() }.Generalize());
            } else {
                numeratorVals.push_back(val->Generalize());
            }
        } else {
            numeratorVals.push_back(val->Generalize());
        }
    }

    // makes expr^1 into expr
    for (auto& val : numeratorVals) {
        if (auto exp = Exponent<Expression, Real>::Specialize(*val); exp != nullptr) {
            if (exp->GetLeastSigOp().GetValue() == 1.0) {
                val = exp->GetMostSigOp().Generalize();
            }
        }
    }

    for (auto& val : denominatorVals) {
        if (auto exp = Exponent<Expression, Real>::Specialize(*val); exp != nullptr) {
            if (exp->GetLeastSigOp().GetValue() == 1.0) {
                val = exp->GetMostSigOp().Generalize();
            }
        }
    }

    auto dividend = numeratorVals.size() == 1 ? std::move(numeratorVals.front()) : BuildFromVector<Multiply>(numeratorVals);
    auto divisor = denominatorVals.size() == 1 ? std::move(denominatorVals.front()) : BuildFromVector<Multiply>(denominatorVals);

    // rebuild subtrees
    if (!dividend && divisor)
        return Divide { Real { 1.0 }, *divisor }.Copy();

    if (dividend && !divisor)
        return dividend;

    return Divide { *dividend, *divisor }.Copy();
}

auto Divide<Expression>::ToString() const -> std::string
{
    return fmt::format("({} / {})", mostSigOp->ToString(), leastSigOp->ToString());
}

auto Divide<Expression>::Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression>
{
    std::unique_ptr<Expression> simplifiedDividend, simplifiedDivisor;

    tf::Task leftSimplifyTask = subflow.emplace([this, &simplifiedDividend](tf::Subflow& sbf) {
        if (!mostSigOp) {
            return;
        }

        simplifiedDividend = mostSigOp->Simplify(sbf);
    });

    tf::Task rightSimplifyTask = subflow.emplace([this, &simplifiedDivisor](tf::Subflow& sbf) {
        if (!leastSigOp) {
            return;
        }

        simplifiedDivisor = leastSigOp->Simplify(sbf);
    });

    Divide simplifiedDivide;

    // While this task isn't actually parallelized, it exists as a prerequisite for check possible cases in parallel
    tf::Task simplifyTask = subflow.emplace([&simplifiedDivide, &simplifiedDividend, &simplifiedDivisor](tf::Subflow&) {
        if (simplifiedDividend) {
            simplifiedDivide.SetMostSigOp(*simplifiedDividend);
        }

        if (simplifiedDivisor) {
            simplifiedDivide.SetLeastSigOp(*simplifiedDivisor);
        }
    });

    simplifyTask.succeed(leftSimplifyTask, rightSimplifyTask);

    std::unique_ptr<Divide<Real>> realCase;

    tf::Task realCaseTask = subflow.emplace([&simplifiedDivide, &realCase](tf::Subflow& sbf) {
        realCase = Divide<Real>::Specialize(simplifiedDivide, sbf);
    });

    simplifyTask.precede(realCaseTask);

    subflow.join();

    if (realCase) {
        const Real& multiplicand = realCase->GetMostSigOp();
        const Real& multiplier = realCase->GetLeastSigOp();

        return std::make_unique<Real>(multiplicand.GetValue() / multiplier.GetValue());
    }

    return simplifiedDivide.Copy();
}

auto Divide<Expression>::Specialize(const Expression& other) -> std::unique_ptr<Divide<Expression, Expression>>
{
    if (!other.Is<Oasis::Divide>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize();
    return std::make_unique<Divide>(dynamic_cast<const Divide&>(*otherGeneralized));
}

auto Divide<Expression>::Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Divide>
{
    if (!other.Is<Oasis::Divide>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize(subflow);
    return std::make_unique<Divide>(dynamic_cast<const Divide&>(*otherGeneralized));
}

auto Divide<Expression>::Differentiate(const Oasis::Expression& differentiationVariable) -> std::unique_ptr<Expression>
{
    // Single differentiation variable
    if (auto variable = Variable::Specialize(differentiationVariable); variable != nullptr) {
        auto simplifiedDiv = this->Simplify();

        // Constant case - differentiation over a divisor
        if (auto constant = Divide<Expression, Real>::Specialize(*simplifiedDiv); constant != nullptr) {
            auto exp = constant->GetMostSigOp().Copy();
            auto num = constant->GetLeastSigOp();
            auto differentiate = (*exp).Differentiate(differentiationVariable);
            if (auto add = Expression::Specialize(*differentiate); add != nullptr) {
                return std::make_unique<Divide<Expression, Real>>(Divide<Expression, Real> { *(add->Simplify()), Real { num.GetValue() } })->Simplify();
            }
        }
        // In case of simplify turning divide into mult
        if (auto constant = Multiply<Expression, Real>::Specialize(*simplifiedDiv); constant != nullptr) {
            auto exp = constant->GetMostSigOp().Copy();
            auto num = constant->GetLeastSigOp();
            auto differentiate = (*exp).Differentiate(differentiationVariable);
            if (auto add = Expression::Specialize(*differentiate); add != nullptr) {
                return std::make_unique<Multiply<Expression, Real>>(Multiply<Expression, Real> { *(add->Simplify()), Real { num.GetValue() } })->Simplify();
            }
        }
        // Quotient Rule: d/dx (f(x)/g(x)) = (g(x)f'(x)-f(x)g'(x))/(g(x)^2)
        if (auto quotient = Divide<Expression, Expression>::Specialize(*simplifiedDiv); quotient != nullptr) {
            auto leftexp = quotient->GetMostSigOp().Copy();
            auto rightexp = quotient->GetLeastSigOp().Copy();
            auto leftDiff = leftexp->Differentiate(differentiationVariable);
            auto rightDiff = rightexp->Differentiate(differentiationVariable);
            auto mult1 = Multiply<Expression, Expression>(Multiply<Expression, Expression> { *(rightexp->Simplify()), *(leftDiff->Simplify()) }).Simplify()->Simplify();
            auto mult2 = Multiply<Expression, Expression>(Multiply<Expression, Expression> { *(leftexp->Simplify()), *(rightDiff->Simplify()) }).Simplify()->Simplify();
            auto numerator = Subtract<Expression, Expression>(Subtract<Expression, Expression> { *mult1, *mult2 }).Simplify();
            auto denominator = Multiply<Expression, Expression>(Multiply<Expression, Expression> { *(rightexp->Simplify()), *(rightexp->Simplify()) }).Simplify();
            return Divide<Expression, Expression>({ *(numerator->Simplify()), *(denominator->Simplify()) }).Simplify();
        }
    }

    return Copy();
}

} // Oasis
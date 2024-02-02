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

    // Factor the expression here
    // Rest is written assuming factor function is complete

    if (auto realCase = Divide<Real>::Specialize(simplifiedDivide); realCase != nullptr) {
        const Real& dividend = realCase->GetMostSigOp();
        const Real& divisor = realCase->GetLeastSigOp();
        return std::make_unique<Real>(dividend.GetValue() / divisor.GetValue());
    }

    /*
    if (auto likeTermsCase = Divide<Multiply<Real, Expression>>::Specialize(simplifiedDivide); likeTermsCase != nullptr) {
        const Oasis::IExpression auto& leftTerm = likeTermsCase->GetMostSigOp().GetLeastSigOp();
        const Oasis::IExpression auto& rightTerm = likeTermsCase->GetLeastSigOp().GetLeastSigOp();
        const Real& coefficient1 = likeTermsCase->GetMostSigOp().GetMostSigOp();
        const Real& coefficient2 = likeTermsCase->GetLeastSigOp().GetMostSigOp();

        if (leftTerm.Equals(rightTerm)) {
            return std::make_unique<Real>(coefficient1.GetValue() / coefficient2.GetValue());
        }
        std::unordered_map<std::string, double> variables;

        const Oasis::IExpression auto& holderLeft = likeTermsCase->GetMostSigOp().GetLeastSigOp();
        const Oasis::IExpression auto& holderRight = likeTermsCase->GetLeastSigOp().GetLeastSigOp();

        auto leftover = holderLeft.Generalize();

        std::list<std::pair<std::unique_ptr<Expression>, double>> topexpress;

        // Variables
        for (auto sortingLeft = Multiply<Variable, Expression>::Specialize(holderLeft); sortingLeft != nullptr;) {
            if (auto it = variables.find(sortingLeft->GetMostSigOp().GetName()); it == variables.end())
                variables.insert(std::make_pair(sortingLeft->GetMostSigOp().GetName(), 0));
            variables[sortingLeft->GetMostSigOp().GetName()]++;
            leftover = sortingLeft->GetLeastSigOp().Generalize();
            sortingLeft = Multiply<Variable, Expression>::Specialize(sortingLeft->GetLeastSigOp());
        }

        // Exponents
        for (auto sortingLeft = Multiply<Exponent<Variable, Real>, Expression>::Specialize(*leftover); sortingLeft != nullptr;) {
            if (auto it = variables.find(sortingLeft->GetMostSigOp().GetMostSigOp().GetName()); it == variables.end())
                variables.insert(std::make_pair(sortingLeft->GetMostSigOp().GetMostSigOp().GetName(), 0));
            variables[sortingLeft->GetMostSigOp().GetMostSigOp().GetName()] += sortingLeft->GetMostSigOp().GetLeastSigOp().GetValue();
            leftover = sortingLeft->GetLeastSigOp().Generalize();
            sortingLeft = Multiply<Exponent<Variable, Real>, Expression>::Specialize(sortingLeft->GetLeastSigOp());
        }

        // Expressions
        for (auto sortingLeft = Multiply<Expression, Expression>::Specialize(*leftover); sortingLeft != nullptr;) {
            if (sortingLeft->GetLeastSigOp().GetType() == ExpressionType::Exponent) {
                auto useable = sortingLeft->Generalize();
                const auto& sortingLeftLeastSigOp = dynamic_cast<const Exponent<Expression>&>(*useable);
                double val = dynamic_cast<const Real&>(sortingLeftLeastSigOp.GetMostSigOp()).GetValue();
                topexpress.push_back(std::make_pair(sortingLeftLeastSigOp.GetLeastSigOp().Copy(), val));
            } else {
                topexpress.push_back(std::make_pair(sortingLeft->GetLeastSigOp().Copy(), 1));
            }
            leftover = sortingLeft->GetMostSigOp().Generalize();
            sortingLeft = Multiply<Expression, Expression>::Specialize(sortingLeft->GetLeastSigOp());
        }

        if (leftover->GetType() == ExpressionType::Variable) {
            auto temp = Variable::Specialize(*leftover);
            if (auto it = variables.find(temp->GetName()); it == variables.end())
                variables.insert(std::make_pair(temp->GetName(), 0));
            variables[temp->GetName()]++;
        } else if (auto temp = Exponent<Variable, Real>::Specialize(*leftover); temp != nullptr) {
            if (auto it = variables.find(temp->GetMostSigOp().GetName()); it == variables.end())
                variables.insert(std::make_pair(temp->GetMostSigOp().GetName(), 0));
            variables[temp->GetMostSigOp().GetName()] += temp->GetLeastSigOp().GetValue();
        } else {
            auto check = Exponent<Expression, Real>::Specialize(*leftover);
            if (check != nullptr) {
                auto useable = check->Generalize();
                const auto& sortingLeftLeastSigOp = dynamic_cast<const Exponent<Expression>&>(*useable);
                double val = dynamic_cast<const Real&>(sortingLeftLeastSigOp.GetMostSigOp()).GetValue();
                topexpress.push_back(std::make_pair(sortingLeftLeastSigOp.GetLeastSigOp().Copy(), val));
            } else {
                topexpress.push_back(std::make_pair(leftover->Generalize(), 1));
            }
        }

        leftover = holderRight.Generalize();
        for (auto sortingRight = Multiply<Variable, Expression>::Specialize(holderRight); sortingRight != nullptr;) {
            if (auto it = variables.find(sortingRight->GetMostSigOp().GetName()); it == variables.end())
                variables.insert(std::make_pair(sortingRight->GetMostSigOp().GetName(), 0));
            variables[sortingRight->GetMostSigOp().GetName()]--;
            leftover = Variable::Specialize(sortingRight->GetLeastSigOp());
            sortingRight = Multiply<Variable, Expression>::Specialize(sortingRight->GetLeastSigOp());
        }
        for (auto sortingRight = Multiply<Exponent<Variable, Real>, Expression>::Specialize(*leftover); sortingRight != nullptr;) {
            if (auto it = variables.find(sortingRight->GetMostSigOp().GetMostSigOp().GetName()); it == variables.end())
                variables.insert(std::make_pair(sortingRight->GetMostSigOp().GetMostSigOp().GetName(), 0));
            variables[sortingRight->GetMostSigOp().GetMostSigOp().GetName()] += sortingRight->GetMostSigOp().GetLeastSigOp().GetValue();
            leftover = sortingRight->GetLeastSigOp().Generalize();
            sortingRight = Multiply<Exponent<Variable, Real>, Expression>::Specialize(sortingRight->GetLeastSigOp());
        }
        for (auto sortingRight = Multiply<Expression, Expression>::Specialize(*leftover); sortingRight != nullptr;) {
            bool checked = true;
            if (sortingRight->GetLeastSigOp().GetType() == ExpressionType::Exponent) {
                auto useable = sortingRight->Generalize();
                const auto& sortingRightLeastSigOp = dynamic_cast<const Exponent<Expression>&>(*useable);
                double val = dynamic_cast<const Real&>(sortingRightLeastSigOp.GetMostSigOp()).GetValue();
                std::list<std::pair<std::unique_ptr<Expression>, double>>::iterator it;
                for (it = topexpress.begin(); it != topexpress.end(); ++it) {
                    if (it->first == sortingRightLeastSigOp.GetLeastSigOp().Copy()) {
                        checked = false;
                        topexpress.push_back(std::make_pair(sortingRightLeastSigOp.GetLeastSigOp().Copy(), it->second - val));
                        topexpress.erase(it);
                    }
                }
                if (checked) {
                    topexpress.push_back(std::make_pair(sortingRightLeastSigOp.GetLeastSigOp().Copy(), -val));
                }
            } else {
                std::list<std::pair<std::unique_ptr<Expression>, double>>::iterator it;
                for (it = topexpress.begin(); it != topexpress.end(); ++it) {
                    if (it->first == sortingRight->GetLeastSigOp().Copy()) {
                        checked = false;
                        topexpress.push_back(std::make_pair(sortingRight->GetLeastSigOp().Copy(), it->second - 1));
                        topexpress.erase(it);
                    }
                }
                if (checked) {
                    topexpress.push_back(std::make_pair(sortingRight->GetLeastSigOp().Copy(), -1));
                }
            }
            leftover = sortingRight->GetLeastSigOp().Generalize();
            sortingRight = Multiply<Expression, Expression>::Specialize(sortingRight->GetLeastSigOp());
        }
        if (leftover->GetType() == ExpressionType::Variable) {
            auto temp = Variable::Specialize(*leftover);
            if (auto it = variables.find(temp->GetName()); it == variables.end())
                variables.insert(std::make_pair(temp->GetName(), 0));
            variables[temp->GetName()]--;
        } else if (auto temp = Exponent<Variable, Real>::Specialize(*leftover); temp != nullptr) {
            if (auto it = variables.find(temp->GetMostSigOp().GetName()); it == variables.end())
                variables.insert(std::make_pair(temp->GetMostSigOp().GetName(), 0));
            variables[temp->GetMostSigOp().GetName()] -= temp->GetLeastSigOp().GetValue();
        } else {
            auto check = Exponent<Expression, Real>::Specialize(*leftover);
            bool checked = true;
            if (check != nullptr) {
                auto useable = check->Generalize();
                const auto& sortingRightLeastSigOp = dynamic_cast<const Exponent<Expression>&>(*useable);
                double val = dynamic_cast<const Real&>(sortingRightLeastSigOp.GetMostSigOp()).GetValue();
                std::list<std::pair<std::unique_ptr<Expression>, double>>::iterator it;
                for (it = topexpress.begin(); it != topexpress.end(); ++it) {
                    if (it->first == sortingRightLeastSigOp.GetLeastSigOp().Copy()) {
                        checked = false;
                        topexpress.push_back(std::make_pair(sortingRightLeastSigOp.GetLeastSigOp().Copy(), it->second - val));
                        topexpress.erase(it);
                    }
                }
                if (checked) {
                    topexpress.push_back(std::make_pair(sortingRightLeastSigOp.GetLeastSigOp().Copy(), -val));
                }
            } else {
                std::list<std::pair<std::unique_ptr<Expression>, double>>::iterator it;
                for (it = topexpress.begin(); it != topexpress.end(); ++it) {
                    if ((*(it->first)).Equals(*leftover->Generalize())) {
                        checked = false;
                        topexpress.push_back(std::make_pair(leftover->Generalize(), it->second - 1));
                        topexpress.erase(it);
                        break;
                    }
                }
                if (checked) {
                    topexpress.push_back(std::make_pair(leftover->Generalize(), -1));
                }
            }
        }
        std::vector<std::unique_ptr<Expression>> top;
        std::vector<std::unique_ptr<Expression>> bot;

        for (auto it = topexpress.begin(); it != topexpress.end(); it++) {
            if (it->second == 0)
                continue;
            else if (it->second == 1)
                top.push_back(std::move(it->first));
            else if (it->second == -1)
                bot.push_back(std::move(it->first));
            else if (it->second > 0) {
                std::unique_ptr<Expression> filler = std::move(it->first);
                top.push_back(std::make_unique<Exponent<Expression /, Real/>>(*filler, Real(it->second)));
            } else if (it->second < 0)
                bot.push_back(std::make_unique<Exponent<Expression /, Real/>>(*(it->first), Real(-1 * (it->second))));
        }

        for (auto it = variables.begin(); it != variables.end(); it++) {
            if (it->second == 0)
                continue;
            else if (it->second == 1)
                top.push_back(std::make_unique<Variable>(it->first));
            else if (it->second == -1)
                bot.push_back(std::make_unique<Variable>(it->first));
            else if (it->second > 0)
                top.push_back(std::make_unique<Exponent<Variable, Real>>(Variable(it->first), Real(it->second)));
            else if (it->second < 0)
                bot.push_back(std::make_unique<Exponent<Variable, Real>>(Variable(it->first), Real(-1 * (it->second))));
        }
        if (bot.size() != 0 && top.size() != 0) {
            return std::make_unique<Divide<Expression>>(Multiply<Expression>(Real(coefficient1.GetValue() / coefficient2.GetValue()), *(BuildFromVector<Oasis::Multiply>(top))), *(BuildFromVector<Oasis::Multiply>(bot)));
        }
        if (top.size() != 0) {
            return std::make_unique<Multiply<Expression>>(Real(coefficient1.GetValue() / coefficient2.GetValue()), *(BuildFromVector<Oasis::Multiply>(top)));
        }
        if (bot.size() != 0) {
            return std::make_unique<Divide<Expression>>(Real(coefficient1.GetValue() / coefficient2.GetValue()), *(BuildFromVector<Oasis::Multiply>(bot)));
        } else {
            return std::make_unique<Real>(coefficient1.GetValue() / coefficient2.GetValue());
        }
    }*/

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
                        // subtract exp
                        break;
                    }
                } else if (result[i]->Equals(expExpr->GetMostSigOp())) {
                    // subtract exp
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
                    // subtract exp
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

    // rebuild subtrees
    if (!numeratorVals.empty() && !denominatorVals.empty())
        return Divide { *(BuildFromVector<Multiply>(numeratorVals)), *(BuildFromVector<Multiply>(denominatorVals)) }.Generalize();
    else if (!numeratorVals.empty() && denominatorVals.empty())
        return BuildFromVector<Multiply>(numeratorVals);
    else
        return Divide { Real { 1.0 }, *(BuildFromVector<Multiply>(denominatorVals)) }.Generalize();
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

} // Oasis
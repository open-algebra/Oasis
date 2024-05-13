//
// Created by Matthew McCall on 7/2/23.
//
#include <unordered_map>

#include "Oasis/Add.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Multiply.hpp"

#define EPSILON 10E-6

namespace Oasis {

auto Add<Expression>::Simplify() const -> std::unique_ptr<Expression>
{
    auto simplifiedAugend = mostSigOp ? mostSigOp->Simplify() : nullptr;
    auto simplifiedAddend = leastSigOp ? leastSigOp->Simplify() : nullptr;

    Add simplifiedAdd { *simplifiedAugend, *simplifiedAddend };

    if (auto realCase = Add<Real>::Specialize(simplifiedAdd); realCase != nullptr) {
        const Real& firstReal = realCase->GetMostSigOp();
        const Real& secondReal = realCase->GetLeastSigOp();

        return std::make_unique<Real>(firstReal.GetValue() + secondReal.GetValue());
    }

    if (auto zeroCase = Add<Real, Expression>::Specialize(simplifiedAdd); zeroCase != nullptr) {
        if (zeroCase->GetMostSigOp().GetValue() == 0) {
            return zeroCase->GetLeastSigOp().Generalize();
        }
    }

    if (auto likeTermsCase = Add<Multiply<Real, Expression>>::Specialize(simplifiedAdd); likeTermsCase != nullptr) {
        const Oasis::IExpression auto& leftTerm = likeTermsCase->GetMostSigOp().GetLeastSigOp();
        const Oasis::IExpression auto& rightTerm = likeTermsCase->GetLeastSigOp().GetLeastSigOp();

        if (leftTerm.Equals(rightTerm)) {
            const Real& coefficient1 = likeTermsCase->GetMostSigOp().GetMostSigOp();
            const Real& coefficient2 = likeTermsCase->GetLeastSigOp().GetMostSigOp();

            return std::make_unique<Multiply<Expression>>(Real(coefficient1.GetValue() + coefficient2.GetValue()), leftTerm);
        }
    }

    // log(a) + log(b) = log(ab)
    if (auto logCase = Add<Log<Expression, Expression>, Log<Expression, Expression>>::Specialize(simplifiedAdd); logCase != nullptr) {
        if (logCase->GetMostSigOp().GetMostSigOp().Equals(logCase->GetLeastSigOp().GetMostSigOp())) {
            const IExpression auto& base = logCase->GetMostSigOp().GetMostSigOp();
            const IExpression auto& argument = Multiply<Expression>({ logCase->GetMostSigOp().GetLeastSigOp(), logCase->GetLeastSigOp().GetLeastSigOp() });
            return std::make_unique<Log<Expression>>(base, argument);
        }
    }

    // x + x = 2x
    if (simplifiedAugend->Equals(*simplifiedAddend)) {
        return Multiply<Real, Expression> { Real { 2.0 }, *simplifiedAugend }.Simplify();
    }

    // 2x + x = 3x
    if (const auto likeTermsCase2 = Add<Multiply<Real, Expression>, Expression>::Specialize(simplifiedAdd); likeTermsCase2 != nullptr) {
        if (likeTermsCase2->GetMostSigOp().GetLeastSigOp().Equals(likeTermsCase2->GetLeastSigOp())) {
            const Real& coeffiecent = likeTermsCase2->GetMostSigOp().GetMostSigOp();
            return std::make_unique<Multiply<Real, Expression>>(Real { coeffiecent.GetValue() + 1 }, likeTermsCase2->GetMostSigOp().GetLeastSigOp());
        }
    }

    // simplifies expressions and combines like terms
    // ex: 1 + 2x + 3 + 5x = 4 + 7x (or 7x + 4)
    std::vector<std::unique_ptr<Expression>> adds;
    std::vector<std::unique_ptr<Expression>> vals;
    simplifiedAdd.Flatten(adds);
    for (const auto& addend : adds) {
        // real
        size_t i = 0;
        if (auto real = Real::Specialize(*addend); real != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = Real::Specialize(*vals[i]); valI != nullptr) {
                    vals[i] = Real { valI->GetValue() + real->GetValue() }.Generalize();
                    break;
                }
            }
            if (i >= vals.size()) {
                // check to make sure it is one thing only
                vals.push_back(addend->Generalize());
            }
            continue;
        }
        // single i
        if (auto img = Imaginary::Specialize(*addend); img != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = Multiply<Expression, Imaginary>::Specialize(*vals[i]); valI != nullptr) {
                    vals[i] = Multiply<Expression> { *(Add<Expression> { valI->GetMostSigOp(), Real { 1.0 } }.Simplify()), Imaginary {} }.Generalize();
                    break;
                }
            }
            if (i >= vals.size()) {
                // check to make sure it is one thing only
                vals.push_back(Multiply<Expression> { Real { 1.0 }, Imaginary {} }.Generalize());
            }
            continue;
        }
        // n*i
        if (auto img = Multiply<Expression, Imaginary>::Specialize(*addend); img != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = Multiply<Expression, Imaginary>::Specialize(*vals[i]); valI != nullptr) {
                    vals[i] = Multiply<Expression> { *(Add<Expression> { valI->GetMostSigOp(), img->GetMostSigOp() }.Simplify()), Imaginary {} }.Generalize();
                    break;
                }
            }
            if (i >= vals.size()) {
                // check to make sure it is one thing only
                vals.push_back(img->Generalize());
            }
            continue;
        }
        // single variable
        if (auto var = Variable::Specialize(*addend); var != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = Multiply<Expression, Variable>::Specialize(*vals[i]); valI != nullptr) {
                    if (valI->GetLeastSigOp().GetName() == var->GetName()) {
                        vals[i] = Multiply<Expression> { *(Add<Expression> { valI->GetMostSigOp(), Real { 1.0 } }.Simplify()), *var }.Generalize();
                        break;
                    } else
                        continue;
                }
            }
            if (i >= vals.size()) {
                // check to make sure it is one thing only
                vals.push_back(Multiply<Expression> { Real { 1.0 }, *var }.Generalize());
            }
            continue;
        }
        // n*variable
        if (auto var = Multiply<Expression, Variable>::Specialize(*addend); var != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = Multiply<Expression, Variable>::Specialize(*vals[i]); valI != nullptr) {
                    // if (auto zeroCase = Multiply<Real, Expression>::Specialize(*valI); zeroCase != nullptr) {}
                    if (valI->GetLeastSigOp().GetName() == var->GetLeastSigOp().GetName()) {
                        vals[i] = Multiply<Expression> { *(Add<Expression> { valI->GetMostSigOp(), var->GetMostSigOp() }.Simplify()), valI->GetLeastSigOp() }.Generalize();
                        break;
                    } else
                        continue;
                }
            }
            if (i >= vals.size()) {
                // check to make sure it is one thing only
                vals.push_back(var->Generalize());
            }
            continue;
        }
        // single exponent
        if (auto exp = Exponent<Expression>::Specialize(*addend); exp != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = Multiply<Expression, Exponent<Expression>>::Specialize(*vals[i]); valI != nullptr) {
                    if (valI->GetLeastSigOp().Equals(*exp)) {
                        vals[i] = Multiply<Expression> { *(Add<Expression> { valI->GetMostSigOp(), Real { 1.0 } }.Simplify()), *exp }.Generalize();
                        break;
                    } else
                        continue;
                }
            }
            if (i >= vals.size()) {
                // check to make sure it is one thing only
                vals.push_back(Multiply<Expression> { Real { 1.0 }, *exp }.Generalize());
            }
            continue;
        }
        // n*exponent
        if (auto exp = Multiply<Expression, Exponent<Expression>>::Specialize(*addend); exp != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = Multiply<Expression, Exponent<Expression>>::Specialize(*vals[i]); valI != nullptr) {
                    if (valI->GetLeastSigOp().Equals(exp->GetLeastSigOp())) {
                        vals[i] = Multiply<Expression> { *(Add<Expression> { valI->GetMostSigOp(), exp->GetMostSigOp() }.Simplify()), valI->GetLeastSigOp() }.Generalize();
                        break;
                    } else
                        continue;
                }
            }
            if (i >= vals.size()) {
                // check to make sure it is one thing only
                vals.push_back(exp->Generalize());
            }
            continue;
        }
    }
    // rebuild equation after simplification.

    for (auto& val : vals) {
        if (auto mul = Multiply<Real, Expression>::Specialize(*val); mul != nullptr) {
            if (mul->GetMostSigOp().GetValue() == 1.0) {
                val = mul->GetLeastSigOp().Generalize();
            }
        }
    }

    // filter out zero-equivalent expressions
    std::vector<std::unique_ptr<Expression>> avals;
    for (auto& val : vals) {
        if (auto real = Real::Specialize(*val); real != nullptr) {
            if (std::abs(real->GetValue()) <= EPSILON) {
                continue;
            }
        }
        if (auto mul = Multiply<Real, Expression>::Specialize(*val); mul != nullptr) {
            if (std::abs(mul->GetMostSigOp().GetValue()) <= EPSILON) {
                continue;
            }
        }
        avals.push_back(val->Generalize());
    }

    if (auto vec = BuildFromVector<Add>(avals); vec != nullptr) {
        return vec;
    }

    return simplifiedAdd.Copy();
}

auto Add<Expression>::Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression>
{
    std::unique_ptr<Expression> simplifiedAugend, simplifiedAddend;

    tf::Task leftSimplifyTask = subflow.emplace([this, &simplifiedAugend](tf::Subflow& sbf) {
        if (!mostSigOp) {
            return;
        }

        simplifiedAugend = mostSigOp->Simplify(sbf);
    });

    tf::Task rightSimplifyTask = subflow.emplace([this, &simplifiedAddend](tf::Subflow& sbf) {
        if (!leastSigOp) {
            return;
        }

        simplifiedAddend = leastSigOp->Simplify(sbf);
    });

    Add simplifiedAdd;

    // While this task isn't actually parallelized, it exists as a prerequisite for check possible cases in parallel
    tf::Task simplifyTask = subflow.emplace([&simplifiedAdd, &simplifiedAugend, &simplifiedAddend](tf::Subflow&) {
        if (simplifiedAugend) {
            simplifiedAdd.SetMostSigOp(*simplifiedAugend);
        }

        if (simplifiedAddend) {
            simplifiedAdd.SetLeastSigOp(*simplifiedAddend);
        }
    });

    simplifyTask.succeed(leftSimplifyTask, rightSimplifyTask);

    std::unique_ptr<Add<Real>> realCase;

    tf::Task realCaseTask = subflow.emplace([&simplifiedAdd, &realCase](tf::Subflow& sbf) {
        realCase = Add<Real>::Specialize(simplifiedAdd, sbf);
    });

    simplifyTask.precede(realCaseTask);

    subflow.join();

    if (realCase) {
        const Real& firstReal = realCase->GetMostSigOp();
        const Real& secondReal = realCase->GetLeastSigOp();

        return std::make_unique<Real>(firstReal.GetValue() + secondReal.GetValue());
    }

    return simplifiedAdd.Copy();
}

auto Add<Expression>::Specialize(const Expression& other) -> std::unique_ptr<Add<Expression, Expression>>
{
    if (!other.Is<Oasis::Add>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize();
    return std::make_unique<Add>(dynamic_cast<const Add&>(*otherGeneralized));
}

auto Add<Expression>::Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Add>
{
    if (!other.Is<Oasis::Add>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize(subflow);
    return std::make_unique<Add>(dynamic_cast<const Add&>(*otherGeneralized));
}

auto Add<Expression>::Integrate(const Expression& integrationVariable) -> std::unique_ptr<Expression>
{
    // Single integration variable
    if (auto variable = Variable::Specialize(integrationVariable); variable != nullptr) {
        auto simplifiedAdd = this->Simplify();

        // Make sure we're still adder
        if (auto adder = Add<Expression>::Specialize(*simplifiedAdd); adder != nullptr) {
            auto leftRef = adder->GetLeastSigOp().Copy();
            auto leftIntegral = leftRef->Integrate(integrationVariable);

            auto specializedLeft = Add<Expression>::Specialize(*leftIntegral);

            auto rightRef = adder->GetMostSigOp().Copy();
            auto rightIntegral = rightRef->Integrate(integrationVariable);

            auto specializedRight = Add<Expression>::Specialize(*rightIntegral);
            if (specializedLeft == nullptr || specializedRight == nullptr) {
                return Copy();
            }
            Add<Expression> add {
                Add<Expression, Expression> {
                    *(specializedLeft->GetMostSigOp().Copy()), *(specializedRight->GetMostSigOp().Copy()) },
                Variable { "C" }
            };

            return add.Simplify();
        }
        // If not, use other integration technique
        else {
            return simplifiedAdd->Integrate(integrationVariable)->Simplify();
        }
    }
    Integral<Expression, Expression> integral { *(this->Copy()), *(integrationVariable.Copy()) };

    return integral.Copy();
}

auto Add<Expression>::Differentiate(const Expression& differentiationVariable) const -> std::unique_ptr<Expression>
{
    if (auto variable = Variable::Specialize(differentiationVariable); variable != nullptr) {
        auto simplifiedAdd = this->Simplify();
        if (auto adder = Add<Expression>::Specialize(*simplifiedAdd); adder != nullptr) {
            auto leftRef = adder->GetLeastSigOp().Copy();
            auto leftDifferentiate = leftRef->Differentiate(differentiationVariable);

            auto specializedLeft = Expression::Specialize(*leftDifferentiate);
            auto rightRef = adder->GetMostSigOp().Copy();

            auto rightDifferentiate = rightRef->Differentiate(differentiationVariable);
            auto specializedRight = Expression::Specialize(*rightDifferentiate);

            if (specializedLeft == nullptr || specializedRight == nullptr) {
                return Copy();
            }
            return std::make_unique<Add<Expression, Expression>>(Add<Expression, Expression> { *(specializedLeft->Copy()), *(specializedRight->Copy()) })->Simplify();
        } else {
            return simplifiedAdd->Differentiate(differentiationVariable)->Simplify();
        }
    }
    return Copy();
}

} // Oasis
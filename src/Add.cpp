//
// Created by Matthew McCall on 7/2/23.
//

#include "Oasis/Add.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Multiply.hpp"

namespace Oasis {

Add<Expression>::Add(const Expression& addend1, const Expression& addend2)
    : BinaryExpression(addend1, addend2)
{
}

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

    if (auto likeTermsCase = Add<Multiply<Real, Expression>>::Specialize(simplifiedAdd); likeTermsCase != nullptr) {
        const Oasis::IExpression auto& leftTerm = likeTermsCase->GetMostSigOp().GetLeastSigOp();
        const Oasis::IExpression auto& rightTerm = likeTermsCase->GetLeastSigOp().GetLeastSigOp();

        if (leftTerm.Equals(rightTerm)) {
            const Real& coefficient1 = likeTermsCase->GetMostSigOp().GetMostSigOp();
            const Real& coefficient2 = likeTermsCase->GetLeastSigOp().GetMostSigOp();

            return std::make_unique<Multiply<Expression>>(Real(coefficient1.GetValue() + coefficient2.GetValue()), leftTerm);
        }
    }

    if (auto ImgCase = Add<Imaginary>::Specialize(simplifiedAdd); ImgCase != nullptr) {
        return std::make_unique<Multiply<Real, Imaginary>>(Real { 2.0 }, Imaginary {});
    }

    if (auto ImgCase = Add<Multiply<Expression, Imaginary>, Imaginary>::Specialize(simplifiedAdd); ImgCase != nullptr) {
        return std::make_unique<Multiply<Expression>>(
            *(Add { Real { 1.0 }, ImgCase->GetMostSigOp().GetMostSigOp() }.Simplify()), Imaginary {});
    }

    if (auto ImgCase = Add<Multiply<Expression, Imaginary>, Multiply<Expression, Imaginary>>::Specialize(simplifiedAdd); ImgCase != nullptr) {
        return std::make_unique<Multiply<Expression>>(
            *(Add { ImgCase->GetLeastSigOp().GetMostSigOp(), ImgCase->GetMostSigOp().GetMostSigOp() }.Simplify()), Imaginary {});
    }

    // exponent + exponent
    if (auto exponentCase = Add<Exponent<Expression>, Exponent<Expression>>::Specialize(simplifiedAdd); exponentCase != nullptr) {
        if (exponentCase->GetMostSigOp().GetMostSigOp().Equals(exponentCase->GetLeastSigOp().GetMostSigOp()) && exponentCase->GetMostSigOp().GetLeastSigOp().Equals(exponentCase->GetLeastSigOp().GetLeastSigOp())) {
            return std::make_unique<Multiply<Expression>>(Real { 2.0 }, exponentCase->GetMostSigOp());
        }
    }

    // a*exponent + exponent
    if (auto exponentCase = Add<Multiply<Expression, Exponent<Expression>>, Exponent<Expression>>::Specialize(simplifiedAdd); exponentCase != nullptr) {
        if (exponentCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exponentCase->GetLeastSigOp().GetMostSigOp()) && exponentCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp().Equals(exponentCase->GetLeastSigOp().GetLeastSigOp())) {
            return std::make_unique<Multiply<Expression>>(*(Add<Expression> { exponentCase->GetMostSigOp().GetMostSigOp(), Real { 1.0 } }.Simplify()),
                exponentCase->GetLeastSigOp());
        }
    }

    if (auto exponentCase = Add<Exponent<Expression>, Multiply<Expression, Exponent<Expression>>>::Specialize(simplifiedAdd); exponentCase != nullptr) {
        if (exponentCase->GetLeastSigOp().GetLeastSigOp().GetMostSigOp().Equals(exponentCase->GetMostSigOp().GetMostSigOp()) && exponentCase->GetLeastSigOp().GetLeastSigOp().GetLeastSigOp().Equals(exponentCase->GetMostSigOp().GetLeastSigOp())) {
            return std::make_unique<Multiply<Expression>>(*(Add<Expression> { exponentCase->GetMostSigOp().GetMostSigOp(), Real { 1.0 } }.Simplify()),
                exponentCase->GetLeastSigOp());
        }
    }

    // a*exponent+b*exponent
    if (auto exponentCase = Add<Multiply<Expression, Exponent<Expression>>, Multiply<Expression, Exponent<Expression>>>::Specialize(simplifiedAdd); exponentCase != nullptr) {
        if (exponentCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exponentCase->GetLeastSigOp().GetLeastSigOp().GetMostSigOp()) && exponentCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp().Equals(exponentCase->GetLeastSigOp().GetLeastSigOp().GetLeastSigOp())) {
            return std::make_unique<Multiply<Expression>>(*(Add<Expression> { exponentCase->GetMostSigOp().GetMostSigOp(), exponentCase->GetLeastSigOp().GetMostSigOp() }.Simplify()),
                exponentCase->GetLeastSigOp());
        }
    }

    // simplifies expressions and combines like terms
    // ex: 1 + 2x + 3 + 5x = 4 + 7x (or 7x + 4)
    std::vector<std::unique_ptr<Expression>> adds;
    std::vector<std::unique_ptr<Expression>> vals;
    simplifiedAdd.Flatten(adds);
    for (const auto& addend : adds) {
        // real
        int i = 0;
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
                // vals.push_back(Multiply<Expression> { img->GetMostSigOp(), Imaginary {} }.Generalize());
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
                    if (valI->GetLeastSigOp().GetName() == var->GetLeastSigOp().GetName()) {
                        vals[i] = Multiply<Expression> { *(Add<Expression> { valI->GetMostSigOp(), var->GetMostSigOp() }.Simplify()), valI->GetLeastSigOp() }.Generalize();
                        break;
                    } else
                        continue;
                }
            }
            if (i >= vals.size()) {
                // check to make sure it is one thing only
                // vals.push_back(Multiply<Expression> { var->GetMostSigOp(), var->GetLeastSigOp() }.Generalize());
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
                // vals.push_back(Multiply<Expression> { var->GetMostSigOp(), var->GetLeastSigOp() }.Generalize());
                vals.push_back(exp->Generalize());
            }
            continue;
        }
    }
    // rebuild equation after simplification.

    return BuildFromVector<Add>(vals);

    // return simplifiedAdd.Copy();
}

auto Add<Expression>::ToString() const -> std::string
{
    return fmt::format("({} + {})", mostSigOp->ToString(), leastSigOp->ToString());
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

} // Oasis
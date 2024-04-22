//
// Created by Matthew McCall on 8/10/23.
//

#include "Oasis/Multiply.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"

namespace Oasis {

auto Multiply<Expression>::Simplify() const -> std::unique_ptr<Expression>
{
    auto simplifiedMultiplicand = mostSigOp->Simplify();
    auto simplifiedMultiplier = leastSigOp->Simplify();

    Multiply simplifiedMultiply { *simplifiedMultiplicand, *simplifiedMultiplier };
    if (auto onezerocase = Multiply<Real, Expression>::Specialize(simplifiedMultiply); onezerocase != nullptr) {
        const Real& multiplicand = onezerocase->GetMostSigOp();
        const Expression& multiplier = onezerocase->GetLeastSigOp();
        if (multiplicand.GetValue() == 0) {
            return std::make_unique<Real>(Real { 0 });
        }
        if (multiplicand.GetValue() == 1) {
            return multiplier.Simplify();
        }
    }
    if (auto realCase = Multiply<Real>::Specialize(simplifiedMultiply); realCase != nullptr) {
        const Real& multiplicand = realCase->GetMostSigOp();
        const Real& multiplier = realCase->GetLeastSigOp();
        return std::make_unique<Real>(multiplicand.GetValue() * multiplier.GetValue());
    }

    if (auto ImgCase = Multiply<Imaginary>::Specialize(simplifiedMultiply); ImgCase != nullptr) {
        return std::make_unique<Real>(-1.0);
    }
    if (auto exprCase = Multiply<Expression>::Specialize(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().Equals(exprCase->GetLeastSigOp())) {
            return std::make_unique<Exponent<Expression, Expression>>(exprCase->GetMostSigOp(), Real { 2.0 });
        }
    }

    if (auto exprCase = Multiply<Expression, Exponent<Expression, Expression>>::Specialize(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetMostSigOp())) {
            return std::make_unique<Exponent<Expression>>(exprCase->GetMostSigOp(),
                *(Add<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } }.Simplify()));
        }
    }

    // x*x^n
    if (auto exprCase = Multiply<Expression, Exponent<Expression>>::Specialize(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetMostSigOp())) {
            return std::make_unique<Exponent<Expression>>(exprCase->GetMostSigOp(),
                *(Add<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } }.Simplify()));
        }
    }

    if (auto exprCase = Multiply<Exponent<Expression>, Expression>::Specialize(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetLeastSigOp().Equals(exprCase->GetMostSigOp().GetMostSigOp())) {
            return std::make_unique<Exponent<Expression>>(exprCase->GetLeastSigOp(),
                *(Add<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(), Real { 1.0 } }.Simplify()));
        }
    }

    // x^n*x^m
    if (auto exprCase = Multiply<Exponent<Expression>, Exponent<Expression>>::Specialize(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetMostSigOp())) {
            return std::make_unique<Exponent<Expression>>(exprCase->GetMostSigOp().GetMostSigOp(),
                *(Add<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(), exprCase->GetLeastSigOp().GetLeastSigOp() }.Simplify()));
        }
    }

    // a*x*x
    if (auto exprCase = Multiply<Multiply<Expression>, Expression>::Specialize(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().Equals(exprCase->GetLeastSigOp())) {
            return std::make_unique<Multiply<Expression, Expression>>(exprCase->GetMostSigOp().GetMostSigOp(),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(), Real { 2.0 } });
        }
    }

    // a*x*b*x
    if (auto exprCase = Multiply<Multiply<Expression>, Multiply<Expression>>::Specialize(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().Equals(exprCase->GetLeastSigOp().GetLeastSigOp())) {
            return std::make_unique<Multiply<Expression>>(
                *(Multiply<Expression> { exprCase->GetMostSigOp().GetMostSigOp(), exprCase->GetLeastSigOp().GetMostSigOp() }.Simplify()),
                *(Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(), Real { 2.0 } }.Simplify()));
        }
    }

    // a*x^n*x
    if (auto exprCase = Multiply<Multiply<Expression, Exponent<Expression>>, Expression>::Specialize(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exprCase->GetLeastSigOp())) {
            return std::make_unique<Multiply<Expression>>(exprCase->GetMostSigOp().GetMostSigOp(),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp(),
                    *(Add<Expression> { exprCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } }.Simplify()) });
        }
    }

    // a*x*x^n
    if (auto exprCase = Multiply<Multiply<Expression>, Exponent<Expression>>::Specialize(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().Equals(exprCase->GetLeastSigOp().GetMostSigOp())) {
            return std::make_unique<Multiply<Expression>>(exprCase->GetMostSigOp().GetMostSigOp(),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(),
                    *(Add<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } }.Simplify()) });
        }
        if (exprCase->GetMostSigOp().GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetMostSigOp())) {
            return std::make_unique<Multiply<Expression>>(exprCase->GetMostSigOp().GetLeastSigOp(),
                Exponent<Expression> { exprCase->GetMostSigOp().GetMostSigOp(),
                    *(Add<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } }.Simplify()) });
        }
    }

    // a*x^n*b*x
    if (auto exprCase = Multiply<Multiply<Expression>, Multiply<Expression, Exponent<Expression>>>::Specialize(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().Equals(exprCase->GetLeastSigOp().GetLeastSigOp().GetMostSigOp())) {
            return std::make_unique<Multiply<Expression>>(
                *(Multiply<Expression> { exprCase->GetMostSigOp().GetMostSigOp(), exprCase->GetLeastSigOp().GetMostSigOp() }.Simplify()),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(),
                    *(Add<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } }.Simplify()) });
        }
    }

    if (auto exprCase = Multiply<Multiply<Expression>, Multiply<Exponent<Expression>, Expression>>::Specialize(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().Equals(exprCase->GetLeastSigOp().GetMostSigOp().GetMostSigOp())) {
            return std::make_unique<Multiply<Expression>>(
                *(Multiply<Expression> { exprCase->GetMostSigOp().GetMostSigOp(), exprCase->GetLeastSigOp().GetLeastSigOp() }.Simplify()),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(),
                    *(Add<Expression> { exprCase->GetLeastSigOp().GetMostSigOp().GetLeastSigOp(), Real { 1.0 } }.Simplify()) });
        }
    }

    if (auto exprCase = Multiply<Multiply<Expression, Exponent<Expression>>, Multiply<Expression>>::Specialize(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetLeastSigOp())) {
            return std::make_unique<Multiply<Expression>>(
                *(Multiply<Expression> { exprCase->GetMostSigOp().GetMostSigOp(), exprCase->GetLeastSigOp().GetLeastSigOp() }.Simplify()),
                Exponent<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp(),
                    *(Add<Expression> { exprCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } }.Simplify()) });
        }
    }

    // a*x^n*x^m
    if (auto exprCase = Multiply<Multiply<Expression, Exponent<Expression>>, Exponent<Expression>>::Specialize(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetLeastSigOp())) {
            return std::make_unique<Multiply<Expression>>(
                exprCase->GetMostSigOp().GetMostSigOp(),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp(),
                    *(Add<Expression> { exprCase->GetLeastSigOp().GetMostSigOp(), exprCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp() }.Simplify()) });
        }
    }

    // a*x^n*b*x^m
    if (auto exprCase = Multiply<Multiply<Expression, Exponent<Expression>>, Multiply<Expression, Exponent<Expression>>>::Specialize(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetLeastSigOp().GetMostSigOp())) {
            return std::make_unique<Multiply<Expression>>(
                *(Multiply<Expression> { exprCase->GetMostSigOp().GetMostSigOp(), exprCase->GetLeastSigOp().GetMostSigOp() }.Simplify()),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp(),
                    *(Add<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp().GetLeastSigOp(), exprCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp() }.Simplify()) });
        }
    }

    // multiply add like terms
    std::vector<std::unique_ptr<Expression>> multiplies;
    std::vector<std::unique_ptr<Expression>> vals;
    simplifiedMultiply.Flatten(multiplies);
    for (const auto& multiplicand : multiplies) {
        size_t i = 0;
        if (auto real = Real::Specialize(*multiplicand); real != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = Real::Specialize(*vals[i]); valI != nullptr) {
                    vals[i] = Real { valI->GetValue() * real->GetValue() }.Generalize();
                    break;
                }
            }
            if (i >= vals.size()) {
                // check to make sure it is one thing only
                vals.push_back(multiplicand->Generalize());
            }
            continue;
        }
        // single i
        if (auto img = Imaginary::Specialize(*multiplicand); img != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = Exponent<Imaginary, Expression>::Specialize(*vals[i]); valI != nullptr) {
                    vals[i] = Exponent<Expression> { Imaginary {}, *(Add<Expression> { valI->GetLeastSigOp(), Real { 1.0 } }.Simplify()) }.Generalize();
                    break;
                }
            }
            if (i >= vals.size()) {
                // check to make sure it is one thing only
                vals.push_back(Exponent<Expression> { Imaginary {}, Real { 1.0 } }.Generalize());
            }
            continue;
        }
        // i^n
        if (auto img = Exponent<Imaginary, Expression>::Specialize(*multiplicand); img != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = Exponent<Imaginary, Expression>::Specialize(*vals[i]); valI != nullptr) {
                    vals[i] = Exponent<Expression> { Imaginary {}, *(Add<Expression> { valI->GetLeastSigOp(), img->GetLeastSigOp() }.Simplify()) }.Generalize();
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
        // expr^n
        if (auto expr = Exponent<Expression, Expression>::Specialize(*multiplicand); expr != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = Exponent<Expression, Expression>::Specialize(*vals[i]); valI != nullptr) {
                    if (valI->GetMostSigOp().Equals(expr->GetMostSigOp())) {
                        vals[i] = Exponent<Expression> { valI->GetMostSigOp(), *(Add<Expression> { valI->GetLeastSigOp(), expr->GetLeastSigOp() }.Simplify()) }.Generalize();
                        break;
                    }
                }
            }
            if (i >= vals.size()) {
                // check to make sure it is one thing only
                // vals.push_back(Multiply<Expression> { img->GetMostSigOp(), Imaginary {} }.Generalize());
                vals.push_back(expr->Generalize());
            }
            continue;
        }
        // single expr
        if (auto expr = Expression::Specialize(*multiplicand); expr != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = Exponent<Expression, Expression>::Specialize(*vals[i]); valI != nullptr) {
                    if (valI->GetMostSigOp().Equals(*expr)) {
                        vals[i] = Exponent<Expression> { valI->GetMostSigOp(), *(Add<Expression> { valI->GetLeastSigOp(), Real { 1.0 } }.Simplify()) }.Generalize();
                        break;
                    }
                }
            }
            if (i >= vals.size()) {
                // check to make sure it is one thing only
                vals.push_back(Exponent<Expression> { *expr, Real { 1.0 } }.Generalize());
            }
            continue;
        }
    }

    // makes all expr^1 into expr
    for (auto& val : vals) {
        if (auto exp = Exponent<Expression, Real>::Specialize(*val); exp != nullptr) {
            if (exp->GetLeastSigOp().GetValue() == 1.0) {
                val = exp->GetMostSigOp().Generalize();
            }
        }
        if (auto mul = Multiply<Real, Expression>::Specialize(*val); mul != nullptr) {
            if (mul->GetMostSigOp().GetValue() == 1.0) {
                val = mul->GetLeastSigOp().Generalize();
            }
        }
    }

    return BuildFromVector<Multiply>(vals);

    // return simplifiedMultiply.Copy();
}

auto Multiply<Expression>::ToString() const -> std::string
{
    return fmt::format("({} * {})", mostSigOp->ToString(), leastSigOp->ToString());
}

auto Multiply<Expression>::Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression>
{
    std::unique_ptr<Expression> simplifiedMultiplicand, simplifiedMultiplier;

    tf::Task leftSimplifyTask = subflow.emplace([this, &simplifiedMultiplicand](tf::Subflow& sbf) {
        if (!mostSigOp) {
            return;
        }

        simplifiedMultiplicand = mostSigOp->Simplify(sbf);
    });

    tf::Task rightSimplifyTask = subflow.emplace([this, &simplifiedMultiplier](tf::Subflow& sbf) {
        if (!leastSigOp) {
            return;
        }

        simplifiedMultiplier = leastSigOp->Simplify(sbf);
    });

    Multiply simplifiedMultiply;

    // While this task isn't actually parallelized, it exists as a prerequisite for check possible cases in parallel
    tf::Task simplifyTask = subflow.emplace([&simplifiedMultiply, &simplifiedMultiplicand, &simplifiedMultiplier](tf::Subflow&) {
        if (simplifiedMultiplicand) {
            simplifiedMultiply.SetMostSigOp(*simplifiedMultiplicand);
        }

        if (simplifiedMultiplier) {
            simplifiedMultiply.SetLeastSigOp(*simplifiedMultiplier);
        }
    });

    simplifyTask.succeed(leftSimplifyTask, rightSimplifyTask);

    std::unique_ptr<Multiply<Real>> realCase;

    tf::Task realCaseTask = subflow.emplace([&simplifiedMultiply, &realCase](tf::Subflow& sbf) {
        realCase = Multiply<Real>::Specialize(simplifiedMultiply, sbf);
    });

    simplifyTask.precede(realCaseTask);

    subflow.join();

    if (realCase) {
        const Real& multiplicand = realCase->GetMostSigOp();
        const Real& multiplier = realCase->GetLeastSigOp();

        return std::make_unique<Real>(multiplicand.GetValue() * multiplier.GetValue());
    }

    return simplifiedMultiply.Copy();
}

auto Multiply<Expression>::Specialize(const Expression& other) -> std::unique_ptr<Multiply<Expression, Expression>>
{
    if (!other.Is<Oasis::Multiply>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize();
    return std::make_unique<Multiply>(dynamic_cast<const Multiply&>(*otherGeneralized));
}

auto Multiply<Expression>::Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Multiply>
{
    if (!other.Is<Oasis::Multiply>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize(subflow);
    return std::make_unique<Multiply>(dynamic_cast<const Multiply&>(*otherGeneralized));
}

auto Multiply<Expression>::Differentiate(const Expression& differentiationVariable) -> std::unique_ptr<Expression>
{
    // Single integration variable
    if (auto variable = Variable::Specialize(differentiationVariable); variable != nullptr) {
        auto simplifiedMult = this->Simplify();

        // Constant case - Constant number multiplied by differentiate
        if (auto constant = Multiply<Real, Expression>::Specialize(*simplifiedMult); constant != nullptr) {
            auto exp = constant->GetLeastSigOp().Copy();
            auto num = constant->GetMostSigOp();
            auto differentiate = (*exp).Differentiate(differentiationVariable);
            if (auto add = Expression::Specialize(*differentiate); add != nullptr) {
                return std::make_unique<Multiply<Real, Expression>>(Multiply<Real, Expression> { Real { num.GetValue() }, *(add->Simplify()) })->Simplify();
            }

        }
        // Product rule: d/dx (f(x)*g(x)) = f'(x)*g(x) + f(x)*g'(x)
        else if (auto product = Multiply<Expression, Expression>::Specialize(*simplifiedMult); product != nullptr) {
            auto left = product->GetMostSigOp().Copy();
            auto right = product->GetLeastSigOp().Copy();
            auto ld = left->Differentiate(differentiationVariable);
            auto rd = right->Differentiate(differentiationVariable);
            auto add = Expression::Specialize(*ld);
            auto add2 = Expression::Specialize(*rd);
            if ((add != nullptr && add2 != nullptr)) {
                return std::make_unique<Add<Multiply<Expression, Expression>,
                    Multiply<Expression, Expression>>>(Add<Multiply<Expression, Expression>, Multiply<Expression, Expression>> { Multiply<Expression, Expression> { *add,
                                                                                                                                     *right },
                                                           Multiply<Expression, Expression> { *add2, *left } })
                    ->Simplify();
            }
        }
    }

    return Copy();
}

} // Oasis
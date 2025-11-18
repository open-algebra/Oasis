//
// Created by Matthew McCall on 7/2/23.
//
#include <unordered_map>

#include "Oasis/Add.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/MatchCast.hpp"
#include "Oasis/Matrix.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/RecursiveCast.hpp"

#define EPSILON 10E-6

namespace Oasis {

auto Add<Expression>::Simplify() const -> std::unique_ptr<Expression>
{
    auto simplifiedAugend = mostSigOp ? mostSigOp->Simplify() : nullptr;
    auto simplifiedAddend = leastSigOp ? leastSigOp->Simplify() : nullptr;

    Add simplifiedAdd { *simplifiedAugend, *simplifiedAddend };

    if (auto realCase = RecursiveCast<Add<Real>>(simplifiedAdd); realCase != nullptr) {
        const Real& firstReal = realCase->GetMostSigOp();
        const Real& secondReal = realCase->GetLeastSigOp();

        return std::make_unique<Real>(firstReal.GetValue() + secondReal.GetValue());
    }

    if (auto zeroCase = RecursiveCast<Add<Real, Expression>>(simplifiedAdd); zeroCase != nullptr) {
        if (zeroCase->GetMostSigOp().GetValue() == 0) {
            return zeroCase->GetLeastSigOp().Generalize();
        }
    }

    if (auto likeTermsCase = RecursiveCast<Add<Multiply<Real, Expression>>>(simplifiedAdd); likeTermsCase != nullptr) {
        const Oasis::IExpression auto& leftTerm = likeTermsCase->GetMostSigOp().GetLeastSigOp();
        const Oasis::IExpression auto& rightTerm = likeTermsCase->GetLeastSigOp().GetLeastSigOp();

        if (leftTerm.Equals(rightTerm)) {
            const Real& coefficient1 = likeTermsCase->GetMostSigOp().GetMostSigOp();
            const Real& coefficient2 = likeTermsCase->GetLeastSigOp().GetMostSigOp();

            return std::make_unique<Multiply<Expression>>(Real(coefficient1.GetValue() + coefficient2.GetValue()), leftTerm);
        }
    }

    // matrix + matrix
    if (auto matrixCase = RecursiveCast<Add<Matrix, Matrix>>(simplifiedAdd); matrixCase != nullptr) {
        const Oasis::IExpression auto& leftTerm = matrixCase->GetMostSigOp();
        const Oasis::IExpression auto& rightTerm = matrixCase->GetLeastSigOp();

        if ((leftTerm.GetRows() == rightTerm.GetRows()) && (leftTerm.GetCols() == rightTerm.GetCols())) {
            return std::make_unique<Matrix>(leftTerm.GetMatrix() + rightTerm.GetMatrix());
        } else {
            return std::make_unique<Add<Expression>>(leftTerm, rightTerm);
        }
    }

    // log(a) + log(b) = log(ab)
    if (auto logCase = RecursiveCast<Add<Log<Expression, Expression>, Log<Expression, Expression>>>(simplifiedAdd); logCase != nullptr) {
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
    if (const auto likeTermsCase2 = RecursiveCast<Add<Multiply<Real, Expression>, Expression>>(simplifiedAdd); likeTermsCase2 != nullptr) {
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
        if (auto real = RecursiveCast<Real>(*addend); real != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = RecursiveCast<Real>(*vals[i]); valI != nullptr) {
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
        if (auto img = RecursiveCast<Imaginary>(*addend); img != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = RecursiveCast<Multiply<Expression, Imaginary>>(*vals[i]); valI != nullptr) {
                    auto e = Add<Expression> { valI->GetMostSigOp(), Real { 1.0 } };
                    SimplifyVisitor sv {};
                    auto s = e.Accept(sv);
                    if (!s) {
                        vals[i] = Multiply<Expression> { e, Imaginary {} }.Generalize();
                        break;
                    } else {
                        vals[i] = Multiply<Expression> { *(std::move(s).value()), Imaginary {} }.Generalize();
                        break;
                    }
                }
            }
            if (i >= vals.size()) {
                // check to make sure it is one thing only
                vals.push_back(Multiply<Expression> { Real { 1.0 }, Imaginary {} }.Generalize());
            }
            continue;
        }
        // n*i
        if (auto img = RecursiveCast<Multiply<Expression, Imaginary>>(*addend); img != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = RecursiveCast<Multiply<Expression, Imaginary>>(*vals[i]); valI != nullptr) {
                    auto e = Add<Expression> { valI->GetMostSigOp(), img->GetMostSigOp() };
                    SimplifyVisitor sv {};
                    auto s = e.Accept(sv);
                    if (!s) {
                        vals[i] = Multiply<Expression> { e, Imaginary {} }.Generalize();
                        break;
                    } else {
                        vals[i] = Multiply<Expression> { *(std::move(s).value()), Imaginary {} }.Generalize();
                        break;
                    }
                }
            }
            if (i >= vals.size()) {
                // check to make sure it is one thing only
                vals.push_back(img->Generalize());
            }
            continue;
        }
        // single variable
        if (auto var = RecursiveCast<Variable>(*addend); var != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = RecursiveCast<Multiply<Expression, Variable>>(*vals[i]); valI != nullptr) {
                    if (valI->GetLeastSigOp().GetName() == var->GetName()) {
                        auto e = Add<Expression> { valI->GetMostSigOp(), Real { 1.0 } };
                        SimplifyVisitor sv {};
                        auto s = e.Accept(sv);
                        if (!s) {
                            vals[i] = Multiply<Expression> { e, *var }.Generalize();
                            break;
                        } else {
                            vals[i] = Multiply<Expression> { *(std::move(s).value()), *var }.Generalize();
                            break;
                        }
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
        if (auto var = RecursiveCast<Multiply<Expression, Variable>>(*addend); var != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = RecursiveCast<Multiply<Expression, Variable>>(*vals[i]); valI != nullptr) {
                    // if (auto zeroCase = RecursiveCast<Multiply<Real, Expression>>(*valI); zeroCase != nullptr) {}
                    if (valI->GetLeastSigOp().GetName() == var->GetLeastSigOp().GetName()) {
                        auto e = Add<Expression> { valI->GetMostSigOp(), var->GetMostSigOp() };
                        SimplifyVisitor sv {};
                        auto s = e.Accept(sv);
                        if (!s) {
                            vals[i] = Multiply<Expression> { e, valI->GetLeastSigOp() }.Generalize();
                            break;
                        } else {
                            vals[i] = Multiply<Expression> { *(std::move(s).value()), valI->GetLeastSigOp() }.Generalize();
                            break;
                        }
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
        if (auto exp = RecursiveCast<Exponent<Expression>>(*addend); exp != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = RecursiveCast<Multiply<Expression, Exponent<Expression>>>(*vals[i]); valI != nullptr) {
                    if (valI->GetLeastSigOp().Equals(*exp)) {
                        auto e = Add<Expression> { valI->GetMostSigOp(), Real { 1.0 } };
                        SimplifyVisitor sv {};
                        auto s = e.Accept(sv);
                        if (!s) {
                            vals[i] = Multiply<Expression> { e, *exp }.Generalize();
                            break;
                        } else {
                            vals[i] = Multiply<Expression> { *(std::move(s).value()), *exp }.Generalize();
                            break;
                        }
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
        if (auto exp = RecursiveCast<Multiply<Expression, Exponent<Expression>>>(*addend); exp != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = RecursiveCast<Multiply<Expression, Exponent<Expression>>>(*vals[i]); valI != nullptr) {
                    if (valI->GetLeastSigOp().Equals(exp->GetLeastSigOp())) {
                        auto e = Add<Expression> { valI->GetMostSigOp(), exp->GetMostSigOp() };
                        SimplifyVisitor sv {};
                        auto s = e.Accept(sv);
                        if (!s) {
                            vals[i] = Multiply<Expression> { e, valI->GetLeastSigOp() }.Generalize();
                            break;
                        } else {
                            vals[i] = Multiply<Expression> { *(std::move(s).value()), valI->GetLeastSigOp() }.Generalize();
                            break;
                        }
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
        if (auto mul = RecursiveCast<Multiply<Real, Expression>>(*val); mul != nullptr) {
            if (mul->GetMostSigOp().GetValue() == 1.0) {
                val = mul->GetLeastSigOp().Generalize();
            }
        }
    }

    // filter out zero-equivalent expressions
    std::vector<std::unique_ptr<Expression>> avals;
    for (auto& val : vals) {
        if (auto real = RecursiveCast<Real>(*val); real != nullptr) {
            if (std::abs(real->GetValue()) <= EPSILON) {
                continue;
            }
        }
        if (auto mul = RecursiveCast<Multiply<Real, Expression>>(*val); mul != nullptr) {
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

auto Add<Expression>::Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression>
{
    // Single integration variable
    SimplifyVisitor simplifyVisitor;
    if (auto variable = RecursiveCast<Variable>(integrationVariable); variable != nullptr) {
        auto simplified = this->Accept(simplifyVisitor);

        if (!simplified) {
            return this->Generalize();
        }

        auto simplifiedAdd = std::move(simplified).value();

        // Make sure we're still adder
        if (auto adder = RecursiveCast<Add<Expression>>(*simplifiedAdd); adder != nullptr) {
            auto leftRef = adder->GetLeastSigOp().Copy();
            auto leftIntegral = leftRef->Integrate(integrationVariable);

            auto specializedLeft = RecursiveCast<Add<Expression>>(*leftIntegral);

            auto rightRef = adder->GetMostSigOp().Copy();
            auto rightIntegral = rightRef->Integrate(integrationVariable);

            auto specializedRight = RecursiveCast<Add<Expression>>(*rightIntegral);
            if (specializedLeft == nullptr || specializedRight == nullptr) {
                return Copy();
            }
            Add<Expression> add {
                Add<Expression, Expression> {
                    *(specializedLeft->GetMostSigOp().Copy()), *(specializedRight->GetMostSigOp().Copy()) },
                Variable { "C" }
            };
            auto postSimplify = add.Accept(simplifyVisitor);
            if (!postSimplify) {
                return add.Generalize();
            }
            return std::move(postSimplify).value();
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
    if (auto variable = RecursiveCast<Variable>(differentiationVariable); variable != nullptr) {
        auto left = mostSigOp->Differentiate(differentiationVariable);
        auto right = leastSigOp->Differentiate(differentiationVariable);
        SimplifyVisitor simplifyVisitor;
        auto simplified = Add<Expression> { *left, *right }.Accept(simplifyVisitor);
        if (!simplified) {
            Add<Expression> { *left, *right };
        }
        return std::move(simplified).value();
    }
    return Copy();
}

} // Oasis
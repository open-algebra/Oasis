//
// Created by Matthew McCall on 8/10/23.
//

#include "Oasis/Multiply.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Matrix.hpp"
#include "Oasis/RecursiveCast.hpp"

#define EPSILON 10E-6

namespace Oasis {

auto Multiply<Expression>::Simplify() const -> std::unique_ptr<Expression>
{
    auto simplifiedMultiplicand = mostSigOp->Simplify();
    auto simplifiedMultiplier = leastSigOp->Simplify();

    SimplifyVisitor simplifyVisitor;

    Multiply simplifiedMultiply { *simplifiedMultiplicand, *simplifiedMultiplier };
    if (auto onezerocase = RecursiveCast<Multiply<Real, Expression>>(simplifiedMultiply); onezerocase != nullptr) {
        const Real& multiplicand = onezerocase->GetMostSigOp();
        const Expression& multiplier = onezerocase->GetLeastSigOp();
        if (std::abs(multiplicand.GetValue()) <= EPSILON) {
            return std::make_unique<Real>(Real { 0.0 });
        }
        if (multiplicand.GetValue() == 1) {
            return multiplier.Simplify();
        }
    }
    if (auto realCase = RecursiveCast<Multiply<Real>>(simplifiedMultiply); realCase != nullptr) {
        const Real& multiplicand = realCase->GetMostSigOp();
        const Real& multiplier = realCase->GetLeastSigOp();
        return std::make_unique<Real>(multiplicand.GetValue() * multiplier.GetValue());
    }

    if (auto ImgCase = RecursiveCast<Multiply<Imaginary>>(simplifiedMultiply); ImgCase != nullptr) {
        return std::make_unique<Real>(-1.0);
    }

    if (auto multCase = RecursiveCast<Multiply<Real, Divide<Expression>>>(simplifiedMultiply); multCase != nullptr) {
        auto e = Multiply<Expression> { multCase->GetMostSigOp(), multCase->GetLeastSigOp().GetMostSigOp() };
        auto m = e.Accept(simplifyVisitor);
        if (!m) {
            return Divide<Expression> { e, (multCase->GetLeastSigOp().GetLeastSigOp()) }.Generalize();
        }
        return Divide<Expression> { *std::move(m).value(), (multCase->GetLeastSigOp().GetLeastSigOp()) }.Generalize();
    }

    if (auto exprCase = RecursiveCast<Multiply<Expression>>(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().Equals(exprCase->GetLeastSigOp())) {
            return std::make_unique<Exponent<Expression, Expression>>(exprCase->GetMostSigOp(), Real { 2.0 });
        }
    }

    if (auto rMatrixCase = RecursiveCast<Multiply<Real, Matrix>>(simplifiedMultiply); rMatrixCase != nullptr) {
        return std::make_unique<Matrix>(rMatrixCase->GetLeastSigOp().GetMatrix() * rMatrixCase->GetMostSigOp().GetValue());
    }

    if (auto matrixCase = RecursiveCast<Multiply<Matrix, Matrix>>(simplifiedMultiply); matrixCase != nullptr) {
        const Oasis::IExpression auto& leftTerm = matrixCase->GetMostSigOp();
        const Oasis::IExpression auto& rightTerm = matrixCase->GetLeastSigOp();

        if (leftTerm.GetCols() == rightTerm.GetRows()) {
            return std::make_unique<Matrix>(leftTerm.GetMatrix() * rightTerm.GetMatrix());
        } else {
            // ERROR: INVALID DIMENSION
            return std::make_unique<Multiply<Expression>>(leftTerm, rightTerm);
        }
    }

    //    Commented out to not cause massive problems with things that need factored expressions
    //    // c*(a-b)
    //    if (auto negated = RecursiveCast<Multiply<Real, Subtract<Expression>>>(simplifiedMultiply); negated != nullptr) {
    //        if (negated->GetMostSigOp().GetValue()<0){
    //            return Add{Multiply{negated->GetMostSigOp(), negated->GetLeastSigOp().GetMostSigOp()},
    //                       Multiply{negated->GetMostSigOp(), negated->GetLeastSigOp().GetLeastSigOp()}}.Simplify();
    //        } else if (negated->GetMostSigOp().GetValue()>0){
    //            return Subtract{Multiply{negated->GetMostSigOp(), negated->GetLeastSigOp().GetMostSigOp()},
    //                       Multiply{negated->GetMostSigOp(), negated->GetLeastSigOp().GetLeastSigOp()}}.Simplify();
    //        } else {
    //            return Real{0}.Copy();
    //        }
    //
    //    }
    //
    //    // c*(a+b)
    //    if (auto negated = RecursiveCast<Multiply<Real, Add<Expression>>>(simplifiedMultiply); negated != nullptr) {
    //        return Add{Multiply{negated->GetMostSigOp(), negated->GetLeastSigOp().GetMostSigOp()},
    //                   Multiply{negated->GetMostSigOp(), negated->GetLeastSigOp().GetLeastSigOp()}}.Simplify();
    //    }

    if (auto multCase = RecursiveCast<Multiply<Expression, Divide<Expression>>>(simplifiedMultiply); multCase != nullptr) {
        auto m = Multiply<Expression> { multCase->GetMostSigOp(), multCase->GetLeastSigOp().GetMostSigOp() };
        auto ms = m.Accept(simplifyVisitor);
        if (!ms) {
            return Divide { m, (multCase->GetLeastSigOp().GetLeastSigOp()) }.Generalize();
        }
        // This doesn't have to check errors because errors should have already been caught above
        return Divide<Expression> { *std::move(ms).value(), (multCase->GetLeastSigOp().GetLeastSigOp()) }.Accept(simplifyVisitor).value();
    }

    if (auto multCase = RecursiveCast<Multiply<Divide<Expression>, Divide<Expression>>>(simplifiedMultiply); multCase != nullptr) {
        auto m = Multiply<Expression> { multCase->GetMostSigOp(), multCase->GetLeastSigOp().GetMostSigOp() };
        auto ms = m.Accept(simplifyVisitor);
        auto n = Multiply<Expression> { multCase->GetMostSigOp().GetLeastSigOp(), multCase->GetLeastSigOp().GetLeastSigOp() };
        auto ns = n.Accept(simplifyVisitor);
        if (!ms || !ns) {
            return this->Generalize();
        }
        return Divide<Expression> { *std::move(ms).value(), *std::move(ns).value() }.Accept(simplifyVisitor).value();
    }

    if (auto exprCase = RecursiveCast<Multiply<Expression, Exponent<Expression, Expression>>>(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetMostSigOp())) {
            auto e = Add<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } };
            auto s = e.Accept(simplifyVisitor);
            if (!s) {
                return std::make_unique<Exponent<Expression>>(exprCase->GetMostSigOp(), e);
            }
            return std::make_unique<Exponent<Expression>>(exprCase->GetMostSigOp(),
                *(std::move(s).value()));
        }
    }

    // x*x^n
    if (auto exprCase = RecursiveCast<Multiply<Expression, Exponent<Expression>>>(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetMostSigOp())) {
            auto e = Add<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } };
            auto s = e.Accept(simplifyVisitor);
            if (!s) {
                return std::make_unique<Exponent<Expression>>(exprCase->GetMostSigOp(), e);
            }
            return std::make_unique<Exponent<Expression>>(exprCase->GetMostSigOp(), *(std::move(s).value()));
        }
    }

    if (auto exprCase = RecursiveCast<Multiply<Exponent<Expression>, Expression>>(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetLeastSigOp().Equals(exprCase->GetMostSigOp().GetMostSigOp())) {
            auto e = Add<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(), Real { 1.0 } };
            auto s = e.Accept(simplifyVisitor);
            if (!s) {
                return std::make_unique<Exponent<Expression>>(exprCase->GetLeastSigOp(), e);
            }
            return std::make_unique<Exponent<Expression>>(exprCase->GetLeastSigOp(), *(std::move(s).value()));
        }
    }

    // x^n*x^m
    if (auto exprCase = RecursiveCast<Multiply<Exponent<Expression>, Exponent<Expression>>>(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetMostSigOp())) {
            auto e = Add<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(), exprCase->GetLeastSigOp().GetLeastSigOp() };
            auto s = e.Accept(simplifyVisitor);
            if (!s) {
                return std::make_unique<Exponent<Expression>>(exprCase->GetMostSigOp().GetMostSigOp(), e);
            }
            return std::make_unique<Exponent<Expression>>(exprCase->GetMostSigOp().GetMostSigOp(), *(std::move(s).value()));
        }
    }

    // a*x*x
    if (auto exprCase = RecursiveCast<Multiply<Multiply<Expression>, Expression>>(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().Equals(exprCase->GetLeastSigOp())) {
            return std::make_unique<Multiply<Expression, Expression>>(exprCase->GetMostSigOp().GetMostSigOp(),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(), Real { 2.0 } });
        }
    }

    // a*x*b*x
    if (auto exprCase = RecursiveCast<Multiply<Multiply<Expression>, Multiply<Expression>>>(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().Equals(exprCase->GetLeastSigOp().GetLeastSigOp())) {
            auto m = Multiply<Expression> { exprCase->GetMostSigOp().GetMostSigOp(), exprCase->GetLeastSigOp().GetMostSigOp() };
            auto ms = m.Accept(simplifyVisitor);
            auto n = Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(), Real { 2.0 } };
            auto ns = n.Accept(simplifyVisitor);
            if (!ms || !ns) {
                return this->Generalize();
            }
            return std::make_unique<Multiply<Expression>>(
                *(std::move(ms).value()),
                *(std::move(ns).value()));
        }
    }

    // a*x^n*x
    if (auto exprCase = RecursiveCast<Multiply<Multiply<Expression, Exponent<Expression>>, Expression>>(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exprCase->GetLeastSigOp())) {
            auto e = Add<Expression> { exprCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } };
            auto s = e.Accept(simplifyVisitor);
            if (!s) {
                return std::make_unique<Multiply<Expression>>(exprCase->GetMostSigOp().GetMostSigOp(),
                    Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp(), e });
            }
            return std::make_unique<Multiply<Expression>>(exprCase->GetMostSigOp().GetMostSigOp(),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp(),
                    *(std::move(s).value()) });
        }
    }

    // a*x*x^n
    if (auto exprCase = RecursiveCast<Multiply<Multiply<Expression>, Exponent<Expression>>>(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().Equals(exprCase->GetLeastSigOp().GetMostSigOp())) {
            auto e = Add<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } };
            auto s = e.Accept(simplifyVisitor);
            if (!s) {
                return std::make_unique<Multiply<Expression>>(exprCase->GetMostSigOp().GetMostSigOp(),
                    Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(), e });
            }
            return std::make_unique<Multiply<Expression>>(exprCase->GetMostSigOp().GetMostSigOp(),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(),
                    *(std::move(s).value()) });
        }
        if (exprCase->GetMostSigOp().GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetMostSigOp())) {
            auto e = Add<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } };
            auto s = e.Accept(simplifyVisitor);
            if (!s) {
                return std::make_unique<Multiply<Expression>>(exprCase->GetMostSigOp().GetLeastSigOp(),
                    Exponent<Expression> { exprCase->GetMostSigOp().GetMostSigOp(), e });
            }
            return std::make_unique<Multiply<Expression>>(exprCase->GetMostSigOp().GetLeastSigOp(),
                Exponent<Expression> { exprCase->GetMostSigOp().GetMostSigOp(),
                    *(std::move(s).value()) });
        }
    }

    // a*x^n*b*x
    if (auto exprCase = RecursiveCast<Multiply<Multiply<Expression>, Multiply<Expression, Exponent<Expression>>>>(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().Equals(exprCase->GetLeastSigOp().GetLeastSigOp().GetMostSigOp())) {
            auto m = Multiply<Expression> { exprCase->GetMostSigOp().GetMostSigOp(), exprCase->GetLeastSigOp().GetMostSigOp() };
            auto ms = m.Accept(simplifyVisitor);
            auto n = Add<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } };
            auto ns = n.Accept(simplifyVisitor);
            if (!ms || !ns) {
                return std::make_unique<Multiply<Expression>>(
                    m, Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(), n });
            }
            return std::make_unique<Multiply<Expression>>(
                *(std::move(ms).value()),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(),
                    *(std::move(ns).value()) });
        }
    }

    if (auto exprCase = RecursiveCast<Multiply<Multiply<Expression>, Multiply<Exponent<Expression>, Expression>>>(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().Equals(exprCase->GetLeastSigOp().GetMostSigOp().GetMostSigOp())) {
            auto m = Multiply<Expression> { exprCase->GetMostSigOp().GetMostSigOp(), exprCase->GetLeastSigOp().GetLeastSigOp() };
            auto ms = m.Accept(simplifyVisitor);
            auto n = Add<Expression> { exprCase->GetLeastSigOp().GetMostSigOp().GetLeastSigOp(), Real { 1.0 } };
            auto ns = n.Accept(simplifyVisitor);
            if (!ms || !ns) {
                return std::make_unique<Multiply<Expression>>(m,
                    Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(),
                        n });
            }
            return std::make_unique<Multiply<Expression>>(
                *(std::move(ms).value()),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(),
                    *(std::move(ns).value()) });
        }
    }

    if (auto exprCase = RecursiveCast<Multiply<Multiply<Expression, Exponent<Expression>>, Multiply<Expression>>>(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetLeastSigOp())) {
            auto m = Multiply<Expression> { exprCase->GetMostSigOp().GetMostSigOp(), exprCase->GetLeastSigOp().GetLeastSigOp() };
            auto ms = m.Accept(simplifyVisitor);
            auto n = Add<Expression> { exprCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } };
            auto ns = n.Accept(simplifyVisitor);
            if (!ms || !ns) {
                return std::make_unique<Multiply<Expression>>(m,
                    Exponent<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp(),
                        n });
            }

            return std::make_unique<Multiply<Expression>>(
                *(std::move(ms).value()),
                Exponent<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp(),
                    *(std::move(ns).value()) });
        }
    }

    // a*x^n*x^m
    if (auto exprCase = RecursiveCast<Multiply<Multiply<Expression, Exponent<Expression>>, Exponent<Expression>>>(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetLeastSigOp())) {
            auto e = Add<Expression> { exprCase->GetLeastSigOp().GetMostSigOp(), exprCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp() };
            auto s = e.Accept(simplifyVisitor);
            if (!s) {
                return std::make_unique<Multiply<Expression>>(
                    exprCase->GetMostSigOp().GetMostSigOp(),
                    Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp(),
                        e });
            }
            return std::make_unique<Multiply<Expression>>(
                exprCase->GetMostSigOp().GetMostSigOp(),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp(),
                    *(std::move(s).value()) });
        }
    }

    // a*x^n*b*x^m
    if (auto exprCase = RecursiveCast<Multiply<Multiply<Expression, Exponent<Expression>>, Multiply<Expression, Exponent<Expression>>>>(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetLeastSigOp().GetMostSigOp())) {
            auto m = Multiply<Expression> { exprCase->GetMostSigOp().GetMostSigOp(), exprCase->GetLeastSigOp().GetMostSigOp() };
            auto ms = m.Accept(simplifyVisitor);
            auto n = Add<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp().GetLeastSigOp(), exprCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp() };
            auto ns = n.Accept(simplifyVisitor);
            if (!ms || !ns) {
                return std::make_unique<Multiply<Expression>>(
                    m,
                    Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp(),
                        n });
            }
            return std::make_unique<Multiply<Expression>>(
                *(std::move(ms).value()),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp(),
                    *(std::move(ns).value()) });
        }
    }

    //    if (auto negate = RecursiveCast<Multiply<Real, Negate<Subtract<Expression>>>>(simplifiedMultiply); negate != nullptr){
    //        return Add{Multiply{negate->GetMostSigOp(), negate->GetLeastSigOp().GetOperand().GetMostSigOp()},
    //                   Multiply{negate->GetMostSigOp(), negate->GetLeastSigOp().GetOperand().GetLeastSigOp()}}.Simplify();
    //    }

    // multiply add like terms
    std::vector<std::unique_ptr<Expression>> multiplies;
    std::vector<std::unique_ptr<Expression>> vals;
    simplifiedMultiply.Flatten(multiplies);
    for (const auto& multiplicand : multiplies) {
        size_t i = 0;
        if (auto real = RecursiveCast<Real>(*multiplicand); real != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = RecursiveCast<Real>(*vals[i]); valI != nullptr) {
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
        if (auto img = RecursiveCast<Imaginary>(*multiplicand); img != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = RecursiveCast<Exponent<Imaginary, Expression>>(*vals[i]); valI != nullptr) {
                    auto e = Add<Expression> { valI->GetLeastSigOp(), Real { 1.0 } };
                    auto s = e.Accept(simplifyVisitor);
                    if (!s) {
                        vals[i] = Exponent<Expression> { Imaginary {}, e }.Generalize();
                    } else {
                        vals[i] = Exponent<Expression> { Imaginary {}, *(std::move(s).value()) }.Generalize();
                    }

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
        if (auto img = RecursiveCast<Exponent<Imaginary, Expression>>(*multiplicand); img != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = RecursiveCast<Exponent<Imaginary, Expression>>(*vals[i]); valI != nullptr) {
                    auto e = Add<Expression> { valI->GetLeastSigOp(), img->GetLeastSigOp() };
                    auto s = e.Accept(simplifyVisitor);
                    if (!s) {
                        vals[i] = Exponent<Expression> { Imaginary {}, e }.Generalize();
                    } else {
                        vals[i] = Exponent<Expression> { Imaginary {}, *(std::move(s).value()) }.Generalize();
                    }
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
        if (auto expr = RecursiveCast<Exponent<Expression, Expression>>(*multiplicand); expr != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = RecursiveCast<Exponent<Expression, Expression>>(*vals[i]); valI != nullptr) {
                    if (valI->GetMostSigOp().Equals(expr->GetMostSigOp())) {
                        auto e = Add<Expression> { valI->GetLeastSigOp(), expr->GetLeastSigOp() };
                        auto s = e.Accept(simplifyVisitor);
                        if (!s) {
                            vals[i] = Exponent<Expression> { valI->GetMostSigOp(), e }.Generalize();
                        } else {
                            vals[i] = Exponent<Expression> { valI->GetMostSigOp(), *(std::move(s).value()) }.Generalize();
                        }
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
        if (auto expr = RecursiveCast<Expression>(*multiplicand); expr != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = RecursiveCast<Exponent<Expression, Expression>>(*vals[i]); valI != nullptr) {
                    if (valI->GetMostSigOp().Equals(*expr)) {
                        auto e = Add<Expression> { valI->GetLeastSigOp(), Real { 1.0 } };
                        auto s = e.Accept(simplifyVisitor);
                        if (!s) {
                            vals[i] = Exponent<Expression> { valI->GetMostSigOp(), e }.Generalize();
                        } else {
                            vals[i] = Exponent<Expression> { valI->GetMostSigOp(), *(std::move(s).value()) }.Generalize();
                        }
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
        if (auto exp = RecursiveCast<Exponent<Expression, Real>>(*val); exp != nullptr) {
            if (exp->GetLeastSigOp().GetValue() == 1.0) {
                val = exp->GetMostSigOp().Generalize();
            }
        }
        if (auto mul = RecursiveCast<Multiply<Real, Expression>>(*val); mul != nullptr) {
            if (mul->GetMostSigOp().GetValue() == 1.0) {
                val = mul->GetLeastSigOp().Generalize();
            }
        }
    }

    return BuildFromVector<Multiply>(vals);

    // return simplifiedMultiply.Copy();
}

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

                return adder.Simplify();
            }
        }

        // TODO: Implement integration by parts
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
                return std::make_unique<Multiply<Real, Expression>>(Multiply<Real, Expression> { Real { num.GetValue() }, *(add->Simplify()) })->Simplify();
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
                    ->Simplify();
            }
        }
    }

    return Copy();
}

} // Oasis
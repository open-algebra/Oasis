//
// Created by Andrew Nazareth on 9/23/25.
//

#include <format>

#include "Oasis/SimplifyVisitor.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Derivative.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/EulerNumber.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Magnitude.hpp"
#include "Oasis/MatchCast.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Negate.hpp"
#include "Oasis/Pi.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Sine.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Undefined.hpp"
#include "Oasis/Variable.hpp"

namespace {
constexpr auto EPSILON = std::numeric_limits<float>::epsilon();
}

namespace Oasis {

SimplifyVisitor::SimplifyVisitor()
{
    options = SimplifyOpts {};
}

SimplifyVisitor::SimplifyVisitor(SimplifyOpts& opts)
    : options(opts)
{
}

SimplifyOpts SimplifyVisitor::GetOptions() const
{
    return options;
}

auto SimplifyVisitor::TypedVisit(const Real& real) -> RetT
{
    return gsl::not_null { std::make_unique<Real>(real) };
}

auto SimplifyVisitor::TypedVisit(const Imaginary&) -> RetT
{
    return gsl::not_null { std::make_unique<Imaginary>() };
}

auto SimplifyVisitor::TypedVisit(const Variable& variable) -> RetT
{
    return gsl::not_null { std::make_unique<Variable>(variable) };
}

auto SimplifyVisitor::TypedVisit(const Undefined&) -> RetT
{
    return gsl::not_null { std::make_unique<Undefined>() };
}

auto SimplifyVisitor::TypedVisit(const Add<>& add) -> RetT
{
    auto mostSigOp = add.GetMostSigOp().Copy();
    auto leastSigOp = add.GetLeastSigOp().Copy();
    if (!mostSigOp) {
        return std::unexpected { "Missing most significant operand." };
    }
    if (!leastSigOp) {
        return std::unexpected { "Missing least significant operand." };
    }

    auto simplifiedAugendResult = mostSigOp->Accept(*this);
    auto simplifiedAddendResult = leastSigOp->Accept(*this);

    if (!simplifiedAugendResult) {
        return std::unexpected { simplifiedAugendResult.error() };
    }
    if (!simplifiedAddendResult) {
        return std::unexpected { simplifiedAddendResult.error() };
    }

    auto simplifiedAugend = std::move(simplifiedAugendResult).value();
    auto simplifiedAddend = std::move(simplifiedAddendResult).value();

    Add simplifiedAdd { *simplifiedAugend, *simplifiedAddend };

    if (auto realCase = RecursiveCast<Add<Real>>(simplifiedAdd); realCase != nullptr) {
        const Real& firstReal = realCase->GetMostSigOp();
        const Real& secondReal = realCase->GetLeastSigOp();

        return gsl::not_null { std::make_unique<Real>(firstReal.GetValue() + secondReal.GetValue()) };
    }

    if (auto zeroCase = RecursiveCast<Add<Real, Expression>>(simplifiedAdd); zeroCase != nullptr) {
        if (zeroCase->GetMostSigOp().GetValue() == 0) {
            return gsl::not_null { zeroCase->GetLeastSigOp().Generalize() };
        }
    }

    if (auto likeTermsCase = RecursiveCast<Add<Multiply<Real, Expression>>>(simplifiedAdd); likeTermsCase != nullptr) {
        const Oasis::IExpression auto& leftTerm = likeTermsCase->GetMostSigOp().GetLeastSigOp();
        const Oasis::IExpression auto& rightTerm = likeTermsCase->GetLeastSigOp().GetLeastSigOp();

        if (leftTerm.Equals(rightTerm)) {
            const Real& coefficient1 = likeTermsCase->GetMostSigOp().GetMostSigOp();
            const Real& coefficient2 = likeTermsCase->GetLeastSigOp().GetMostSigOp();

            return gsl::not_null { std::make_unique<Multiply<Expression>>(Real(coefficient1.GetValue() + coefficient2.GetValue()), leftTerm) };
        }
    }

    // matrix + matrix
    if (auto matrixCase = RecursiveCast<Add<Matrix, Matrix>>(simplifiedAdd); matrixCase != nullptr) {
        const Oasis::IExpression auto& leftTerm = matrixCase->GetMostSigOp();
        const Oasis::IExpression auto& rightTerm = matrixCase->GetLeastSigOp();

        if ((leftTerm.GetRows() == rightTerm.GetRows()) && (leftTerm.GetCols() == rightTerm.GetCols())) {
            return gsl::not_null { std::make_unique<Matrix>(leftTerm.GetMatrix() + rightTerm.GetMatrix()) };
        } else {
            return gsl::not_null { std::make_unique<Add<Expression>>(leftTerm, rightTerm) };
        }
    }

    // log(a) + log(b) = log(ab)
    if (auto logCase = RecursiveCast<Add<Log<Expression, Expression>, Log<Expression, Expression>>>(simplifiedAdd); logCase != nullptr) {
        if (logCase->GetMostSigOp().GetMostSigOp().Equals(logCase->GetLeastSigOp().GetMostSigOp())) {
            const IExpression auto& base = logCase->GetMostSigOp().GetMostSigOp();
            const IExpression auto& argument = Multiply<Expression>({ logCase->GetMostSigOp().GetLeastSigOp(), logCase->GetLeastSigOp().GetLeastSigOp() });
            return gsl::not_null { std::make_unique<Log<Expression>>(base, argument) };
        }
    }

    // x + x = 2x
    if (simplifiedAugend->Equals(*simplifiedAddend)) {
        return Multiply<Real, Expression> { Real { 2.0 }, *simplifiedAugend }.Accept(*this);
    }

    // 2x + x = 3x
    if (const auto likeTermsCase2 = RecursiveCast<Add<Multiply<Real, Expression>, Expression>>(simplifiedAdd); likeTermsCase2 != nullptr) {
        if (likeTermsCase2->GetMostSigOp().GetLeastSigOp().Equals(likeTermsCase2->GetLeastSigOp())) {
            const Real& coeffiecent = likeTermsCase2->GetMostSigOp().GetMostSigOp();
            return gsl::not_null { std::make_unique<Multiply<Real, Expression>>(Real { coeffiecent.GetValue() + 1 }, likeTermsCase2->GetMostSigOp().GetLeastSigOp()) };
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
                    auto addResult = Add<Expression> { valI->GetMostSigOp(), Real { 1.0 } }.Accept(*this);
                    if (!addResult) {
                        return addResult;
                    }
                    vals[i] = Multiply<Expression> { *(addResult.value()), Imaginary {} }.Generalize();
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
        if (auto img = RecursiveCast<Multiply<Expression, Imaginary>>(*addend); img != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = RecursiveCast<Multiply<Expression, Imaginary>>(*vals[i]); valI != nullptr) {
                    auto addResult = Add<Expression> { valI->GetMostSigOp(), img->GetMostSigOp() }.Accept(*this);
                    if (!addResult) {
                        return addResult;
                    }
                    vals[i] = Multiply<Expression> { *(addResult.value()), Imaginary {} }.Generalize();
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
        if (auto var = RecursiveCast<Variable>(*addend); var != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = RecursiveCast<Multiply<Expression, Variable>>(*vals[i]); valI != nullptr) {
                    if (valI->GetLeastSigOp().GetName() == var->GetName()) {
                        auto addResult = Add<Expression> { valI->GetMostSigOp(), Real { 1.0 } }.Accept(*this);
                        if (!addResult) {
                            return addResult;
                        }
                        vals[i] = Multiply<Expression> { *(addResult.value()), *var }.Generalize();
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
        if (auto var = RecursiveCast<Multiply<Expression, Variable>>(*addend); var != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = RecursiveCast<Multiply<Expression, Variable>>(*vals[i]); valI != nullptr) {
                    // if (auto zeroCase = RecursiveCast<Multiply<Real, Expression>>(*valI); zeroCase != nullptr) {}
                    if (valI->GetLeastSigOp().GetName() == var->GetLeastSigOp().GetName()) {
                        auto addResult = Add<Expression> { valI->GetMostSigOp(), var->GetMostSigOp() }.Accept(*this);
                        if (!addResult) {
                            return addResult;
                        }
                        vals[i] = Multiply<Expression> { *(addResult.value()), valI->GetLeastSigOp() }.Generalize();
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
        if (auto exp = RecursiveCast<Exponent<Expression>>(*addend); exp != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = RecursiveCast<Multiply<Expression, Exponent<Expression>>>(*vals[i]); valI != nullptr) {
                    if (valI->GetLeastSigOp().Equals(*exp)) {
                        auto addResult = Add<Expression> { valI->GetMostSigOp(), Real { 1.0 } }.Accept(*this);
                        if (!addResult) {
                            return addResult;
                        }
                        vals[i] = Multiply<Expression> { *(addResult.value()), *exp }.Generalize();
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
        if (auto exp = RecursiveCast<Multiply<Expression, Exponent<Expression>>>(*addend); exp != nullptr) {
            for (; i < vals.size(); i++) {
                if (auto valI = RecursiveCast<Multiply<Expression, Exponent<Expression>>>(*vals[i]); valI != nullptr) {
                    if (valI->GetLeastSigOp().Equals(exp->GetLeastSigOp())) {
                        auto addResult = Add<Expression> { valI->GetMostSigOp(), exp->GetMostSigOp() }.Accept(*this);
                        if (!addResult) {
                            return addResult;
                        }
                        vals[i] = Multiply<Expression> { *(addResult.value()), valI->GetLeastSigOp() }.Generalize();
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
        return gsl::not_null { std::move(vec) };
    }

    return gsl::not_null { simplifiedAdd.Copy() };
}

auto SimplifyVisitor::TypedVisit(const Subtract<>& subtract) -> RetT
{
    auto mostSigOp = subtract.GetMostSigOp().Copy();
    auto leastSigOp = subtract.GetLeastSigOp().Copy();
    if (!mostSigOp) {
        return std::unexpected { "Missing most significant operand." };
    }
    if (!leastSigOp) {
        return std::unexpected { "Missing least significant operand." };
    }

    auto simplifiedMostSigOpResult = mostSigOp->Accept(*this);
    auto simplifiedLeastSigOpResult = leastSigOp->Accept(*this);

    if (!simplifiedMostSigOpResult) {
        return std::unexpected { simplifiedMostSigOpResult.error() };
    }
    if (!simplifiedLeastSigOpResult) {
        return std::unexpected { simplifiedLeastSigOpResult.error() };
    }

    auto simplifiedMinuend = std::move(simplifiedMostSigOpResult).value();
    auto simplifiedSubtrahend = std::move(simplifiedLeastSigOpResult).value();

    const Subtract simplifiedSubtract { *simplifiedMinuend, *simplifiedSubtrahend };

    // 2 - 1 = 1
    if (auto realCase = RecursiveCast<Subtract<Real>>(simplifiedSubtract); realCase != nullptr) {
        const Real& minuend = realCase->GetMostSigOp();
        const Real& subtrahend = realCase->GetLeastSigOp();

        return gsl::not_null { std::make_unique<Real>(minuend.GetValue() - subtrahend.GetValue()) };
    }

    // x - x = 0
    if (simplifiedMinuend->Equals(*simplifiedSubtrahend)) {
        return gsl::not_null { std::make_unique<Real>(Real { 0.0 }) };
    }

    if (auto matrixCase = RecursiveCast<Subtract<Matrix, Matrix>>(simplifiedSubtract); matrixCase != nullptr) {
        const Oasis::IExpression auto& leftTerm = matrixCase->GetMostSigOp();
        const Oasis::IExpression auto& rightTerm = matrixCase->GetLeastSigOp();

        if ((leftTerm.GetRows() == rightTerm.GetRows()) && (leftTerm.GetCols() == leftTerm.GetCols())) {
            return gsl::not_null { std::make_unique<Matrix>(leftTerm.GetMatrix() - rightTerm.GetMatrix()) };
        } else {
            return gsl::not_null { std::make_unique<Subtract<Expression>>(leftTerm, rightTerm) };
        }
    }

    // ax - x = (a-1)x
    if (const auto minusOneCase = RecursiveCast<Subtract<Multiply<>, Expression>>(simplifiedSubtract); minusOneCase != nullptr) {
        if (minusOneCase->GetMostSigOp().GetLeastSigOp().Equals(minusOneCase->GetLeastSigOp())) {
            const Subtract newCoefficient { minusOneCase->GetMostSigOp().GetMostSigOp(), Real { 1.0 } };
            return Multiply { newCoefficient, minusOneCase->GetLeastSigOp() }.Accept(*this);
        }
    }

    // x-ax = (1-a)x
    if (const auto oneMinusCase = RecursiveCast<Subtract<Expression, Multiply<>>>(simplifiedSubtract); oneMinusCase != nullptr) {
        if (oneMinusCase->GetMostSigOp().Equals(oneMinusCase->GetLeastSigOp().GetLeastSigOp())) {
            const Subtract newCoefficient { Real { 1.0 }, oneMinusCase->GetLeastSigOp().GetMostSigOp() };
            return Multiply { newCoefficient, oneMinusCase->GetMostSigOp() }.Accept(*this);
        }
    }

    // ax-bx= (a-b)x
    if (const auto coefficientCase = RecursiveCast<Subtract<Multiply<>>>(simplifiedSubtract); coefficientCase != nullptr) {
        if (coefficientCase->GetMostSigOp().GetLeastSigOp().Equals(coefficientCase->GetLeastSigOp().GetLeastSigOp())) {
            const Subtract newCoefficient { coefficientCase->GetMostSigOp().GetMostSigOp(), coefficientCase->GetLeastSigOp().GetMostSigOp() };
            return Multiply { newCoefficient, coefficientCase->GetLeastSigOp().GetLeastSigOp() }.Accept(*this);
        }
    }

    // log(a) - log(b) = log(a / b)
    if (const auto logCase = RecursiveCast<Subtract<Log<>>>(simplifiedSubtract); logCase != nullptr) {
        if (logCase->GetMostSigOp().GetMostSigOp().Equals(logCase->GetLeastSigOp().GetMostSigOp())) {
            const IExpression auto& base = logCase->GetMostSigOp().GetMostSigOp();
            const IExpression auto& argument = Divide({ logCase->GetMostSigOp().GetLeastSigOp(), logCase->GetLeastSigOp().GetLeastSigOp() });
            return gsl::not_null { std::make_unique<Log<>>(base, argument) };
        }
    }

    // makes subtraction into addition because it is easier to deal with
    auto negated = Multiply<Expression> { Real { -1 }, *simplifiedSubtrahend };
    if (auto added = RecursiveCast<Add<Expression>>(negated.GetLeastSigOp()); added != nullptr) {
        auto msOp = Multiply<Expression> { Real { -1.0 }, added->GetMostSigOp() }.Accept(*this);
        if (!msOp) {
            return msOp;
        }
        auto lsOp = Multiply<Expression> { Real { -1.0 }, added->GetLeastSigOp() }.Accept(*this);
        if (!lsOp) {
            return lsOp;
        }

        auto RHS = Add { *(msOp.value()), *(lsOp.value()) }.Accept(*this);
        if (!RHS) {
            return RHS;
        }
        return Add { *simplifiedMinuend, *(RHS.value()) }.Accept(*this);
    }
    if (auto subtracted = RecursiveCast<Subtract<Expression>>(negated.GetLeastSigOp()); subtracted != nullptr) {
        auto msOp = Multiply<Expression> { Real { -1.0 }, subtracted->GetMostSigOp() }.Accept(*this);
        if (!msOp) {
            return msOp;
        }
        auto lsOp = subtracted->GetLeastSigOp().Accept(*this);
        if (!lsOp) {
            return lsOp;
        }

        auto RHS = Add { *(msOp.value()), *(lsOp.value()) }.Accept(*this);
        if (!RHS) {
            return RHS;
        }
        return Add { *simplifiedMinuend, *(RHS.value()) }.Accept(*this);
    }
    return Add { *simplifiedMinuend, negated }.Accept(*this);
}

auto SimplifyVisitor::TypedVisit(const Multiply<>& multiply) -> RetT
{
    auto mostSigOp = multiply.GetMostSigOp().Copy();
    auto leastSigOp = multiply.GetLeastSigOp().Copy();
    if (!mostSigOp) {
        return std::unexpected { "Missing most significant operand." };
    }
    if (!leastSigOp) {
        return std::unexpected { "Missing least significant operand." };
    }

    auto simplifiedMostSigOpResult = mostSigOp->Accept(*this);
    auto simplifiedLeastSigOpResult = leastSigOp->Accept(*this);

    if (!simplifiedMostSigOpResult) {
        return std::unexpected { simplifiedMostSigOpResult.error() };
    }
    if (!simplifiedLeastSigOpResult) {
        return std::unexpected { simplifiedLeastSigOpResult.error() };
    }

    auto simplifiedMultiplicand = std::move(simplifiedMostSigOpResult).value();
    auto simplifiedMultiplier = std::move(simplifiedLeastSigOpResult).value();

    Multiply simplifiedMultiply { *simplifiedMultiplicand, *simplifiedMultiplier };
    if (auto onezerocase = RecursiveCast<Multiply<Real, Expression>>(simplifiedMultiply); onezerocase != nullptr) {
        const Real& multiplicand = onezerocase->GetMostSigOp();
        const Expression& multiplier = onezerocase->GetLeastSigOp();
        if (std::abs(multiplicand.GetValue()) <= EPSILON) {
            return gsl::not_null { std::make_unique<Real>(Real { 0.0 }) };
        }
        if (multiplicand.GetValue() == 1) {
            return multiplier.Accept(*this);
        }
    }
    if (auto realCase = RecursiveCast<Multiply<Real>>(simplifiedMultiply); realCase != nullptr) {
        const Real& multiplicand = realCase->GetMostSigOp();
        const Real& multiplier = realCase->GetLeastSigOp();
        return gsl::not_null { std::make_unique<Real>(multiplicand.GetValue() * multiplier.GetValue()) };
    }

    if (auto ImgCase = RecursiveCast<Multiply<Imaginary>>(simplifiedMultiply); ImgCase != nullptr) {
        return gsl::not_null { std::make_unique<Real>(-1.0) };
    }

    if (auto multCase = RecursiveCast<Multiply<Real, Divide<Expression>>>(simplifiedMultiply); multCase != nullptr) {
        auto m = Multiply<Expression> { multCase->GetMostSigOp(), multCase->GetLeastSigOp().GetMostSigOp() }.Accept(*this);
        if (!m) {
            return m;
        }
        return gsl::not_null { Divide<Expression> { *(m.value()), (multCase->GetLeastSigOp().GetLeastSigOp()) }.Generalize() };
    }

    if (auto exprCase = RecursiveCast<Multiply<Expression>>(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().Equals(exprCase->GetLeastSigOp())) {
            return gsl::not_null { std::make_unique<Exponent<Expression, Expression>>(exprCase->GetMostSigOp(), Real { 2.0 }) };
        }
    }

    if (auto rMatrixCase = RecursiveCast<Multiply<Real, Matrix>>(simplifiedMultiply); rMatrixCase != nullptr) {
        return gsl::not_null { std::make_unique<Matrix>(rMatrixCase->GetLeastSigOp().GetMatrix() * rMatrixCase->GetMostSigOp().GetValue()) };
    }

    if (auto matrixCase = RecursiveCast<Multiply<Matrix, Matrix>>(simplifiedMultiply); matrixCase != nullptr) {
        const Oasis::IExpression auto& leftTerm = matrixCase->GetMostSigOp();
        const Oasis::IExpression auto& rightTerm = matrixCase->GetLeastSigOp();

        if (leftTerm.GetCols() == rightTerm.GetRows()) {
            return gsl::not_null { std::make_unique<Matrix>(leftTerm.GetMatrix() * rightTerm.GetMatrix()) };
        } else {
            // ERROR: INVALID DIMENSION
            return gsl::not_null { std::make_unique<Multiply<Expression>>(leftTerm, rightTerm) };
        }
    }

    if (auto multCase = RecursiveCast<Multiply<Expression, Divide<Expression>>>(simplifiedMultiply); multCase != nullptr) {
        auto m = Multiply<Expression> { multCase->GetMostSigOp(), multCase->GetLeastSigOp().GetMostSigOp() }.Accept(*this);
        if (!m) {
            return m;
        }
        return Divide<Expression> { *(m.value()), (multCase->GetLeastSigOp().GetLeastSigOp()) }.Accept(*this);
    }

    if (auto multCase = RecursiveCast<Multiply<Divide<Expression>, Divide<Expression>>>(simplifiedMultiply); multCase != nullptr) {
        auto m = Multiply<Expression> { multCase->GetMostSigOp(), multCase->GetLeastSigOp().GetMostSigOp() }.Accept(*this);
        if (!m) {
            return m;
        }
        auto n = Multiply<Expression> { multCase->GetMostSigOp().GetLeastSigOp(), multCase->GetLeastSigOp().GetLeastSigOp() }.Accept(*this);
        if (!n) {
            return n;
        }
        return Divide<Expression> { *(m.value()), *(n.value()) }.Accept(*this);
    }

    if (auto exprCase = RecursiveCast<Multiply<Expression, Exponent<Expression, Expression>>>(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetMostSigOp())) {
            auto lsOp = Add<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } }.Accept(*this);
            if (!lsOp) {
                return lsOp;
            }
            return gsl::not_null { std::make_unique<Exponent<Expression>>(exprCase->GetMostSigOp(), *(lsOp.value())) };
        }
    }

    // x*x^n
    if (auto exprCase = RecursiveCast<Multiply<Expression, Exponent<Expression>>>(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetMostSigOp())) {
            auto lsOp = Add<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } }.Accept(*this);
            if (!lsOp) {
                return lsOp;
            }
            return gsl::not_null { std::make_unique<Exponent<Expression>>(exprCase->GetMostSigOp(), *(lsOp.value())) };
        }
    }

    if (auto exprCase = RecursiveCast<Multiply<Exponent<Expression>, Expression>>(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetLeastSigOp().Equals(exprCase->GetMostSigOp().GetMostSigOp())) {
            auto lsOp = Add<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(), Real { 1.0 } }.Accept(*this);
            if (!lsOp) {
                return lsOp;
            }
            return gsl::not_null { std::make_unique<Exponent<Expression>>(exprCase->GetLeastSigOp(), *(lsOp.value())) };
        }
    }

    // x^n*x^m
    if (auto exprCase = RecursiveCast<Multiply<Exponent<Expression>, Exponent<Expression>>>(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetMostSigOp())) {
            auto lsOp = Add<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(), exprCase->GetLeastSigOp().GetLeastSigOp() }.Accept(*this);
            if (!lsOp) {
                return lsOp;
            }
            return gsl::not_null { std::make_unique<Exponent<Expression>>(exprCase->GetMostSigOp().GetMostSigOp(), *(lsOp.value())) };
        }
    }

    // a*x*x
    if (auto exprCase = RecursiveCast<Multiply<Multiply<Expression>, Expression>>(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().Equals(exprCase->GetLeastSigOp())) {
            return gsl::not_null { std::make_unique<Multiply<Expression, Expression>>(exprCase->GetMostSigOp().GetMostSigOp(),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(), Real { 2.0 } }) };
        }
    }

    // a*x*b*x
    if (auto exprCase = RecursiveCast<Multiply<Multiply<Expression>, Multiply<Expression>>>(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().Equals(exprCase->GetLeastSigOp().GetLeastSigOp())) {
            auto msOp = Multiply<Expression> { exprCase->GetMostSigOp().GetMostSigOp(), exprCase->GetLeastSigOp().GetMostSigOp() }.Accept(*this);
            if (!msOp) {
                return msOp;
            }
            auto lsOp = Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(), Real { 2.0 } }.Accept(*this);
            if (!lsOp) {
                return lsOp;
            }
            return gsl::not_null { std::make_unique<Multiply<Expression>>(*(msOp.value()), *(lsOp.value())) };
        }
    }

    // a*x^n*x
    if (auto exprCase = RecursiveCast<Multiply<Multiply<Expression, Exponent<Expression>>, Expression>>(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exprCase->GetLeastSigOp())) {
            auto lsOp = Add<Expression> { exprCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } }.Accept(*this);
            if (!lsOp) {
                return lsOp;
            }
            return gsl::not_null { std::make_unique<Multiply<Expression>>(exprCase->GetMostSigOp().GetMostSigOp(),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp(), *(lsOp.value()) }) };
        }
    }

    // a*x*x^n
    if (auto exprCase = RecursiveCast<Multiply<Multiply<Expression>, Exponent<Expression>>>(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().Equals(exprCase->GetLeastSigOp().GetMostSigOp())) {
            auto lsOp = Add<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } }.Accept(*this);
            if (!lsOp) {
                return lsOp;
            }
            return gsl::not_null { std::make_unique<Multiply<Expression>>(exprCase->GetMostSigOp().GetMostSigOp(),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(), *(lsOp.value()) }) };
        }
        if (exprCase->GetMostSigOp().GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetMostSigOp())) {
            auto lsOp = Add<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } }.Accept(*this);
            if (!lsOp) {
                return lsOp;
            }
            return gsl::not_null { std::make_unique<Multiply<Expression>>(exprCase->GetMostSigOp().GetLeastSigOp(),
                Exponent<Expression> { exprCase->GetMostSigOp().GetMostSigOp(), *(lsOp.value()) }) };
        }
    }

    // a*x^n*b*x
    if (auto exprCase = RecursiveCast<Multiply<Multiply<Expression>, Multiply<Expression, Exponent<Expression>>>>(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().Equals(exprCase->GetLeastSigOp().GetLeastSigOp().GetMostSigOp())) {
            auto msOp = Multiply<Expression> { exprCase->GetMostSigOp().GetMostSigOp(), exprCase->GetLeastSigOp().GetMostSigOp() }.Accept(*this);
            if (!msOp) {
                return msOp;
            }
            auto lsLsOp = Add<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } }.Accept(*this);
            if (!lsLsOp) {
                return lsLsOp;
            }
            return gsl::not_null { std::make_unique<Multiply<Expression>>(
                *(msOp.value()), Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(), *(lsLsOp.value()) }) };
        }
    }

    if (auto exprCase = RecursiveCast<Multiply<Multiply<Expression>, Multiply<Exponent<Expression>, Expression>>>(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().Equals(exprCase->GetLeastSigOp().GetMostSigOp().GetMostSigOp())) {
            auto msOp = Multiply<Expression> { exprCase->GetMostSigOp().GetMostSigOp(), exprCase->GetLeastSigOp().GetLeastSigOp() }.Accept(*this);
            if (!msOp) {
                return msOp;
            }
            auto lsLsOp = Add<Expression> { exprCase->GetLeastSigOp().GetMostSigOp().GetLeastSigOp(), Real { 1.0 } }.Accept(*this);
            if (!lsLsOp) {
                return lsLsOp;
            }
            return gsl::not_null { std::make_unique<Multiply<Expression>>(
                *(msOp.value()), Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(), *(lsLsOp.value()) }) };
        }
    }

    if (auto exprCase = RecursiveCast<Multiply<Multiply<Expression, Exponent<Expression>>, Multiply<Expression>>>(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetLeastSigOp())) {
            auto msOp = Multiply<Expression> { exprCase->GetMostSigOp().GetMostSigOp(), exprCase->GetLeastSigOp().GetLeastSigOp() }.Accept(*this);
            if (!msOp) {
                return msOp;
            }
            auto lsLsOp = Add<Expression> { exprCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } }.Accept(*this);
            if (!lsLsOp) {
                return lsLsOp;
            }
            return gsl::not_null { std::make_unique<Multiply<Expression>>(
                *(msOp.value()),
                Exponent<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp(), *(lsLsOp.value()) }) };
        }
    }

    // a*x^n*x^m
    if (auto exprCase = RecursiveCast<Multiply<Multiply<Expression, Exponent<Expression>>, Exponent<Expression>>>(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetLeastSigOp())) {
            auto lsOp = Add<Expression> { exprCase->GetLeastSigOp().GetMostSigOp(), exprCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp() }.Accept(*this);
            if (!lsOp) {
                return lsOp;
            }
            return gsl::not_null { std::make_unique<Multiply<Expression>>(
                exprCase->GetMostSigOp().GetMostSigOp(),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp(),
                    *(lsOp.value()) }) };
        }
    }

    // a*x^n*b*x^m
    if (auto exprCase = RecursiveCast<Multiply<Multiply<Expression, Exponent<Expression>>, Multiply<Expression, Exponent<Expression>>>>(simplifiedMultiply); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetLeastSigOp().GetMostSigOp())) {
            auto msOp = Multiply<Expression> { exprCase->GetMostSigOp().GetMostSigOp(), exprCase->GetLeastSigOp().GetMostSigOp() }.Accept(*this);
            if (!msOp) {
                return msOp;
            }
            auto lsOp = Add<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp().GetLeastSigOp(), exprCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp() }.Accept(*this);
            if (!lsOp) {
                return lsOp;
            }
            return gsl::not_null { std::make_unique<Multiply<Expression>>(
                *(msOp.value()),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp(),
                    *(lsOp.value()) }) };
        }
    }

    // IDK why this is commented out but it was commented out where I sourced it from
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
                    auto lsOp = Add<Expression> { valI->GetLeastSigOp(), Real { 1.0 } }.Accept(*this);
                    if (!lsOp) {
                        return lsOp;
                    }
                    vals[i] = Exponent<Expression> { Imaginary {}, *(lsOp.value()) }.Generalize();
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
                    auto lsOp = Add<Expression> { valI->GetLeastSigOp(), img->GetLeastSigOp() }.Accept(*this);
                    if (!lsOp) {
                        return lsOp;
                    }
                    vals[i] = Exponent<Expression> { Imaginary {}, *(lsOp.value()) }.Generalize();
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
                        auto lsOp = Add<Expression> { valI->GetLeastSigOp(), expr->GetLeastSigOp() }.Accept(*this);
                        if (!lsOp) {
                            return lsOp;
                        }
                        vals[i] = Exponent<Expression> { valI->GetMostSigOp(), *(lsOp.value()) }.Generalize();
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
                    auto lsOp = Add<Expression> { valI->GetLeastSigOp(), Real { 1.0 } }.Accept(*this);
                    if (!lsOp) {
                        return lsOp;
                    }
                    if (valI->GetMostSigOp().Equals(*expr)) {
                        vals[i] = Exponent<Expression> { valI->GetMostSigOp(), *(lsOp.value()) }.Generalize();
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

    return gsl::not_null { BuildFromVector<Multiply>(vals) };
}

auto SimplifyVisitor::TypedVisit(const Divide<>& divide) -> RetT
{
    auto mostSigOp = divide.GetMostSigOp().Copy();
    auto leastSigOp = divide.GetLeastSigOp().Copy();
    if (!mostSigOp) {
        return std::unexpected { "Missing most significant operand." };
    }
    if (!leastSigOp) {
        return std::unexpected { "Missing least significant operand." };
    }

    auto simplifiedMostSigOpResult = mostSigOp->Accept(*this);
    auto simplifiedLeastSigOpResult = leastSigOp->Accept(*this);

    if (!simplifiedMostSigOpResult) {
        return std::unexpected { simplifiedMostSigOpResult.error() };
    }
    if (!simplifiedLeastSigOpResult) {
        return std::unexpected { simplifiedLeastSigOpResult.error() };
    }

    auto simplifiedDividend = std::move(simplifiedMostSigOpResult).value();
    auto simplifiedDivider = std::move(simplifiedLeastSigOpResult).value();

    Divide simplifiedDivide { *simplifiedDividend, *simplifiedDivider };

    if (auto realCase = RecursiveCast<Divide<Real>>(simplifiedDivide); realCase != nullptr) {
        const Real& dividend = realCase->GetMostSigOp();
        const Real& divisor = realCase->GetLeastSigOp();
        return gsl::not_null { std::make_unique<Real>(dividend.GetValue() / divisor.GetValue()) };
    }

    // log(a)/log(b)=log[b](a)
    if (auto logCase = RecursiveCast<Divide<Log<Expression, Expression>, Log<Expression, Expression>>>(simplifiedDivide); logCase != nullptr) {
        if (logCase->GetMostSigOp().GetMostSigOp().Equals(logCase->GetLeastSigOp().GetMostSigOp())) {
            const IExpression auto& base = logCase->GetLeastSigOp().GetLeastSigOp();
            const IExpression auto& argument = logCase->GetMostSigOp().GetLeastSigOp();
            return gsl::not_null { std::make_unique<Log<Expression>>(base, argument) };
        }
    }
    // convert the terms in numerator and denominator into a vector to make manipulations easier
    std::vector<std::unique_ptr<Expression>> numerator;
    std::vector<std::unique_ptr<Expression>> denominator;
    std::vector<std::unique_ptr<Expression>> result;
    std::vector<std::unique_ptr<Expression>> numeratorVals;
    std::vector<std::unique_ptr<Expression>> denominatorVals;

    if (auto multipliedNumerator = RecursiveCast<Multiply<Expression>>(*simplifiedDividend); multipliedNumerator != nullptr) {
        multipliedNumerator->Flatten(numerator);
    } else {
        numerator.push_back(simplifiedDividend->Copy());
    }

    if (auto multipliedDenominator = RecursiveCast<Multiply<Expression>>(*simplifiedDivider); multipliedDenominator != nullptr) {
        multipliedDenominator->Flatten(denominator);
    } else {
        denominator.push_back(simplifiedDivider->Copy());
    }

    // now that we have the terms in a vector, we have to cancel like terms and simplify them
    result.reserve(numerator.size());
    for (const auto& num : numerator) {
        if (auto div = RecursiveCast<Divide<Expression>>(*num); div != nullptr) {
            std::vector<std::unique_ptr<Expression>> flat_n;
            std::vector<std::unique_ptr<Expression>> flat_d;
            if (auto mN = RecursiveCast<Multiply<Expression>>(*div); mN != nullptr) {
                mN->Flatten(flat_n);
            } else {
                result.push_back(div->GetMostSigOp().Copy());
            }

            if (auto mD = RecursiveCast<Multiply<Expression>>(*div); mD != nullptr) {
                mD->Flatten(flat_d);
            } else {
                denominator.push_back(div->GetLeastSigOp().Copy());
            }

            for (const auto& v : flat_n) {
                result.push_back(v->Copy());
            }
            for (const auto& v : flat_d) {
                denominator.push_back(v->Copy());
            }
        } else {
            result.push_back(num->Copy());
        }
    }

    for (const auto& denom : denominator) {
        size_t i = 0;
        if (auto real = RecursiveCast<Real>(*denom); real != nullptr) {
            for (; i < result.size(); i++) {
                if (auto resI = RecursiveCast<Real>(*result[i]); resI != nullptr) {
                    result[i] = Real { resI->GetValue() / real->GetValue() }.Generalize();
                    break;
                }
            }
            if (i >= result.size()) {
                result.push_back(Real { 1 / real->GetValue() }.Generalize());
            }
            continue;
        }
        if (auto img = RecursiveCast<Imaginary>(*denom); img != nullptr) {
            for (; i < result.size(); i++) {
                if (auto resI = RecursiveCast<Imaginary>(*result[i]); resI != nullptr) {
                    result[i] = Real { 1.0 }.Generalize();
                    break;
                }
            }
            if (i >= result.size()) {
                result.push_back(Exponent<Expression> { Imaginary {}, Real { -1.0 } }.Generalize());
            }
            continue;
        }
        if (auto var = RecursiveCast<Variable>(*denom); var != nullptr) {
            for (; i < result.size(); i++) {
                if (auto resIexp = RecursiveCast<Exponent<Variable, Expression>>(*result[i]); resIexp != nullptr) {
                    if (resIexp->GetMostSigOp().Equals(*var)) {
                        auto lsOp = Subtract<Expression> { resIexp->GetLeastSigOp(), Real { 1.0 } }.Accept(*this);
                        if (!lsOp) {
                            return lsOp;
                        }
                        result[i] = Exponent<Expression> { *var, *(lsOp.value()) }.Generalize();
                        break;
                    }
                } else if (auto resI = RecursiveCast<Variable>(*result[i]); resI != nullptr) {
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
        if (auto var = RecursiveCast<Exponent<Variable, Expression>>(*denom); var != nullptr) {
            for (; i < result.size(); i++) {
                if (auto resIexp = RecursiveCast<Exponent<Variable, Expression>>(*result[i]); resIexp != nullptr) {
                    if (resIexp->GetMostSigOp().Equals(var->GetMostSigOp())) {
                        auto lsOp = Subtract<Expression> { resIexp->GetLeastSigOp(), var->GetLeastSigOp() }.Accept(*this);
                        if (!lsOp) {
                            return lsOp;
                        }
                        result[i] = Exponent<Expression> { var->GetMostSigOp(), *(lsOp.value()) }.Generalize();
                        break;
                    }
                } else if (auto resI = RecursiveCast<Variable>(*result[i]); resI != nullptr) {
                    if (resI->Equals(*var)) {
                        auto lsOp = Subtract<Expression> { Real { 1.0 }, var->GetLeastSigOp() }.Accept(*this);
                        if (!lsOp) {
                            return lsOp;
                        }
                        result[i] = Exponent<Expression> { var->GetMostSigOp(), *(lsOp.value()) }.Generalize();
                    }
                }
            }
            if (i >= result.size()) {
                result.push_back(Exponent<Expression> { var->GetMostSigOp(), Multiply { Real { -1.0 }, var->GetLeastSigOp() } }.Generalize());
            }
            continue;
        }
        if (auto expExpr = RecursiveCast<Exponent<Expression>>(*denom); expExpr != nullptr) {
            for (; i < result.size(); i++) {
                if (auto resExpr = RecursiveCast<Exponent<Expression, Expression>>(*result[i]); resExpr != nullptr) {
                    if (expExpr->GetMostSigOp().Equals(resExpr->GetMostSigOp())) {
                        auto lsOp = Subtract { resExpr->GetLeastSigOp(), expExpr->GetLeastSigOp() }.Accept(*this);
                        if (!lsOp) {
                            return lsOp;
                        }
                        auto resultVisited = Exponent { expExpr->GetMostSigOp(), *(lsOp.value()) }.Accept(*this);
                        if (!resultVisited) {
                            return resultVisited;
                        }
                        result[i] = std::move(resultVisited).value();
                        break;
                    }
                } else if (result[i]->Equals(expExpr->GetMostSigOp())) {
                    auto lsOp = Subtract { Real { 1.0 }, expExpr->GetLeastSigOp() }.Accept(*this);
                    if (!lsOp) {
                        return lsOp;
                    }
                    auto resultVisited = Exponent { expExpr->GetMostSigOp(), *(lsOp.value()) }.Accept(*this);
                    if (!resultVisited) {
                        return resultVisited;
                    }
                    result[i] = std::move(resultVisited).value();
                    break;
                }
            }
            if (i >= result.size()) {
                auto resultVisited = Exponent<Expression> { expExpr->GetMostSigOp(), Multiply { Real { -1.0 }, expExpr->GetLeastSigOp() } }.Accept(*this);
                if (!resultVisited) {
                    return resultVisited;
                }
                result.push_back(std::move(resultVisited).value());
            }
            continue;
        }
        for (; i < result.size(); i++) {
            if (auto resExpr = RecursiveCast<Exponent<Expression, Expression>>(*result[i]); resExpr != nullptr) {
                if (denom->Equals(resExpr->GetMostSigOp())) {
                    auto lsOp = Subtract { resExpr->GetLeastSigOp(), Real { 1.0 } }.Accept(*this);
                    if (!lsOp) {
                        return lsOp;
                    }
                    auto resultVisited = Exponent { *denom, *(lsOp.value()) }.Accept(*this);
                    if (!resultVisited) {
                        return resultVisited;
                    }
                    result[i] = std::move(resultVisited).value();
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
        if (auto valI = RecursiveCast<Real>(*val); valI != nullptr) {
            if (valI->GetValue() != 1.0) {
                numeratorVals.push_back(valI->Generalize());
            }
        } else if (auto var = RecursiveCast<Variable>(*val); var != nullptr) {
            numeratorVals.push_back(val->Generalize());
        } else if (auto img = RecursiveCast<Imaginary>(*val); img != nullptr) {
            numeratorVals.push_back(val->Generalize());
        } else if (auto expR = RecursiveCast<Exponent<Expression, Real>>(*val); expR != nullptr) {
            if (expR->GetLeastSigOp().GetValue() < 0.0) {
                denominatorVals.push_back(Exponent { expR->GetMostSigOp(), Real { expR->GetLeastSigOp().GetValue() * -1.0 } }.Generalize());
            } else {
                numeratorVals.push_back(val->Generalize());
            }
        } else if (auto exp = RecursiveCast<Exponent<Expression, Multiply<Real, Expression>>>(*val); exp != nullptr) {
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
        if (auto exp = RecursiveCast<Exponent<Expression, Real>>(*val); exp != nullptr) {
            if (exp->GetLeastSigOp().GetValue() == 1.0) {
                val = exp->GetMostSigOp().Generalize();
            }
        }
    }

    for (auto& val : denominatorVals) {
        if (auto exp = RecursiveCast<Exponent<Expression, Real>>(*val); exp != nullptr) {
            if (exp->GetLeastSigOp().GetValue() == 1.0) {
                val = exp->GetMostSigOp().Generalize();
            }
        }
    }

    auto dividend = numeratorVals.size() == 1 ? std::move(numeratorVals.front()) : BuildFromVector<Multiply>(numeratorVals);
    auto divisor = denominatorVals.size() == 1 ? std::move(denominatorVals.front()) : BuildFromVector<Multiply>(denominatorVals);

    // rebuild subtrees
    if (!dividend && divisor)
        return gsl::not_null { Divide { Real { 1.0 }, *divisor }.Copy() };

    if (dividend && !divisor)
        return gsl::not_null { dividend->Copy() };

    if (!dividend && !divisor)
        return gsl::not_null { Real { 1.0 }.Copy() };

    return gsl::not_null { Divide { *dividend, *divisor }.Copy() };
}

auto SimplifyVisitor::TypedVisit(const Exponent<>& exponent) -> RetT
{
    static auto match_cast = MatchCast<Expression>()
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
                                     [](const Exponent<Real>&) -> bool { return true; },
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
                                     [](const Exponent<Exponent<>, Expression>& expExpCase, SimplifyVisitor* visitor) -> std::expected<gsl::not_null<std::unique_ptr<Expression>>, std::string_view> {
                                         auto lsOp = Multiply { expExpCase.GetMostSigOp().GetLeastSigOp(), expExpCase.GetLeastSigOp() }.Accept(*visitor);
                                         if (!lsOp) {
                                             return lsOp;
                                         }
                                         Exponent exp { expExpCase.GetMostSigOp().GetMostSigOp(), *(lsOp.value()) };
                                         return gsl::make_not_null(exp.Copy());
                                     })
                                 .Case(
                                     [](const Exponent<Expression, Log<>>& logCase) -> bool {
                                         return logCase.GetMostSigOp().Equals(logCase.GetLeastSigOp().GetMostSigOp());
                                     },
                                     [](const Exponent<Expression, Log<>>& logCase, const void*) -> std::expected<gsl::not_null<std::unique_ptr<Expression>>, std::string_view> {
                                         return gsl::make_not_null(logCase.GetLeastSigOp().GetLeastSigOp().Copy());
                                     });

    auto mostSigOp = exponent.GetMostSigOp().Copy();
    auto leastSigOp = exponent.GetLeastSigOp().Copy();
    if (!mostSigOp) {
        return std::unexpected { "Missing most significant operand." };
    }
    if (!leastSigOp) {
        return std::unexpected { "Missing least significant operand." };
    }

    auto simplifiedMostSigOpResult = mostSigOp->Accept(*this);
    auto simplifiedLeastSigOpResult = leastSigOp->Accept(*this);

    if (!simplifiedMostSigOpResult) {
        return std::unexpected { simplifiedMostSigOpResult.error() };
    }
    if (!simplifiedLeastSigOpResult) {
        return std::unexpected { simplifiedLeastSigOpResult.error() };
    }

    auto simplifiedBase = std::move(simplifiedMostSigOpResult).value();
    auto simplifiedPower = std::move(simplifiedLeastSigOpResult).value();

    const Exponent simplifiedExponent { *simplifiedBase, *simplifiedPower };
    auto matchResult = match_cast.Execute(simplifiedExponent, this).value();
    return gsl::not_null { matchResult ? std::move(matchResult) : std::move(simplifiedExponent.Copy()) };
}

auto SimplifyVisitor::TypedVisit(const Log<>& logIn) -> RetT
{
    auto mostSigOp = logIn.GetMostSigOp().Copy();
    auto leastSigOp = logIn.GetLeastSigOp().Copy();

    if (!mostSigOp) {
        return std::unexpected { "Missing most significant operand." };
    }
    if (!leastSigOp) {
        return std::unexpected { "Missing least significant operand." };
    }

    auto simplifiedMostSigOpResult = mostSigOp->Accept(*this);
    auto simplifiedLeastSigOpResult = leastSigOp->Accept(*this);

    if (!simplifiedMostSigOpResult) {
        return std::unexpected { simplifiedMostSigOpResult.error() };
    }
    if (!simplifiedLeastSigOpResult) {
        return std::unexpected { simplifiedLeastSigOpResult.error() };
    }

    auto simplifiedBase = std::move(simplifiedMostSigOpResult).value();
    auto simplifiedPower = std::move(simplifiedLeastSigOpResult).value();

    const Log simplifiedLog { *simplifiedBase, *simplifiedPower };

    if (const auto realBaseCase = RecursiveCast<Log<Real, Expression>>(simplifiedLog); realBaseCase != nullptr) {
        if (const Real& b = realBaseCase->GetMostSigOp(); b.GetValue() <= 0.0 || b.GetValue() == 1) {
            return gsl::not_null { std::make_unique<Undefined>() };
        }
    }

    if (const auto realExponentCase = RecursiveCast<Log<Expression, Real>>(simplifiedLog); realExponentCase != nullptr) {
        const Real& argument = realExponentCase->GetLeastSigOp();

        if (argument.GetValue() <= 0.0) {
            return gsl::not_null { std::make_unique<Undefined>() };
        }

        if (argument.GetValue() == 1.0) {
            return gsl::not_null { std::make_unique<Real>(0.0) };
        }
    }

    if (const auto realCase = RecursiveCast<Log<Real>>(simplifiedLog); realCase != nullptr) {
        const Real& base = realCase->GetMostSigOp();
        const Real& argument = realCase->GetLeastSigOp();

        return gsl::not_null { std::make_unique<Real>(log2(argument.GetValue()) * (1 / log2(base.GetValue()))) };
    }

    // log(a) with a < 0 log(-a)
    if (auto negCase = RecursiveCast<Log<Expression, Real>>(simplifiedLog); negCase != nullptr) {
        if (negCase->GetLeastSigOp().GetValue() < 0) {
            return gsl::not_null { Add<Expression> { Log { negCase->GetMostSigOp(), Real { -1 * negCase->GetLeastSigOp().GetValue() } }, Multiply<Expression> { Imaginary {}, Pi {} } }.Generalize() };
        }
    }

    // log[a](a) = 1
    if (const auto sameCase = RecursiveCast<Log<Expression, Expression>>(simplifiedLog); sameCase != nullptr) {
        if (sameCase->leastSigOp->Equals(*sameCase->mostSigOp)) {
            return gsl::not_null { Real { 1 }.Generalize() };
        }
    }

    // log[a](b^x) = x * log[a](b)
    if (const auto expCase = RecursiveCast<Log<Expression, Exponent<>>>(simplifiedLog); expCase != nullptr) {
        const auto exponent = expCase->GetLeastSigOp();
        const IExpression auto& log = Log<Expression>(expCase->GetMostSigOp(), exponent.GetMostSigOp()); // might need to check that it isnt nullptr
        const IExpression auto& factor = exponent.GetLeastSigOp();
        return Oasis::Multiply<Oasis::Expression>(factor, log).Accept(*this);
    }

    return gsl::not_null { simplifiedLog.Copy() };
}

auto SimplifyVisitor::TypedVisit(const Negate<Expression>& negate) -> RetT
{
    auto op = negate.GetOperand().Copy();
    if (!op) {
        return std::unexpected { "Missing operand." };
    }

    auto simplifiedMostSigOpResult = op->Accept(*this);

    if (!simplifiedMostSigOpResult) {
        return std::unexpected { simplifiedMostSigOpResult.error() };
    }

    auto simplifiedOp = std::move(simplifiedMostSigOpResult).value();

    return std::move(Multiply { Real { -1 }, *simplifiedOp }.Accept(*this)).value();
}

auto SimplifyVisitor::TypedVisit(const Sine<Expression>& sine) -> RetT
{
    auto op = sine.GetOperand().Copy();
    if (!op) {
        return std::unexpected { "Missing operand." };
    }

    auto simplifiedMostSigOpResult = op->Accept(*this);

    if (!simplifiedMostSigOpResult) {
        return std::unexpected { simplifiedMostSigOpResult.error() };
    }

    auto simplifiedOp = std::move(simplifiedMostSigOpResult).value();

    return simplifiedOp->Accept(*this);
}

auto SimplifyVisitor::TypedVisit(const Derivative<>& derivative) -> RetT
{
    auto mostSigOp = derivative.GetMostSigOp().Copy();
    auto leastSigOp = derivative.GetLeastSigOp().Copy();
    if (!mostSigOp) {
        return std::unexpected { "Missing most significant operand." };
    }
    if (!leastSigOp) {
        return std::unexpected { "Missing least significant operand." };
    }

    auto simplifiedMostSigOpResult = mostSigOp->Accept(*this);
    auto simplifiedLeastSigOpResult = leastSigOp->Accept(*this);

    if (!simplifiedMostSigOpResult) {
        return simplifiedMostSigOpResult;
    }
    if (!simplifiedLeastSigOpResult) {
        return simplifiedLeastSigOpResult;
    }

    auto simplifiedExpression = std::move(simplifiedMostSigOpResult).value();
    auto simplifiedVar = std::move(simplifiedLeastSigOpResult).value();
    auto simplifiedDiff = simplifiedExpression->Differentiate(*simplifiedVar);
    auto s = simplifiedDiff->Accept(*this);
    if (!s) {
        return s;
    }
    return gsl::not_null { std::move(s).value()->Accept(*this).value() };
}

auto SimplifyVisitor::TypedVisit(const Integral<>& integral) -> RetT
{
    auto mostSigOp = integral.GetMostSigOp().Copy();
    auto leastSigOp = integral.GetLeastSigOp().Copy();
    if (!mostSigOp) {
        return std::unexpected { "Missing most significant operand." };
    }
    if (!leastSigOp) {
        return std::unexpected { "Missing least significant operand." };
    }

    auto simplifiedMostSigOpResult = mostSigOp->Accept(*this);
    auto simplifiedLeastSigOpResult = leastSigOp->Accept(*this);

    if (!simplifiedMostSigOpResult) {
        return simplifiedMostSigOpResult;
    }
    if (!simplifiedLeastSigOpResult) {
        return simplifiedLeastSigOpResult;
    }

    auto simplifiedIntegrand = std::move(simplifiedMostSigOpResult).value();
    auto simplifiedDifferential = std::move(simplifiedLeastSigOpResult).value();

    auto integrated = simplifiedIntegrand->Integrate(*simplifiedDifferential);
    return gsl::not_null { std::move(integrated) };
}

auto SimplifyVisitor::TypedVisit(const Matrix& matrix) -> RetT
{
    return gsl::not_null { matrix.Copy() };
}

auto SimplifyVisitor::TypedVisit(const EulerNumber&) -> RetT
{
    return gsl::not_null { EulerNumber {}.Copy() };
}

auto SimplifyVisitor::TypedVisit(const Pi&) -> RetT
{
    return gsl::not_null { Pi {}.Copy() };
}

auto SimplifyVisitor::TypedVisit(const Magnitude<Expression>& magnitude) -> RetT
{
    auto simplified = magnitude.GetOperand().Accept(*this);
    if (!simplified) {
        return simplified;
    }
    auto simpOp = std::move(simplified).value();
    if (auto realCase = RecursiveCast<Real>(*simpOp); realCase != nullptr) {
        double val = realCase->GetValue();
        return gsl::not_null { val >= 0.0 ? std::make_unique<Real>(val) : std::make_unique<Real>(-val) };
    }
    if (auto imgCase = RecursiveCast<Imaginary>(*simpOp); imgCase != nullptr) {
        return gsl::not_null { std::make_unique<Real>(1.0) };
    }
    if (auto mulImgCase = RecursiveCast<Multiply<Expression, Imaginary>>(*simpOp); mulImgCase != nullptr) {
        return Magnitude<Expression> { mulImgCase->GetMostSigOp() }.Accept(*this);
    }
    if (auto addCase = RecursiveCast<Add<Expression, Imaginary>>(*simpOp); addCase != nullptr) {
        return Exponent { Add<Expression> { Exponent<Expression> { addCase->GetMostSigOp(), Real { 2 } },
                              Real { 1.0 } },
            Real { 0.5 } }
            .Accept(*this);
    }
    if (auto addCase = RecursiveCast<Add<Expression, Multiply<Expression, Imaginary>>>(*simpOp); addCase != nullptr) {
        return Exponent { Add<Expression> { Exponent<Expression> { addCase->GetMostSigOp(), Real { 2 } },
                              Exponent<Expression> { addCase->GetLeastSigOp().GetMostSigOp(), Real { 2 } } },
            Real { 0.5 } }
            .Accept(*this);
    }
    if (auto matrixCase = RecursiveCast<Matrix>(*simpOp); matrixCase != nullptr) {
        double sum = 0;
        for (size_t i = 0; i < matrixCase->GetRows(); i++) {
            for (size_t j = 0; j < matrixCase->GetCols(); j++) {
                sum += pow(matrixCase->GetMatrix()(i, j), 2);
            }
        }
        return Exponent { Real { sum }, Real { 0.5 } }.Accept(*this);
    }

    return gsl::not_null { magnitude.Generalize() };
}

} // Oasis
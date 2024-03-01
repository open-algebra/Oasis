//
// Created by Levy Lin on 2/09/2024.
//

#include "Oasis/Integrate.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Log.hpp"

namespace Oasis{

Integrate<Expression>::Integrate(const Expression& integrandend, const Expression& differentialend)
    : BinaryExpression(integrandend, differentialend)
{
}

auto Integrate<Expression>::Simplify() const -> std::unique_ptr<Expression>
{
    // To be implemented : Returns simplified expression

    auto simplifiedOperandLeft = mostSigOp ? mostSigOp->Simplify() : nullptr;
    auto simplifiedOperandRight = leastSigOp ? leastSigOp->Simplify() : nullptr;

    Integrate simplifiedIntegrate { *simplifiedOperandLeft, *simplifiedOperandRight };


    // Add Cases

    if (auto realCaseAdd = Add<Real>::Specialize(simplifiedIntegrate); realCaseAdd != nullptr) {
        const Real& firstReal = realCaseAdd -> GetMostSigOp();
        const Real& secondReal = realCaseAdd -> GetLeastSigOp();

        return std::make_unique<Real>(firstReal.GetValue() + secondReal.GetValue());
    }

    if (auto likeTermsCase = Add<Multiply<Real, Expression>>::Specialize(simplifiedIntegrate); likeTermsCase != nullptr) {
        const Oasis::IExpression auto& leftTerm = likeTermsCase->GetMostSigOp().GetLeastSigOp();
        const Oasis::IExpression auto& rightTerm = likeTermsCase->GetLeastSigOp().GetLeastSigOp();

        // x + x = 2x
        if (simplifiedOperandLeft->Equals(*simplifiedOperandRight)){
            return Multiply<Real, Expression> {Real { 2.0 }, *simplifiedOperandLeft}.Simplify();
        }

        if (leftTerm.Equals(rightTerm)) {
            const Real& coefficient1 = likeTermsCase ->GetMostSigOp().GetMostSigOp();
            const Real& coefficient2 = likeTermsCase ->GetLeastSigOp().GetMostSigOp();

            return std::make_unique<Multiply<Expression>>( Real(coefficient1.GetValue() + coefficient2.GetValue()), leftTerm);
        }
    }

    // log(a) + log(b) = log(ab)
    if (auto logAddCase = Add<Log<Expression, Expression>, Log<Expression, Expression>>::Specialize(simplifiedIntegrate); logAddCase != nullptr) {
        if (logAddCase->GetMostSigOp().GetMostSigOp().Equals(logAddCase->GetLeastSigOp().GetMostSigOp())) {
            const IExpression auto& base = logAddCase->GetMostSigOp().GetMostSigOp();
            const IExpression auto& argument = Multiply<Expression>({ logAddCase->GetMostSigOp().GetLeastSigOp(), logAddCase->GetLeastSigOp().GetLeastSigOp() });
            return std::make_unique<Log<Expression>>(base, argument);
        }
    }

    // 2x + x = 3x
    if (const auto likeTermsCase2 = Add<Multiply<Real, Expression>, Expression>::Specialize(simplifiedIntegrate); likeTermsCase2 != nullptr) {
        if (likeTermsCase2->GetMostSigOp().GetLeastSigOp().Equals(likeTermsCase2->GetLeastSigOp())) {
            const Real& coefficient = likeTermsCase2->GetMostSigOp().GetMostSigOp();
            return std::make_unique<Multiply<Real, Expression>>(Real { coefficient.GetValue() + 1}, likeTermsCase2->GetMostSigOp().GetLeastSigOp());
        }
    }

    // simplifies expressions and combines like terms
    // ex: 1 + 2x + 3 + 5x = 4 + 7x (or 7x + 4)
    std::vector<std::unique_ptr<Expression>> adds;
    std::vector<std::unique_ptr<Expression>> add_vals;
    simplifiedIntegrate.Flatten(adds);
    // To Do
    // for (...)

    // Subtract Cases

    if (auto realCaseSubtract = Subtract<Real>::Specialize(simplifiedIntegrate); realCaseSubtract != nullptr) {
        const Real& firstReal = realCaseSubtract -> GetMostSigOp();
        const Real& secondReal = realCaseSubtract -> GetLeastSigOp();

        return std::make_unique<Real>(firstReal.GetValue() - secondReal.GetValue());
    }

    if (auto imgCaseSubtract = Subtract<Imaginary>::Specialize(simplifiedIntegrate); imgCaseSubtract != nullptr) {
        return std::make_unique<Multiply<Real, Imaginary>>( Real{2.0}, Imaginary {});
    }

    if (auto imgCaseSubtract = Subtract<Multiply<Real, Imaginary>, Imaginary>::Specialize(simplifiedIntegrate); imgCaseSubtract != nullptr) {
        return std::make_unique<Multiply<Expression>>(
            *(Subtract { imgCaseSubtract->GetMostSigOp().GetMostSigOp(), Real { 1.0 } }.Simplify()), Imaginary {});
    }

    if (auto imgCaseSubtract = Subtract<Imaginary, Multiply<Real, Imaginary>>::Specialize(simplifiedIntegrate); imgCaseSubtract != nullptr) {
        return std::make_unique<Multiply<Expression>>(
            *(Subtract { Real { 1.0 }, imgCaseSubtract->GetLeastSigOp().GetMostSigOp() }.Simplify()), Imaginary {});
    }

    if (auto ImgCase = Subtract<Multiply<Expression, Imaginary>, Multiply<Expression, Imaginary>>::Specialize(simplifiedIntegrate); ImgCase != nullptr) {
        return std::make_unique<Multiply<Expression>>(
            *(Subtract { ImgCase->GetLeastSigOp().GetMostSigOp(), ImgCase->GetMostSigOp().GetMostSigOp() }.Simplify()), Imaginary {});
    }

    // exponent - exponent
    if (auto exponentCase = Subtract<Exponent<Expression>, Exponent<Expression>>::Specialize(simplifiedIntegrate); exponentCase != nullptr) {
        if (exponentCase->GetMostSigOp().GetMostSigOp().Equals(exponentCase->GetLeastSigOp().GetMostSigOp()) && exponentCase->GetMostSigOp().GetLeastSigOp().Equals(exponentCase->GetLeastSigOp().GetLeastSigOp())) {
            return std::make_unique<Real>(Real { 0.0 });
        }
    }

    // a*exponent - exponent
    if (auto exponentCase = Subtract<Multiply<Expression, Exponent<Expression>>, Exponent<Expression>>::Specialize(simplifiedIntegrate); exponentCase != nullptr) {
        if (exponentCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exponentCase->GetLeastSigOp().GetMostSigOp()) && exponentCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp().Equals(exponentCase->GetLeastSigOp().GetLeastSigOp())) {
            if (Real { 1.0 }.Equals(exponentCase->GetMostSigOp().GetMostSigOp()))
                return std::make_unique<Real>(Real { 0.0 });
            return std::make_unique<Multiply<Expression>>(*(Subtract { exponentCase->GetMostSigOp().GetMostSigOp(), Real { 1.0 } }.Simplify()),
                exponentCase->GetLeastSigOp());
        }
    }

    // exponent - a*exponent
    if (auto exponentCase = Subtract<Exponent<Expression>, Multiply<Expression, Exponent<Expression>>>::Specialize(simplifiedIntegrate); exponentCase != nullptr) {
        if (exponentCase->GetLeastSigOp().GetLeastSigOp().GetMostSigOp().Equals(exponentCase->GetMostSigOp().GetMostSigOp()) && exponentCase->GetLeastSigOp().GetLeastSigOp().GetLeastSigOp().Equals(exponentCase->GetMostSigOp().GetLeastSigOp())) {
            if (Real { 1.0 }.Equals(exponentCase->GetLeastSigOp().GetMostSigOp()))
                return std::make_unique<Real>(Real { 0.0 });
            return std::make_unique<Multiply<Expression>>(*(Subtract { Real { 1.0 }, exponentCase->GetLeastSigOp().GetMostSigOp() }.Simplify()),
                exponentCase->GetMostSigOp());
        }
    }

    // a*exponent - b*exponent
    if (auto exponentCase = Subtract<Multiply<Expression, Exponent<Expression>>, Multiply<Expression, Exponent<Expression>>>::Specialize(simplifiedIntegrate); exponentCase != nullptr) {
        if (exponentCase->GetLeastSigOp().GetLeastSigOp().GetMostSigOp().Equals(exponentCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp()) && exponentCase->GetLeastSigOp().GetLeastSigOp().GetLeastSigOp().Equals(exponentCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp())) {
            if (Real { 1.0 }.Equals(exponentCase->GetLeastSigOp().GetMostSigOp()))
                return std::make_unique<Real>(Real { 0.0 });
            return std::make_unique<Multiply<Expression>>(
                *(Subtract { exponentCase->GetMostSigOp().GetMostSigOp(), exponentCase->GetLeastSigOp().GetMostSigOp() }.Simplify()),
                exponentCase->GetMostSigOp().GetLeastSigOp());
        }
    }

    // log(a) - log(b) = log(a / b)
    if (auto logCase = Subtract<Log<Expression, Expression>, Log<Expression, Expression>>::Specialize(simplifiedIntegrate); logCase != nullptr) {
        if (logCase->GetMostSigOp().GetMostSigOp().Equals(logCase->GetLeastSigOp().GetMostSigOp())) {
            const IExpression auto& base = logCase->GetMostSigOp().GetMostSigOp();
            const IExpression auto& argument = Divide<Expression>({ logCase->GetMostSigOp().GetLeastSigOp(), logCase->GetLeastSigOp().GetLeastSigOp() });
            return std::make_unique<Log<Expression>>(base, argument);
        }
    }


    // Multiply Cases

    if (auto realCaseMultiply = Multiply<Real>::Specialize(simplifiedIntegrate); realCaseMultiply != nullptr) {
        const Real& firstReal = realCaseMultiply -> GetMostSigOp();
        const Real& secondReal = realCaseMultiply -> GetLeastSigOp();

        return std::make_unique<Real>(firstReal.GetValue() * secondReal.GetValue());
    }

    if (auto ImgCaseMultiply = Multiply<Imaginary>::Specialize(simplifiedIntegrate); ImgCaseMultiply != nullptr) {
        return std::make_unique<Real>(-1.0);
    }

    if (auto exprCaseMultiply = Multiply<Expression>::Specialize(simplifiedIntegrate); exprCaseMultiply != nullptr) {
        if (exprCaseMultiply->GetMostSigOp().Equals(exprCaseMultiply->GetLeastSigOp())) {
            return std::make_unique<Exponent<Expression, Expression>>(exprCaseMultiply->GetMostSigOp(), Real { 2.0 });
        }
    }

    if (auto exprCaseMultiply = Multiply<Expression, Exponent<Expression, Expression>>::Specialize(simplifiedIntegrate); exprCaseMultiply != nullptr) {
        if (exprCaseMultiply->GetMostSigOp().Equals(exprCaseMultiply->GetLeastSigOp().GetMostSigOp())) {
            return std::make_unique<Exponent<Expression>>(exprCaseMultiply->GetMostSigOp(),
                *(Add<Expression> { exprCaseMultiply->GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } }.Simplify()));
        }
    }

    // x*x^n
    if (auto exprCaseMultiply = Multiply<Expression, Exponent<Expression>>::Specialize(simplifiedIntegrate); exprCaseMultiply != nullptr) {
        if (exprCaseMultiply->GetMostSigOp().Equals(exprCaseMultiply->GetLeastSigOp().GetMostSigOp())) {
            return std::make_unique<Exponent<Expression>>(exprCaseMultiply->GetMostSigOp(),
                *(Add<Expression> { exprCaseMultiply->GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } }.Simplify()));
        }
    }

    if (auto exprCaseMultiply = Multiply<Exponent<Expression>, Expression>::Specialize(simplifiedIntegrate); exprCaseMultiply != nullptr) {
        if (exprCaseMultiply->GetLeastSigOp().Equals(exprCaseMultiply->GetMostSigOp().GetMostSigOp())) {
            return std::make_unique<Exponent<Expression>>(exprCaseMultiply->GetLeastSigOp(),
                *(Add<Expression> { exprCaseMultiply->GetMostSigOp().GetLeastSigOp(), Real { 1.0 } }.Simplify()));
        }
    }

    // x^n*x^m
    if (auto exprCaseMultiply = Multiply<Exponent<Expression>, Exponent<Expression>>::Specialize(simplifiedIntegrate); exprCaseMultiply != nullptr) {
        if (exprCaseMultiply->GetMostSigOp().GetMostSigOp().Equals(exprCaseMultiply->GetLeastSigOp().GetMostSigOp())) {
            return std::make_unique<Exponent<Expression>>(exprCaseMultiply->GetMostSigOp().GetMostSigOp(),
                *(Add<Expression> { exprCaseMultiply->GetMostSigOp().GetLeastSigOp(), exprCaseMultiply->GetLeastSigOp().GetLeastSigOp() }.Simplify()));
        }
    }

    // a*x*x
    if (auto exprCaseMultiply = Multiply<Multiply<Expression>, Expression>::Specialize(simplifiedIntegrate); exprCaseMultiply != nullptr) {
        if (exprCaseMultiply->GetMostSigOp().GetLeastSigOp().Equals(exprCaseMultiply->GetLeastSigOp())) {
            return std::make_unique<Multiply<Expression, Expression>>(exprCaseMultiply->GetMostSigOp().GetMostSigOp(),
                Exponent<Expression> { exprCaseMultiply->GetMostSigOp().GetLeastSigOp(), Real { 2.0 } });
        }
    }

    // a*x*b*x
    if (auto exprCaseMultiply = Multiply<Multiply<Expression>, Multiply<Expression>>::Specialize(simplifiedIntegrate); exprCaseMultiply != nullptr) {
        if (exprCaseMultiply->GetMostSigOp().GetLeastSigOp().Equals(exprCaseMultiply->GetLeastSigOp().GetLeastSigOp())) {
            return std::make_unique<Multiply<Expression>>(
                *(Multiply<Expression> { exprCaseMultiply->GetMostSigOp().GetMostSigOp(), exprCaseMultiply->GetLeastSigOp().GetMostSigOp() }.Simplify()),
                *(Exponent<Expression> { exprCaseMultiply->GetMostSigOp().GetLeastSigOp(), Real { 2.0 } }.Simplify()));
        }
    }

    // a*x^n*x
    if (auto exprCase = Multiply<Multiply<Expression, Exponent<Expression>>, Expression>::Specialize(simplifiedIntegrate); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exprCase->GetLeastSigOp())) {
            return std::make_unique<Multiply<Expression>>(exprCase->GetMostSigOp().GetMostSigOp(),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp(),
                    *(Add<Expression> { exprCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } }.Simplify()) });
        }
    }

    // a*x*x^n
    if (auto exprCase = Multiply<Multiply<Expression>, Exponent<Expression>>::Specialize(simplifiedIntegrate); exprCase != nullptr) {
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
    if (auto exprCase = Multiply<Multiply<Expression>, Multiply<Expression, Exponent<Expression>>>::Specialize(simplifiedIntegrate); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().Equals(exprCase->GetLeastSigOp().GetLeastSigOp().GetMostSigOp())) {
            return std::make_unique<Multiply<Expression>>(
                *(Multiply<Expression> { exprCase->GetMostSigOp().GetMostSigOp(), exprCase->GetLeastSigOp().GetMostSigOp() }.Simplify()),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(),
                    *(Add<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } }.Simplify()) });
        }
    }

    if (auto exprCase = Multiply<Multiply<Expression>, Multiply<Exponent<Expression>, Expression>>::Specialize(simplifiedIntegrate); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().Equals(exprCase->GetLeastSigOp().GetMostSigOp().GetMostSigOp())) {
            return std::make_unique<Multiply<Expression>>(
                *(Multiply<Expression> { exprCase->GetMostSigOp().GetMostSigOp(), exprCase->GetLeastSigOp().GetLeastSigOp() }.Simplify()),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp(),
                    *(Add<Expression> { exprCase->GetLeastSigOp().GetMostSigOp().GetLeastSigOp(), Real { 1.0 } }.Simplify()) });
        }
    }

    if (auto exprCase = Multiply<Multiply<Expression, Exponent<Expression>>, Multiply<Expression>>::Specialize(simplifiedIntegrate); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetLeastSigOp())) {
            return std::make_unique<Multiply<Expression>>(
                *(Multiply<Expression> { exprCase->GetMostSigOp().GetMostSigOp(), exprCase->GetLeastSigOp().GetLeastSigOp() }.Simplify()),
                Exponent<Expression> { exprCase->GetLeastSigOp().GetLeastSigOp(),
                    *(Add<Expression> { exprCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp(), Real { 1.0 } }.Simplify()) });
        }
    }

    // a*x^n*x^m
    if (auto exprCase = Multiply<Multiply<Expression, Exponent<Expression>>, Exponent<Expression>>::Specialize(simplifiedIntegrate); exprCase != nullptr) {
        if (exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exprCase->GetLeastSigOp().GetLeastSigOp())) {
            return std::make_unique<Multiply<Expression>>(
                exprCase->GetMostSigOp().GetMostSigOp(),
                Exponent<Expression> { exprCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp(),
                    *(Add<Expression> { exprCase->GetLeastSigOp().GetMostSigOp(), exprCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp() }.Simplify()) });
        }
    }

    // a*x^n*b*x^m
    if (auto exprCaseMultiply = Multiply<Multiply<Expression, Exponent<Expression>>, Multiply<Expression, Exponent<Expression>>>::Specialize(simplifiedIntegrate); exprCaseMultiply != nullptr) {
        if (exprCaseMultiply->GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exprCaseMultiply->GetLeastSigOp().GetLeastSigOp().GetMostSigOp())) {
            return std::make_unique<Multiply<Expression>>(
                *(Multiply<Expression> { exprCaseMultiply->GetMostSigOp().GetMostSigOp(), exprCaseMultiply->GetLeastSigOp().GetMostSigOp() }.Simplify()),
                Exponent<Expression> { exprCaseMultiply->GetMostSigOp().GetLeastSigOp().GetMostSigOp(),
                    *(Add<Expression> { exprCaseMultiply->GetLeastSigOp().GetLeastSigOp().GetLeastSigOp(), exprCaseMultiply->GetMostSigOp().GetLeastSigOp().GetLeastSigOp() }.Simplify()) });
        }
    }

    // multiply add like terms
    std::vector<std::unique_ptr<Expression>> multiplies;
    // std::vector<std::unique_ptr<Expression>> vals;
    simplifiedIntegrate.Flatten(multiplies);

    // for (...)


    // Divide Cases

    if (auto realCaseDivide = Divide<Real>::Specialize(simplifiedIntegrate); realCaseDivide != nullptr) {
        const Real& firstReal = realCaseDivide -> GetMostSigOp();
        const Real& secondReal = realCaseDivide -> GetLeastSigOp();

        return std::make_unique<Real>(firstReal.GetValue() / secondReal.GetValue());
    }

    // log(a)/log(b)=log[b](a)
    if (auto logCaseDivide = Divide<Log<Expression, Expression>, Log<Expression, Expression>>::Specialize(simplifiedIntegrate); logCaseDivide != nullptr) {
        if (logCaseDivide->GetMostSigOp().GetMostSigOp().Equals(logCaseDivide->GetLeastSigOp().GetMostSigOp())) {
            const IExpression auto& base = logCaseDivide->GetLeastSigOp().GetLeastSigOp();
            const IExpression auto& argument = logCaseDivide->GetMostSigOp().GetLeastSigOp();
            return std::make_unique<Log<Expression>>(base, argument);
        }
    }

    // for(...)











    // return simplifiedIntegrate.copy() or return vals after build
    return simplifiedIntegrate.copy();
}

auto Integrate<Expression>::ToString() const -> std::string
{
    return fmt::format("({} + {})", mostSigOp->ToString(), leastSigOp->ToString());
}

auto Integrate<Expression>::Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression>
{
    // To be Implemented : Performs Integration on simplified Expression
    std::unique_ptr<Expression> simplifiedIntegrand, simplifiedDifferential;
}

auto Integrate<Expression>::Specialize(const Expression& other) -> std::unique_ptr<Integrate<Expression, Expression>>
{
    if (!other.Is<Oasis::Integrate>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize();
    return std::make_unique<Integrate>(dynamic_cast<const Integrate&>(*otherGeneralized));
}

auto Integrate<Expression>::Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Integrate>
{
    if (!other.Is<Oasis::Integrate>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize(subflow);
    return std::make_unique<Integrate>(dynamic_cast<const Integrate&>(*otherGeneralized));
}

} // Oasis

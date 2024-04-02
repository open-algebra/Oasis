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

namespace Oasis {

Integrate<Expression>::Integrate(const Expression& integrandend, const Expression& differentialend)
    : BinaryExpression(integrandend, differentialend)
{
}

auto Integrate<Expression>::Simplify() const -> std::unique_ptr<Expression>
{
    // Returns simplified Integral

    auto simplifiedIntegrand = mostSigOp ? mostSigOp->Simplify() : nullptr;
    auto simplifiedDifferential = leastSigOp ? leastSigOp->Simplify() : nullptr;

    Integrate simplifiedIntegrate { *simplifiedIntegrand, *simplifiedDifferential };


    // Add Cases

    if (auto realAddCase = Integrate<Add<Real>, Expression>::Specialize(simplifiedIntegrate); realAddCase != nullptr) {
        const Real& firstReal = realAddCase->GetMostSigOp().GetMostSigOp();
        const Real& secondReal = realAddCase->GetMostSigOp().GetLeastSigOp();
        const Expression& differential = realAddCase -> GetLeastSigOp();

        return std::make_unique<Integrate<Real, Expression>>( Real(firstReal.GetValue() + secondReal.GetValue()), differential);
    }

    if (auto likeTermsAddCase = Integrate<Add<Multiply<Real, Expression>>, Expression>::Specialize(simplifiedIntegrate); likeTermsAddCase != nullptr) {
        const Oasis::IExpression auto& leftTerm = likeTermsAddCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp();
        const Oasis::IExpression auto& rightTerm = likeTermsAddCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp();
        const Expression& differential = likeTermsAddCase -> GetLeastSigOp();

        if (leftTerm.Equals(rightTerm)) {
            const Real& coefficient1 = likeTermsAddCase->GetMostSigOp().GetMostSigOp().GetMostSigOp();
            const Real& coefficient2 = likeTermsAddCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp();

            return std::make_unique<Integrate<Multiply<Real, Expression>, Expression>>( Multiply( Real(coefficient1.GetValue() + coefficient2.GetValue()), leftTerm), differential);
        }
    }

    // log(a) + log(b) = log(ab)
    if (auto logAddCase = Integrate<Add<Log<Expression, Expression>, Log<Expression, Expression>>, Expression>::Specialize(simplifiedIntegrate); logAddCase != nullptr) {
        if (logAddCase->GetMostSigOp().GetMostSigOp().GetMostSigOp().Equals(logAddCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp())) {
            const IExpression auto& base = logAddCase->GetMostSigOp().GetMostSigOp().GetMostSigOp();
            const IExpression auto& argument = Multiply<Expression>({ logAddCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp(), logAddCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp() });
            const Expression& differential = logAddCase -> GetLeastSigOp();

            return std::make_unique<Integrate<Log<Expression, Multiply<Expression>>, Expression>>( Log(base, argument), differential);
        }
    }

    // x + x = 2x
    if (auto liketermCase = Integrate<Add<Expression>, Expression>::Specialize(simplifiedIntegrate); liketermCase != nullptr) {
        if (liketermCase->GetMostSigOp().GetMostSigOp().Equals(liketermCase->GetMostSigOp().GetLeastSigOp())) {
            const Expression& differential = liketermCase->GetLeastSigOp();

            return std::make_unique<Integrate<Multiply<Real, Expression>, Expression>>( Multiply(Real{2.0}, liketermCase->GetMostSigOp().GetMostSigOp()), differential);
        }
    }

    // 2x + x = 3x
    if (const auto likeTermsCase2 = Integrate<Add<Multiply<Real, Expression>, Expression>, Expression>::Specialize(simplifiedIntegrate); likeTermsCase2 != nullptr) {
        if (likeTermsCase2->GetMostSigOp().GetMostSigOp().GetLeastSigOp().Equals(likeTermsCase2->GetMostSigOp().GetLeastSigOp())) {
            const Real& coeffiecent = likeTermsCase2->GetMostSigOp().GetMostSigOp().GetMostSigOp();
            return std::make_unique<Multiply<Real, Expression>>(Real { coeffiecent.GetValue() + 1 }, likeTermsCase2->GetMostSigOp().GetMostSigOp().GetLeastSigOp());
        }
    }

    // For Add Statments

    // Subtract Cases

    if (auto realSubCase = Integrate<Subtract<Real>, Expression>::Specialize(simplifiedIntegrate); realSubCase != nullptr) {
        const Real& minuend = realSubCase->GetMostSigOp().GetMostSigOp();
        const Real& subtrahend = realSubCase->GetMostSigOp().GetLeastSigOp();
        const Expression& differential = realSubCase -> GetLeastSigOp();

        return std::make_unique<Integrate<Real, Expression>>( Real(minuend.GetValue() - subtrahend.GetValue()), differential);
    }

    if (auto ImgSubCase = Integrate<Subtract<Imaginary>, Expression>::Specialize(simplifiedIntegrate); ImgSubCase != nullptr) {
        const Expression& differential = ImgSubCase -> GetLeastSigOp();
        return std::make_unique<Integrate<Multiply<Real, Imaginary>, Expression>>( Multiply(Real { 2.0 }, Imaginary {}), differential);
    }

    if (auto ImgSubCase = Integrate<Subtract<Multiply<Expression, Imaginary>, Imaginary>, Expression>::Specialize(simplifiedIntegrate); ImgSubCase != nullptr) {
        const Expression& differential = ImgSubCase -> GetLeastSigOp();
        return std::make_unique<Integrate<Multiply<Expression, Imaginary>, Expression>>(
            ( Multiply (*(Subtract{ ImgSubCase->GetMostSigOp().GetMostSigOp().GetMostSigOp(), Real {1.0} }.Simplify()), Imaginary {} )), differential);
    }

    if (auto ImgSubCase = Integrate<Subtract<Imaginary, Multiply<Expression, Imaginary>>, Expression>::Specialize(simplifiedIntegrate); ImgSubCase != nullptr) {
        const Expression& differential = ImgSubCase -> GetLeastSigOp();
        return std::make_unique<Integrate<Multiply<Expression, Imaginary>, Expression>>(
            ( Multiply (*(Subtract{ Real {1.0}, ImgSubCase -> GetMostSigOp().GetLeastSigOp().GetMostSigOp() }.Simplify()), Imaginary {} )), differential);
    }

    if (auto ImgSubCase = Integrate<Subtract<Multiply<Expression, Imaginary>, Multiply<Expression, Imaginary>>, Expression>::Specialize(simplifiedIntegrate); ImgSubCase != nullptr) {
        const Expression& differential = ImgSubCase -> GetLeastSigOp();
        return std::make_unique<Integrate<Multiply<Expression, Imaginary>, Expression>>(
            ( Multiply( *( Subtract( ImgSubCase -> GetMostSigOp().GetMostSigOp().GetMostSigOp(), ImgSubCase -> GetMostSigOp().GetLeastSigOp().GetMostSigOp() ).Simplify() ), Imaginary {} )), differential);
    }

    // exponent - exponent
    if (auto exponentSubCase = Integrate<Subtract<Exponent<Expression>, Exponent<Expression>>, Expression>::Specialize(simplifiedIntegrate); exponentSubCase != nullptr) {
        if (exponentSubCase->GetMostSigOp().GetMostSigOp().GetMostSigOp().Equals(exponentSubCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp()) && exponentSubCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp().Equals(exponentSubCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp())) {
            const Expression& differential = exponentSubCase->GetLeastSigOp();
            return std::make_unique<Integrate<Real, Expression>>( Real {0.0}, differential);
        }
    }

    // a*exponent - exponent
    if (auto exponentSubCase = Integrate<Subtract<Multiply<Expression, Exponent<Expression>>, Exponent<Expression>>, Expression>::Specialize(simplifiedIntegrate); exponentSubCase != nullptr) {
        if (exponentSubCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exponentSubCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp()) && exponentSubCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp().GetLeastSigOp().Equals(exponentSubCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp())) {
            const Expression& differential = exponentSubCase->GetLeastSigOp();
            if (Real {1.0}.Equals(exponentSubCase->GetMostSigOp().GetMostSigOp().GetMostSigOp())) {
                return std::make_unique<Integrate<Real, Expression>>( Real {0.0}, differential);
            }
            return std::make_unique<Integrate<Multiply<Expression, Exponent<Expression>>, Expression>>(
                Multiply( *(Subtract{ exponentSubCase->GetMostSigOp().GetMostSigOp().GetMostSigOp() , Real {1.0} }.Simplify() ), exponentSubCase->GetMostSigOp().GetLeastSigOp() ), differential);
        }
    }

    // exponent - a*exponent
    if (auto exponentSubCase = Integrate<Subtract<Exponent<Expression>, Multiply<Expression, Exponent<Expression>>>, Expression>::Specialize(simplifiedIntegrate); exponentSubCase != nullptr) {
        if (exponentSubCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp().GetMostSigOp().Equals(exponentSubCase->GetMostSigOp().GetMostSigOp().GetMostSigOp()) && exponentSubCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp().GetLeastSigOp().Equals(exponentSubCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp())) {
            const Expression& differential = exponentSubCase->GetLeastSigOp();
            if (Real {1.0}.Equals(exponentSubCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp())) {
                return std::make_unique<Integrate<Real, Expression>>( Real {0.0}, differential);
            }
            return std::make_unique<Integrate<Multiply<Expression, Exponent<Expression>>, Expression>>(
                Multiply( *(Subtract{ Real {1.0}, exponentSubCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp() }.Simplify() ), exponentSubCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp() ), differential);
        }
    }

    // a*exponent - b*exponent
    if (auto exponentSubCase = Integrate<Subtract<Multiply<Expression, Exponent<Expression>>, Multiply<Expression, Exponent<Expression>>>, Expression>::Specialize(simplifiedIntegrate); exponentSubCase != nullptr) {
        if (exponentSubCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exponentSubCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp().GetMostSigOp()) && exponentSubCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp().GetLeastSigOp().Equals(exponentSubCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp().GetLeastSigOp())) {
            const Expression& differential = exponentSubCase->GetLeastSigOp();
            if ( exponentSubCase->GetMostSigOp().GetMostSigOp().GetMostSigOp().Equals(exponentSubCase -> GetMostSigOp().GetLeastSigOp().GetMostSigOp()) ) {
                return std::make_unique<Integrate<Real, Expression>> ( Real {0.0}, differential);
            }

            return std::make_unique<Integrate<Multiply<Expression, Exponent<Expression>>, Expression>>(
                    Multiply( *(Subtract{ exponentSubCase->GetMostSigOp().GetMostSigOp().GetMostSigOp(), exponentSubCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp() }.Simplify()), exponentSubCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp() ), differential);
        }
    }

    // log(a) - log(b) = log(a / b)
    if (auto logSubCase = Integrate<Subtract<Log<Expression, Expression>, Log<Expression, Expression>>, Expression>::Specialize(simplifiedIntegrate); logSubCase != nullptr) {
        if (logSubCase->GetMostSigOp().GetMostSigOp().GetMostSigOp().Equals(logSubCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp())) {
            const Expression& differential = logSubCase->GetLeastSigOp();
            const IExpression auto& base = logSubCase->GetMostSigOp().GetMostSigOp().GetMostSigOp();
            const IExpression auto& argument = Divide<Expression>( { logSubCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp(), logSubCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp() } );

            return std::make_unique<Integrate<Log<Expression, Expression>, Expression>>( Log({base, argument}), differential);
        }
    }

    // Multiply Cases

    if (auto realMultCase = Integrate<Multiply<Real>, Expression>::Specialize(simplifiedIntegrate); realMultCase != nullptr) {
        const Expression& differential = realMultCase->GetLeastSigOp();
        const Real& multiplicand = realMultCase->GetMostSigOp().GetMostSigOp();
        const Real& multiplier = realMultCase->GetMostSigOp().GetLeastSigOp();

        return std::make_unique<Integrate<Real, Expression>>( Real(multiplicand.GetValue() * multiplier.GetValue()), differential );
    }

    // i * i = -1
    if (auto imgMultCase = Integrate<Multiply<Imaginary>, Expression>::Specialize(simplifiedIntegrate); imgMultCase != nullptr) {
        const Expression& differential = imgMultCase->GetLeastSigOp();

        return std::make_unique<Integrate<Real, Expression>> (Real { -1.0 }, differential);
    }

    // x^1 * x^1 = x^2
    if (auto exponentMultCase = Integrate<Multiply<Expression>, Expression>::Specialize(simplifiedIntegrate); exponentMultCase != nullptr) {
        if (exponentMultCase->GetMostSigOp().GetMostSigOp().Equals(exponentMultCase->GetMostSigOp().GetLeastSigOp())) {
            const Expression& differential = exponentMultCase->GetLeastSigOp();
            return std::make_unique<Integrate<Exponent<Expression>, Expression>> (Exponent<Expression>(exponentMultCase->GetMostSigOp().GetMostSigOp(), Real{2.0}) , differential);
        }
    }

    // x * x^n = x^(1 + n)
    if (auto exponentMultCase = Integrate<Multiply<Expression, Exponent<Expression, Expression>>, Expression>::Specialize(simplifiedIntegrate); exponentMultCase != nullptr) {
        if (exponentMultCase->GetMostSigOp().GetMostSigOp().Equals(exponentMultCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp())) {
            const Expression& differential = exponentMultCase->GetLeastSigOp();

            return std::make_unique<Integrate<Exponent<Expression>, Expression>>(
                Exponent( exponentMultCase->GetMostSigOp().GetMostSigOp(),
                    *(Add<Expression>{ Real{1.0}, exponentMultCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp() }.Simplify()) ), differential);
        }
    }

    // x^n * x = x^(n + 1)
    if (auto exponentMultCase = Integrate<Multiply<Exponent<Expression, Expression>, Expression>, Expression>::Specialize(simplifiedIntegrate); exponentMultCase != nullptr) {
        if (exponentMultCase->GetMostSigOp().GetMostSigOp().GetMostSigOp().Equals(exponentMultCase->GetMostSigOp().GetLeastSigOp())) {
            const Expression& differential = exponentMultCase->GetLeastSigOp();

            return std::make_unique<Integrate<Exponent<Expression>, Expression>>(
                Exponent( exponentMultCase->GetMostSigOp().GetLeastSigOp(),
                    *(Add<Expression>{ exponentMultCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp(), Real {1.0}}.Simplify()) ), differential);
        }
    }

    // x^n * x^m = x^(n + m)
    if (auto exponentMultCase = Integrate<Multiply<Exponent<Expression, Expression>, Exponent<Expression, Expression>>, Expression>::Specialize(simplifiedIntegrate); exponentMultCase != nullptr) {
        if (exponentMultCase->GetMostSigOp().GetMostSigOp().GetMostSigOp().Equals(exponentMultCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp())) {
            const Expression& differential = exponentMultCase->GetLeastSigOp();

            return std::make_unique<Integrate<Exponent<Expression>, Expression>>(
                Exponent<Expression>( exponentMultCase->GetMostSigOp().GetMostSigOp().GetMostSigOp(),
                    *(Add<Expression>{ exponentMultCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp(), exponentMultCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp()}.Simplify()) ), differential);
        }
    }

    // a*x * x = a*x^2
    if (auto exponentMultCase = Integrate<Multiply<Multiply<Expression>, Expression>, Expression>::Specialize(simplifiedIntegrate); exponentMultCase != nullptr) {
        if (exponentMultCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp().Equals(exponentMultCase->GetMostSigOp().GetLeastSigOp())) {
            const Expression& differential = exponentMultCase->GetLeastSigOp();

            return std::make_unique<Integrate<Multiply<Expression>, Expression>>(
                Multiply<Expression>( exponentMultCase->GetMostSigOp().GetMostSigOp().GetMostSigOp(),
                    *(Add<Expression>{ exponentMultCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp(), Real{ 1.0 } }.Simplify()) ), differential);
        }
    }

    // a*x * b*x = (a*b) * x^2
    if (auto exponentMultCase = Integrate<Multiply<Multiply<Expression, Expression>, Multiply<Expression, Expression>>, Expression>::Specialize(simplifiedIntegrate); exponentMultCase != nullptr) {
        if (exponentMultCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp().Equals(exponentMultCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp())) {
            const Expression& differential = exponentMultCase->GetLeastSigOp();

            return std::make_unique<Integrate<Multiply<Expression>, Expression>>(
                Multiply<Expression>(*(Multiply<Expression>{exponentMultCase->GetMostSigOp().GetMostSigOp().GetMostSigOp(),
                             exponentMultCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp()}.Simplify()),
                                *(Exponent<Expression>{ exponentMultCase->GetMostSigOp().GetMostSigOp().GetMostSigOp(), Real{2.0} }.Simplify())), differential);
        }
    }

    // a*x^n * x = a*x^(n + 1)
    if (auto exponentMultCase = Integrate<Multiply<Multiply<Expression, Exponent<Expression>>, Expression>, Expression>::Specialize(simplifiedIntegrate); exponentMultCase != nullptr) {
        if (exponentMultCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(exponentMultCase->GetMostSigOp().GetLeastSigOp())) {
            const Expression& differential = exponentMultCase->GetLeastSigOp();

            return std::make_unique<Integrate<Multiply<Expression>, Expression>>(
                Multiply<Expression>( exponentMultCase->GetMostSigOp().GetMostSigOp().GetMostSigOp(),
                    *(Exponent<Expression>{exponentMultCase->GetMostSigOp().GetLeastSigOp(),
                        *(Add<Expression>{ exponentMultCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp().GetMostSigOp(), Real{1.0} }.Simplify())}.Simplify()) ), differential);
        }
    }

    // a*x * x^n = a*x^(n + 1)
    if (auto exponentMultCase = Integrate<Multiply<Multiply<Expression>, Exponent<Expression>>, Expression>::Specialize(simplifiedIntegrate); exponentMultCase != nullptr) {
        if (exponentMultCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp().Equals(exponentMultCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp())) {
            const Expression& differential = exponentMultCase->GetLeastSigOp();

            return std::make_unique<Integrate<Multiply<Expression>, Expression>>(
                Multiply<Expression>( exponentMultCase->GetMostSigOp().GetMostSigOp().GetMostSigOp(),
                    *(Exponent<Expression>{ exponentMultCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp(),
                        *(Add<Expression>{ exponentMultCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp(), Real{1.0} }.Simplify()) }.Simplify()) ), differential);
        }
        if (exponentMultCase->GetMostSigOp().GetMostSigOp().GetMostSigOp().Equals(exponentMultCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp())) {
            const Expression& differential = exponentMultCase->GetLeastSigOp();

            return std::make_unique<Integrate<Multiply<Expression>, Expression>>(
                Multiply<Expression>( exponentMultCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp(),
                    *(Exponent<Expression>{ exponentMultCase->GetMostSigOp().GetMostSigOp().GetMostSigOp(),
                        *(Add{ exponentMultCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp(), Real{1.0} }.Simplify()) }.Simplify()) ), differential);
        }
    }

    // a*x^n * b*x = a*b * x^(n+1)
    if (auto liketermsCase = Integrate<Multiply<Multiply<Expression, Exponent<Expression>>, Multiply<Expression>>, Expression>::Specialize(simplifiedIntegrate); liketermsCase != nullptr) {
        if (liketermsCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(liketermsCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp())) {
            const Expression& differential = liketermsCase->GetLeastSigOp();

            return std::make_unique<Integrate<Multiply<Expression>, Expression>>(
                Multiply<Expression>( *(Multiply<Expression>{ liketermsCase->GetMostSigOp().GetMostSigOp().GetMostSigOp(), liketermsCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp() }.Simplify()),
                    *(Exponent<Expression>{ liketermsCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp(),
                        *(Add<Expression>{ liketermsCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp().GetLeastSigOp(), Real{1.0} }.Simplify()) }.Simplify()) ), differential );
        }
        if (liketermsCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(liketermsCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp())) {
            const Expression& differential = liketermsCase->GetLeastSigOp();

            return std::make_unique<Integrate<Multiply<Expression>, Expression>>(
                Multiply<Expression>( *(Multiply<Expression>{ liketermsCase->GetMostSigOp().GetMostSigOp().GetMostSigOp(), liketermsCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp()}.Simplify()),
                    *(Exponent<Expression>{ liketermsCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp(),
                        *(Add{ liketermsCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp().GetLeastSigOp(), Real{1.0} }.Simplify()) }.Simplify()) ), differential );
        }
    }

    // a*x * b*x^n = a*b * x^(n+1)
    if (auto liketermsCase = Integrate<Multiply<Multiply<Expression>, Multiply<Expression, Exponent<Expression>>>, Expression>::Specialize(simplifiedIntegrate); liketermsCase != nullptr) {
        if (liketermsCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp().Equals(liketermsCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp().GetMostSigOp())) {
            const Expression& differential = liketermsCase->GetLeastSigOp();

            return std::make_unique<Integrate<Multiply<Expression>, Expression>>(
                Multiply<Expression>( *(Multiply<Expression>{ liketermsCase->GetMostSigOp().GetMostSigOp().GetMostSigOp(), liketermsCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp() }.Simplify()),
                    *(Exponent<Expression>{ liketermsCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp(),
                        *(Add<Expression>{ liketermsCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp().GetLeastSigOp(), Real{1.0} }.Simplify()) }.Simplify()) ), differential );
        }
        if (liketermsCase->GetMostSigOp().GetMostSigOp().GetMostSigOp().Equals(liketermsCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp().GetMostSigOp())) {
            const Expression& differential = liketermsCase->GetLeastSigOp();

            return std::make_unique<Integrate<Multiply<Expression>, Expression>>(
                Multiply<Expression>( *(Multiply<Expression>{ liketermsCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp(), liketermsCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp()}.Simplify()),
                    *(Exponent<Expression>{ liketermsCase->GetMostSigOp().GetMostSigOp().GetMostSigOp(),
                        *(Add{ liketermsCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp().GetLeastSigOp(), Real{1.0} }.Simplify()) }.Simplify()) ), differential );
        }
    }

    if (auto liketermsCase = Integrate<Multiply<Multiply<Expression>, Multiply<Exponent<Expression>, Expression>>, Expression>::Specialize(simplifiedIntegrate); liketermsCase != nullptr) {
        if (liketermsCase->GetMostSigOp().GetMostSigOp().GetMostSigOp().Equals(liketermsCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().GetMostSigOp())) {
            const Expression& differential = liketermsCase->GetLeastSigOp();

            return std::make_unique<Integrate<Multiply<Expression>, Expression>>(
                Multiply<Expression>( *(Multiply<Expression>{ liketermsCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp(), liketermsCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp() }.Simplify()),
                    *(Exponent<Expression>{ liketermsCase->GetMostSigOp().GetMostSigOp().GetMostSigOp(),
                        *(Add<Expression>{ liketermsCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().GetLeastSigOp(), Real{1.0} }.Simplify()) }.Simplify()) ), differential );
        }
        if (liketermsCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp().Equals(liketermsCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().GetMostSigOp())) {
            const Expression& differential = liketermsCase->GetLeastSigOp();

            return std::make_unique<Integrate<Multiply<Expression>, Expression>>(
                Multiply<Expression>( *(Multiply<Expression>{ liketermsCase->GetMostSigOp().GetMostSigOp().GetMostSigOp(), liketermsCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp() }.Simplify()),
                    *(Exponent<Expression>{ liketermsCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp(),
                        *(Add<Expression>{ liketermsCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp().GetLeastSigOp(), Real{1.0} }.Simplify()) }.Simplify()) ), differential );
        }
    }

    if (auto liketermCase = Integrate<Multiply<Multiply<Exponent<Expression>, Expression>, Multiply<Expression>>, Expression>::Specialize(simplifiedIntegrate); liketermCase != nullptr) {
        if (liketermCase->GetMostSigOp().GetMostSigOp().GetMostSigOp().GetMostSigOp().Equals(liketermCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp()) ) {
            const Expression& differential = liketermCase->GetLeastSigOp();

            return std::make_unique<Integrate<Multiply<Expression>, Expression>>(
                Multiply<Expression>( *(Multiply<Expression>{ liketermCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp(), liketermCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp() }.Simplify()),
                    *(Exponent<Expression>{ liketermCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp(),
                        *(Add<Expression>{ liketermCase->GetMostSigOp().GetMostSigOp().GetMostSigOp().GetLeastSigOp(), Real{1.0} }.Simplify()) }) ), differential);
        }
        if (liketermCase->GetMostSigOp().GetMostSigOp().GetMostSigOp().GetMostSigOp().Equals(liketermCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp())) {
            const Expression& differential = liketermCase->GetLeastSigOp();

            return std::make_unique<Integrate<Multiply<Expression>, Expression>>(
                Multiply<Expression>( *(Multiply<Expression>{ liketermCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp(), liketermCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp() }.Simplify()),
                    *(Exponent<Expression>{ liketermCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp(),
                        *(Add<Expression>{ liketermCase->GetMostSigOp().GetMostSigOp().GetMostSigOp().GetLeastSigOp(), Real{1.0} }.Simplify()) }) ), differential);
        }
    }

    // a*x^n * x^m = a * x^(n + m)
    if (auto liketermCase = Integrate<Multiply<Multiply<Expression, Exponent<Expression>>, Exponent<Expression>>, Expression>::Specialize(simplifiedIntegrate); liketermCase != nullptr) {
        if (liketermCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(liketermCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp())) {
            const Expression& differential = liketermCase->GetLeastSigOp();

            return std::make_unique<Integrate<Multiply<Expression>, Expression>>(
                Multiply<Expression>( liketermCase->GetMostSigOp().GetMostSigOp().GetMostSigOp(),
                    *(Exponent<Expression>{ liketermCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp(),
                        *(Add<Expression>{ liketermCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp().GetLeastSigOp(), liketermCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp() }.Simplify()) }.Simplify()) ), differential);
        }
    }

    // a*x^n * b*x^n = a*b * x^(n + m)
    if (auto liketermCase = Integrate<Multiply<Multiply<Expression, Exponent<Expression>>, Multiply<Expression, Exponent<Expression>>>, Expression>::Specialize(simplifiedIntegrate); liketermCase != nullptr) {
        if (liketermCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp().GetMostSigOp().Equals(liketermCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp().GetMostSigOp())) {
            const Expression& differential = liketermCase->GetLeastSigOp();

            return std::make_unique<Integrate<Multiply<Expression>, Expression>>(
                Multiply<Expression>( *(Multiply<Expression>{ liketermCase->GetMostSigOp().GetMostSigOp().GetMostSigOp(), liketermCase->GetMostSigOp().GetLeastSigOp().GetMostSigOp() }.Simplify()),
                    *(Exponent<Expression>{ liketermCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp().GetMostSigOp(),
                        *(Add<Expression>{ liketermCase->GetMostSigOp().GetMostSigOp().GetLeastSigOp().GetLeastSigOp(), liketermCase->GetMostSigOp().GetLeastSigOp().GetLeastSigOp().GetLeastSigOp() }.Simplify()) }.Simplify()) ), differential);
        }
    }

    // For Multiply Statements


    // Divide Cases

    if ()



    // simplifies expressions and combines like terms


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

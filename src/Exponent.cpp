//
// Created by Andrew Nazareth on 9/19/23.
//

#include "Oasis/Exponent.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Util.hpp"
#include <cmath>
#include <tuple>

#ifndef M_PIl
constexpr auto M_PIl = 3.141592653589793238462643383279502884L;
#endif
namespace Oasis {

auto complexSpecialize(const Exponent<Expression>& exp) -> std::tuple<double,double,double>{
    auto specializeExp = Exponent<Add<Real,Expression>,Real>::Specialize(exp);
    std::tuple<double,double,double> failure(NAN,NAN,NAN);
    if(specializeExp==nullptr){
        return failure;
    }
    double pow = specializeExp->GetLeastSigOp().GetValue();
    const auto& base = specializeExp->GetMostSigOp();
    double realPart = base.GetMostSigOp().GetValue();
    const auto& imgPart = base.GetLeastSigOp();
    if(imgPart.Is<Imaginary>()){
        return std::make_tuple(realPart,1,pow);
    } else if (auto imgPartMult = Multiply<Real,Imaginary>::Specialize(imgPart); imgPartMult !=nullptr){
        return std::make_tuple(realPart,imgPartMult->GetMostSigOp().GetValue(),pow);
    }
    return failure;
}

Exponent<Expression>::Exponent(const Expression& base, const Expression& power)
    : BinaryExpression(base, power)
{
}

auto Exponent<Expression>::Simplify() const -> std::unique_ptr<Expression>
{
    auto simplifiedBase = mostSigOp->Simplify();
    std::unique_ptr<Expression> simplifiedPower = leastSigOp->Simplify();

    Exponent simplifiedExponent { *simplifiedBase, *simplifiedPower };

    if (auto realCase = Exponent<Real>::Specialize(simplifiedExponent); realCase != nullptr) {
        double powerValue = realCase->GetLeastSigOp().GetValue();
        double baseValue = realCase->GetMostSigOp().GetValue();
        if (baseValue >= 0) {
            return std::make_unique<Real>(std::pow(baseValue, powerValue));
        }
        double powerNumValueD = powerValue;
        double powerDenValueD = 1;
        if(powerValue == 0.5){
            powerNumValueD = 1;
            powerDenValueD = 2;
        } else if(auto PowerDiv = Divide<Expression>::Specialize(*leastSigOp); PowerDiv!=nullptr){
            auto SimplifedPowerDiv = Divide<Real>::Specialize(Divide(*PowerDiv->GetMostSigOp().Simplify(), *PowerDiv->GetLeastSigOp().Simplify()));
            if (SimplifedPowerDiv != nullptr) {
                powerNumValueD = SimplifedPowerDiv->GetMostSigOp().GetValue();
                powerDenValueD = SimplifedPowerDiv->GetLeastSigOp().GetValue();
                if (Util::isInt(powerNumValueD) && Util::isInt(powerDenValueD)) {
            long long powerNumValue = lround(powerNumValueD);
            long long powerDenValue = lround(powerDenValueD);
            long long simpBy = Util::gcf(powerNumValue, powerDenValue);
            powerDenValue /= simpBy;
            if (powerDenValue % 2 == 1) {
                return std::make_unique<Real>(-std::pow(-baseValue, powerValue));
            }
            powerNumValueD /= simpBy;
            powerDenValueD = 1.0*powerDenValue;
        }
            }
        }
        double newAbs = std::pow(-baseValue, powerValue);
        if(powerNumValueD==1&&powerDenValueD==2){
            return std::make_unique<Multiply<Real,Imaginary>>(Real(newAbs),Imaginary());
        }
        double angle = std::fmod((M_PIl * powerNumValueD),(2*M_PIl));
        if(angle>M_PIl){
            angle-=2*M_PIl;
        }
        angle/=powerDenValueD;
        Real newReal = Real(newAbs*std::cos(angle));
        Real newImg = Real(newAbs*std::sin(angle));
        return std::make_unique<Add<Real,Multiply<Real,Imaginary>>>(newReal,Multiply(newImg,Imaginary()));
    }
    
    if (auto multCase = Exponent<Multiply<Expression>,Expression>::Specialize(simplifiedExponent); multCase != nullptr){
        auto& base = multCase->GetMostSigOp();
        auto& left = base.GetMostSigOp();
        auto& right = base.GetLeastSigOp();
        auto& exp = multCase->GetLeastSigOp();
        return Multiply(Exponent(left,exp),Exponent(right,exp)).Simplify();
    }
    if (auto specialPower = Exponent<Expression, Real>::Specialize(simplifiedExponent); specialPower != nullptr) {
        const Real& power = specialPower->GetLeastSigOp();
        if (power.GetValue() == 1.0) {
            return specialPower->GetMostSigOp().Copy();
        }
        if (power.GetValue() == 0.0) {
            return std::make_unique<Real>(1.0);
        }
    }

    if (auto specialBase = Exponent<Real, Expression>::Specialize(simplifiedExponent); specialBase != nullptr) {
        const Real& base = specialBase->GetMostSigOp();
        if(base.GetValue() == 1.0){
            return std::make_unique<Real>(1.0);
        } 
        if (base.GetValue() == 0.0){
            return std::make_unique<Real>(0.0);
        }
    }

    if (auto ImgCase = Exponent<Imaginary, Real>::Specialize(simplifiedExponent); ImgCase != nullptr) {
        const auto power = std::fmod((ImgCase->GetLeastSigOp()).GetValue(), 4);
        if (power == 1) {
            return std::make_unique<Imaginary>();
        } else if (power == 2) {
            return std::make_unique<Real>(-1);
        } else if (power == 0) {
            return std::make_unique<Real>(1);
        } else if (power == 3) {
            return std::make_unique<Multiply<Real, Imaginary>>(Real { -1 }, Imaginary {});
        } else {
            auto angle = M_PIl/2*(power);
            return std::make_unique<Add<Real,Multiply<Real,Imaginary> > >(Real(std::cos(angle)),Multiply(Real(std::sin(angle)),Imaginary()));
        }
    }

    if (auto ImgCase = Exponent<Multiply<Real, Expression>, Real>::Specialize(simplifiedExponent); ImgCase != nullptr) {
        if (ImgCase->GetMostSigOp().GetMostSigOp().GetValue() < 0 && ImgCase->GetLeastSigOp().GetValue() == 0.5) {
            return std::make_unique<Multiply<Expression>>(
                Multiply<Expression> { Real { pow(std::abs(ImgCase->GetMostSigOp().GetMostSigOp().GetValue()), 0.5) },
                    Exponent<Expression> { ImgCase->GetMostSigOp().GetLeastSigOp(), Real { 0.5 } } },
                Imaginary {});
        }
    }

    if (auto complexCase = complexSpecialize(simplifiedExponent); !std::isnan(std::get<0>(complexCase))) {
        double realPart = std::get<0>(complexCase);
        double imgPart = std::get<1>(complexCase);
        double power = std::get<2>(complexCase);
        double powerNumValueD = power;
        double powerDenValueD = 1;
        if(power == 0.5){
            powerNumValueD = 1;
            powerDenValueD = 2;
        } else if(auto PowerDiv = Divide<Expression>::Specialize(*leastSigOp); PowerDiv!=nullptr){
            auto SimplifedPowerDiv = Divide<Real>::Specialize(Divide(*PowerDiv->GetMostSigOp().Simplify(), *PowerDiv->GetLeastSigOp().Simplify()));
            if (SimplifedPowerDiv != nullptr) {
                powerNumValueD = SimplifedPowerDiv->GetMostSigOp().GetValue();
                powerDenValueD = SimplifedPowerDiv->GetLeastSigOp().GetValue();
                if (Util::isInt(powerNumValueD) && Util::isInt(powerDenValueD)) {
                    long long powerNumValue = lround(powerNumValueD);
                    long long powerDenValue = lround(powerDenValueD);
                    long long simpBy = Util::gcf(powerNumValue, powerDenValue);
                    powerNumValueD /= simpBy;
                    powerDenValueD /= simpBy;
                }
            }
        }
        double absSquare = realPart*realPart+imgPart*imgPart;
        double newAbs = std::pow(absSquare,power/2.0);
        double angle = std::atan2(imgPart,realPart);
        angle*=powerNumValueD;
        angle = std::fmod(angle,(2*M_PIl));
        if(angle>M_PIl){
            angle-=2*M_PIl;
        }
        angle/=powerDenValueD;
        Real newReal = Real(newAbs*std::cos(angle));
        Real newImg = Real(newAbs*std::sin(angle));
        return std::make_unique<Add<Real,Multiply<Real,Imaginary>>>(newReal,Multiply(newImg,Imaginary()));
    }

    if (auto expExpCase = Exponent<Exponent<Expression, Expression>, Expression>::Specialize(simplifiedExponent); expExpCase != nullptr) {
        return std::make_unique<Exponent<Expression>>(expExpCase->GetMostSigOp().GetMostSigOp(),
            *(Multiply { expExpCase->GetMostSigOp().GetLeastSigOp(), expExpCase->GetLeastSigOp() }.Simplify()));
    }

    // a^log[a](x) = x - maybe add domain stuff (should only be defined for x >= 0)
    if (auto logCase = Exponent<Expression, Log<Expression, Expression>>::Specialize(simplifiedExponent); logCase != nullptr) {
        if (logCase->GetMostSigOp().Equals(logCase->GetLeastSigOp().GetMostSigOp())) {
            return Expression::Specialize(logCase->GetLeastSigOp().GetLeastSigOp());
        }
    }

    return simplifiedExponent.Copy();
}

auto Exponent<Expression>::ToString() const -> std::string
{
    return fmt::format("({}^{})", mostSigOp->ToString(), leastSigOp->ToString());
}

auto Exponent<Expression>::Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression>
{
    std::unique_ptr<Expression> simplifiedBase, simplifiedPower;

    tf::Task baseSimplifyTask = subflow.emplace([this, &simplifiedBase](tf::Subflow& sbf) {
        if (!mostSigOp) {
            return;
        }
        simplifiedBase = mostSigOp->Simplify(sbf);
    });

    tf::Task powerSimplifyTask = subflow.emplace([this, &simplifiedPower](tf::Subflow& sbf) {
        if (!leastSigOp) {
            return;
        }

        simplifiedPower = leastSigOp->Simplify(sbf);
    });

    Exponent simplifiedExponent;

    tf::Task simplifyTask = subflow.emplace([&simplifiedExponent, &simplifiedBase, &simplifiedPower](tf::Subflow&) {
        if (simplifiedPower) {
            simplifiedExponent.SetMostSigOp(*simplifiedBase);
        }

        if (simplifiedPower) {
            simplifiedExponent.SetLeastSigOp(*simplifiedPower);
        }
    });

    simplifyTask.succeed(baseSimplifyTask, powerSimplifyTask);

    std::unique_ptr<Exponent<Real>> realCase;

    tf::Task realCaseTask = subflow.emplace([&simplifiedExponent, &realCase](tf::Subflow& sbf) {
        realCase = Exponent<Real>::Specialize(simplifiedExponent, sbf);
    });

    simplifyTask.precede(realCaseTask);

    subflow.join();

    if (realCase) {
        const Real& base = realCase->GetMostSigOp();
        const Real& power = realCase->GetLeastSigOp();

        return std::make_unique<Real>(pow(base.GetValue(), power.GetValue()));
    }

    return simplifiedExponent.Copy();
}

auto Exponent<Expression>::Specialize(const Oasis::Expression& other) -> std::unique_ptr<Exponent<Expression, Expression>>
{
    if (!other.Is<Oasis::Exponent>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize();
    return std::make_unique<Exponent>(dynamic_cast<const Exponent&>(*otherGeneralized));
}

auto Exponent<Expression>::Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Exponent>
{
    if (!other.Is<Oasis::Exponent>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize(subflow);
    return std::make_unique<Exponent>(dynamic_cast<const Exponent&>(*otherGeneralized));
}

} // Oasis

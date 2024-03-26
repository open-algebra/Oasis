#include "taskflow/taskflow.hpp"

#include "Oasis/Expression.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Util.hpp"
#include "Oasis/Variable.hpp"

std::vector<long long> getAllFactors(long long n)
{
    std::vector<long long> answer;
    for (long long i = 1; i * i <= n; i++) {
        if (n % i == 0) {
            answer.push_back(i);
        }
    }
    if (std::abs(n) != 1) {
        answer.push_back(std::abs(n));
    }
    return answer;
}

namespace Oasis {

auto Expression::FindZeros() const -> std::vector<std::unique_ptr<Expression>>
{
    std::vector<std::unique_ptr<Expression>> results;
    std::vector<std::unique_ptr<Expression>> termsE;
    if (auto addCase = Add<Expression>::Specialize(*this); addCase != nullptr) {
        addCase->Flatten(termsE);
    } else {
        termsE.push_back(Copy());
    }
    std::string varName = "x";
    std::vector<std::unique_ptr<Expression>> posCoefficents;
    std::vector<std::unique_ptr<Expression>> negCoefficents;
    for (const auto& i : termsE) {
        std::unique_ptr<Expression> coefficent;
        std::string variableName;
        double exponent;
        if (auto variableCase = Variable::Specialize(*i); variableCase != nullptr) {
            coefficent = Real(1).Copy();
            variableName = variableCase->GetName();
            exponent = 1;
            if (variableName != varName) {
                coefficent = i->Copy();
                variableName = varName;
                exponent = 0;
            }
        } else if (auto expCase = Exponent<Variable, Real>::Specialize(*i); expCase != nullptr) {
            coefficent = Real(1).Copy();
            variableName = expCase->GetMostSigOp().GetName();
            exponent = expCase->GetLeastSigOp().GetValue();
        } else if (auto prodCase = Multiply<Expression, Variable>::Specialize(*i); prodCase != nullptr && prodCase->GetLeastSigOp().GetName() == varName) {
            coefficent = prodCase->GetMostSigOp().Copy();
            variableName = prodCase->GetLeastSigOp().GetName();
            exponent = 1;
        } else if (auto prodExpCase = Multiply<Expression, Exponent<Variable, Real>>::Specialize(*i); prodExpCase != nullptr) {
            coefficent = prodExpCase->GetMostSigOp().Copy();
            variableName = prodExpCase->GetLeastSigOp().GetMostSigOp().GetName();
            exponent = prodExpCase->GetLeastSigOp().GetLeastSigOp().GetValue();
        } else if (auto divCase = Divide<Expression, Variable>::Specialize(*i); divCase != nullptr) {
            coefficent = divCase->GetMostSigOp().Copy();
            variableName = divCase->GetLeastSigOp().GetName();
            exponent = -1;
        } else if (auto divExpCase = Divide<Expression, Exponent<Variable, Real>>::Specialize(*i); divExpCase != nullptr) {
            coefficent = divExpCase->GetMostSigOp().Copy();
            variableName = divExpCase->GetLeastSigOp().GetMostSigOp().GetName();
            exponent = -divExpCase->GetLeastSigOp().GetLeastSigOp().GetValue();
        } else {
            coefficent = i->Copy();
            variableName = varName;
            exponent = 0;
        }
        if (varName == "") {
            varName = variableName;
        }
        if (!Util::isInt(exponent) || varName != variableName) {
            return {};
        }
        if (exponent >= 0) {
            while (posCoefficents.size() <= exponent) {
                posCoefficents.push_back(Real(0).Copy());
            }
            posCoefficents[lround(exponent)] = Add<Expression>(*coefficent, *posCoefficents[lround(exponent)]).Copy();
        } else {
            exponent *= -1;
            while (negCoefficents.size() <= exponent) {
                negCoefficents.push_back(Real(0).Copy());
            }
            negCoefficents[lround(exponent)] = Add<Expression>(*coefficent, *negCoefficents[lround(exponent)]).Copy();
        }
    }
    while (negCoefficents.size() > 0 && Real::Specialize(*negCoefficents.back()) != nullptr && Real::Specialize(*negCoefficents.back())->GetValue() == 0) {
        negCoefficents.pop_back();
    }
    while (posCoefficents.size() > 0 && Real::Specialize(*posCoefficents.back()) != nullptr && Real::Specialize(*posCoefficents.back())->GetValue() == 0) {
        posCoefficents.pop_back();
    }
    std::vector<std::unique_ptr<Expression>> coefficents;
    for (size_t i = negCoefficents.size(); i > 1; i--) {
        coefficents.push_back(negCoefficents[i - 1]->Simplify());
    }
    for (const std::unique_ptr<Expression>& i : posCoefficents) {
        coefficents.push_back(i->Simplify());
    }
    if (coefficents.size() <= 1) {
        return {};
    }
    std::vector<long long> termsC;
    for (auto& i : coefficents) {
        auto realCase = Real::Specialize(*i);
        if (realCase == nullptr) {
            break;
        }
        double value = realCase->GetValue();
        if (!Util::isInt(value)) {
            break;
        } else {
            termsC.push_back(lround(value));
        }
    }
    if (termsC.size() == coefficents.size()) {
        std::reverse(termsC.begin(), termsC.end());
        for (auto pv : getAllFactors(termsC.back())) {
            for (auto qv : getAllFactors(termsC.front())) {
                if (Util::gcf(pv, qv) == 1) {
                    for (long long sign : { -1, 1 }) {
                        bool doAdd = true;
                        while (true) {
                            long long mpv = pv * sign;
                            std::vector<long long> newTermsC;
                            long long h = 0;
                            for (long long i : termsC) {
                                h *= mpv;
                                if (h % qv != 0) {
                                    break;
                                }
                                h /= qv;
                                h += i;
                                newTermsC.push_back(h);
                            }
                            if (newTermsC.size() == termsC.size() && newTermsC.back() == 0) {
                                termsC = newTermsC;
                                if (doAdd) {
                                    results.push_back(std::make_unique<Divide<Real>>(Real(1.0 * mpv), Real(1.0 * qv)));
                                    doAdd = false;
                                }
                                do {
                                    termsC.pop_back();
                                } while (termsC.back() == 0);
                                if (termsC.size() <= 1) {
                                    break;
                                }
                            } else {
                                break;
                            }
                        }
                    }
                }
                if (termsC.size() <= 1) {
                    break;
                }
            }
            if (termsC.size() <= 1) {
                break;
            }
        }
        coefficents.clear();
        std::reverse(termsC.begin(), termsC.end());
        for (auto i : termsC) {
            coefficents.push_back(Real(i * 1.0).Copy());
        }
    }
    const Real n1(-1);
    const Real n2(-2);
    const Real n3(-3);
    const Real n4(-4);
    const Real p1(1);
    const Real p2(2);
    const Real p3(3);
    const Real p4(4);
    const Divide half(p1, p2);
    const Divide third(p1, p3);
    if (coefficents.size() == 2) {
        results.push_back(Divide(Multiply(n1, *coefficents[0]), *coefficents[1]).Simplify());
    } else if (coefficents.size() == 3) {
        auto& a = coefficents[2];
        auto& b = coefficents[1];
        auto& c = coefficents[0];
        auto negB = Multiply(n1, *b).Simplify();
        auto sqrt = Exponent(*Add(Multiply(*b, *b), Multiply(n4, Multiply(*a, *c))).Simplify(), half).Copy();
        auto twoA = Multiply(p2, *a).Simplify();
        results.push_back(Divide(Add(*negB, *sqrt), *twoA).Copy());
        results.push_back(Divide(Subtract(*negB, *sqrt), *twoA).Copy());
    } else if (coefficents.size() == 4) {
        Divide p(*coefficents[2], *coefficents[3]);
        Divide q(*coefficents[1], *coefficents[3]);
        Divide r(*coefficents[0], *coefficents[3]);
        Multiply base(p3, Exponent(p2, third));
        Add aLeft(Multiply(n2, Exponent(p, p3)), Add(Multiply(Real(9), Multiply(p, q)), Multiply(Real(-27), r)));
        Add aRight(Add(
                       Multiply(n1, Exponent(Multiply(p, q), p2)),
                       Multiply(p4, Exponent(q, p3))),
            Add(Add(
                    Multiply(p4, Multiply(Exponent(p, p3), r)),
                    Multiply(Multiply(Real(-18), p), Multiply(q, r))),
                Multiply(Real(27), Exponent(r, p2))));
        Exponent aTop(Add(aLeft, Multiply(Multiply(p3, Exponent(p3, half)), Exponent(aRight, half))), third);
        auto a = Divide(aTop, base).Simplify();
        auto b = Divide(Subtract(Multiply(p3, q), Exponent(p, p2)), Multiply(Real(9), *a)).Simplify();
        auto rotaterLeft = Divide(Add(n1, Exponent(n3, half)), p2).Simplify();
        auto rotaterRight = Divide(Subtract(n1, Exponent(n3, half)), p2).Simplify();
        for (int i = 0; i < 3; i++) {
            Real iR = Real(i);
            Add totalExpression(Divide(p, n3), Subtract(Multiply(Exponent(*rotaterLeft, iR), *a), Multiply(Exponent(*rotaterRight, iR), *b)));
            results.push_back(totalExpression.Copy());
        }
    } else if (coefficents.size() == 5) {
        auto& a = coefficents[4];
        auto& b = coefficents[3];
        auto& c = coefficents[2];
        auto& d = coefficents[1];
        auto& e = coefficents[0];
        // 2c^3-8bc+27ad^2+27b^2e-27ace
        std::vector<std::unique_ptr<Expression>> addVector;
        addVector.push_back(Multiply(p2, Exponent(*c, p3)).Copy());
        addVector.push_back(Multiply(Multiply(Real(-9), *b), Multiply(*c, *d)).Copy());
        addVector.push_back(Multiply(Multiply(Real(27), *a), Exponent(*d, p2)).Copy());
        addVector.push_back(Multiply(Multiply(Real(27), Exponent(*b, p2)), *e).Copy());
        addVector.push_back(Multiply(Multiply(Real(-72), *a), Multiply(*c, *e)).Copy());
        Add param1 = *Add<Expression>::Specialize(*BuildFromVector<Add>(addVector));

        // param1 + sqrt(-4(c^2-3bd+12ae)^3+param1^2)
        addVector.clear();
        addVector.push_back(Exponent(*c, p2).Copy());
        addVector.push_back(Multiply(n3, Multiply(*b, *d)).Copy());
        addVector.push_back(Multiply(Real(12), Multiply(*a, *e)).Copy());
        Add param2(param1, Exponent(Add(Multiply(n4, Exponent(*BuildFromVector<Add>(addVector), p3)), Exponent(param1, p2)), half));

        //(c^2-3bd+12ae)/(3a*cbrt(param2/2))+cbrt(param2/2)/(3a)
        addVector.clear();
        addVector.push_back(Exponent(*c, p2).Copy());
        addVector.push_back(Multiply(n3, Multiply(*b, *d)).Copy());
        addVector.push_back(Multiply(Real(12), Multiply(*a, *e)).Copy());
        Add param3(Divide(*BuildFromVector<Add>(addVector), Multiply(p3, Multiply(*a, Exponent(Divide(param2, p2), third)))), Divide(Exponent(Divide(param2, p2), third), Multiply(p3, *a)));

        // sqrt(b^2/(4a^2)-(2c)/(3a)+param3)
        addVector.clear();
        addVector.push_back(Divide(Exponent(*b, p2), Multiply(p4, Exponent(*a, p2))).Copy());
        addVector.push_back(Divide(Multiply(n2, *c), Multiply(p3, *a)).Copy());
        addVector.push_back(param3.Copy());
        Exponent param4(*BuildFromVector<Add>(addVector), half);

        // b^2/(2a^2)-4c/(3a)-param3
        addVector.clear();
        addVector.push_back(Divide(Exponent(*b, p2), Multiply(p2, Exponent(*a, p2))).Copy());
        addVector.push_back(Divide(Multiply(n4, *c), Multiply(p3, *a)).Copy());
        addVector.push_back(Multiply(n1, param3).Copy());
        Add param5 = *Add<Expression>::Specialize(*BuildFromVector<Add>(addVector));

        //(-b^3/a^3+4bc/a^2-8d/a)/(4*param4)
        addVector.clear();
        addVector.push_back(Multiply(n1, Divide(Exponent(*b, p3), Exponent(*a, p3))).Copy());
        addVector.push_back(Divide(Multiply(Multiply(p4, *b), *c), Exponent(*a, p2)).Copy());
        addVector.push_back(Divide(Multiply(Real(-8), *d), *a).Copy());
        Divide param6(*BuildFromVector<Add>(addVector), Multiply(p4, param4));
        const std::vector<Real> plusMinus = { n1, p1 };
        Divide nbo4a(*b, Multiply(n4, *a));
        for (Real pm1 : plusMinus) {
            Divide rootExp(Exponent(Add(param5, Multiply(pm1, param6)), half), p2);
            for (Real pm2 : plusMinus) {
                results.push_back(Add(nbo4a, Add(Multiply(pm1, Divide(param4, p2)), Multiply(pm2, rootExp))).Copy());
            }
        }
    }
    return results;
}

auto Expression::GetCategory() const -> uint32_t
{
    return 0;
}

auto Expression::GetType() const -> ExpressionType
{
    return ExpressionType::None;
}

auto Expression::Generalize() const -> std::unique_ptr<Expression>
{
    return Copy();
}

auto Expression::Generalize(tf::Subflow& subflow) const -> std::unique_ptr<Expression>
{
    return Copy(subflow);
}

auto Expression::Specialize(const Expression& other) -> std::unique_ptr<Expression>
{
    return other.Copy();
}

auto Expression::Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Expression>
{
    return other.Copy(subflow);
}

auto Expression::Simplify() const -> std::unique_ptr<Expression>
{
    return Copy();
}

auto Expression::Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression>
{
    return Copy(subflow);
}

auto Expression::SimplifyAsync() const -> std::unique_ptr<Expression>
{
    static tf::Executor executor;
    tf::Taskflow taskflow;

    std::unique_ptr<Expression> simplifiedExpression;

    taskflow.emplace([this, &simplifiedExpression](tf::Subflow& subflow) {
        simplifiedExpression = Simplify(subflow);
    });

    executor.run(taskflow).wait();
    return simplifiedExpression;
}

auto Expression::StructurallyEquivalentAsync(const Expression& other) const -> bool
{
    static tf::Executor executor;
    tf::Taskflow taskflow;

    bool equivalent = false;

    taskflow.emplace([&equivalent, this, &other](tf::Subflow& subflow) {
        equivalent = StructurallyEquivalent(other, subflow);
    });

    executor.run(taskflow).wait();
    return equivalent;
}

} // namespace Oasis
std::unique_ptr<Oasis::Expression> operator+(const std::unique_ptr<Oasis::Expression>& lhs, const std::unique_ptr<Oasis::Expression>& rhs)
{
    return Oasis::Add { *lhs, *rhs }.Simplify();
}
std::unique_ptr<Oasis::Expression> operator-(const std::unique_ptr<Oasis::Expression>& lhs, const std::unique_ptr<Oasis::Expression>& rhs)
{
    return Oasis::Subtract { *lhs, *rhs }.Simplify();
}
std::unique_ptr<Oasis::Expression> operator*(const std::unique_ptr<Oasis::Expression>& lhs, const std::unique_ptr<Oasis::Expression>& rhs)
{
    return Oasis::Multiply { *lhs, *rhs }.Simplify();
}
std::unique_ptr<Oasis::Expression> operator/(const std::unique_ptr<Oasis::Expression>& lhs, const std::unique_ptr<Oasis::Expression>& rhs)
{
    return Oasis::Divide { *lhs, *rhs }.Simplify();
}

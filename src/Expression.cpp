#include <cmath>

#include <Oasis/Add.hpp>
#include <Oasis/Divide.hpp>
#include <Oasis/Exponent.hpp>
#include <Oasis/Integral.hpp>
#include <Oasis/Multiply.hpp>
#include <Oasis/RecursiveCast.hpp>
#include <Oasis/Subtract.hpp>
#include <Oasis/Variable.hpp>

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

long long gcf(long long a, long long b)
{
    if (b > a) {
        std::swap(a, b);
    }
    while (1) {
        a %= b;
        if (a == 0) {
            return b;
        }
        b %= a;
        if (b == 0) {
            return a;
        }
    }
}

namespace Oasis {

// currently only supports polynomials of one variable.
/**
 * The FindZeros function finds all rational zeros of a polynomial. Currently assumes an expression of the form a+bx+cx^2+dx^3+... where a, b, c, d are a integers.
 *
 * @tparam origonalExpresion The expression for which all the factors will be found.
 */
auto Expression::FindZeros() const -> std::vector<std::unique_ptr<Expression>>
{
    std::vector<std::unique_ptr<Expression>> results;
    std::vector<std::unique_ptr<Expression>> termsE;
    if (auto addCase = RecursiveCast<Add<Expression>>(*this); addCase != nullptr) {
        addCase->Flatten(termsE);
    } else {
        termsE.push_back(Copy());
    }
    std::string varName = "";
    std::vector<std::unique_ptr<Expression>> posCoefficents;
    std::vector<std::unique_ptr<Expression>> negCoefficents;
    for (const auto& i : termsE) {
        std::unique_ptr<Expression> coefficent;
        std::string variableName;
        double exponent;
        if (auto variableCase = RecursiveCast<Variable>(*i); variableCase != nullptr) {
            coefficent = Real(1).Copy();
            variableName = variableCase->GetName();
            exponent = 1;
        } else if (auto expCase = RecursiveCast<Exponent<Variable, Real>>(*i); expCase != nullptr) {
            coefficent = Real(1).Copy();
            variableName = expCase->GetMostSigOp().GetName();
            exponent = expCase->GetLeastSigOp().GetValue();
        } else if (auto prodCase = RecursiveCast<Multiply<Expression, Variable>>(*i); prodCase != nullptr) {
            coefficent = prodCase->GetMostSigOp().Copy();
            variableName = prodCase->GetLeastSigOp().GetName();
            exponent = 1;
        } else if (auto prodExpCase = RecursiveCast<Multiply<Expression, Exponent<Variable, Real>>>(*i); prodExpCase != nullptr) {
            coefficent = prodExpCase->GetMostSigOp().Copy();
            variableName = prodExpCase->GetLeastSigOp().GetMostSigOp().GetName();
            exponent = prodExpCase->GetLeastSigOp().GetLeastSigOp().GetValue();
        } else if (auto divCase = RecursiveCast<Divide<Expression, Variable>>(*i); divCase != nullptr) {
            coefficent = divCase->GetMostSigOp().Copy();
            variableName = divCase->GetLeastSigOp().GetName();
            exponent = -1;
        } else if (auto divExpCase = RecursiveCast<Divide<Expression, Exponent<Variable, Real>>>(*i); divExpCase != nullptr) {
            coefficent = divExpCase->GetMostSigOp().Copy();
            variableName = divExpCase->GetLeastSigOp().GetMostSigOp().GetName();
            exponent = -divExpCase->GetLeastSigOp().GetLeastSigOp().GetValue();
        } else {
            coefficent = i->Copy();
            variableName = varName;
            exponent = 0;
        }
        if (varName.empty()) {
            varName = variableName;
        }
        if (exponent != std::round(exponent) || varName != variableName) {
            return {};
        }
        long flooredExponent = std::lround(exponent);
        if (exponent >= 0) {
            while (posCoefficents.size() <= exponent) {
                posCoefficents.push_back(Real(0).Copy());
            }
            posCoefficents[flooredExponent] = Add<Expression>(*coefficent, *posCoefficents[flooredExponent]).Copy();
        } else {
            exponent *= -1;
            while (negCoefficents.size() <= exponent) {
                negCoefficents.push_back(Real(0).Copy());
            }
            negCoefficents[flooredExponent] = Add<Expression>(*coefficent, *negCoefficents[flooredExponent]).Copy();
        }
    }
    while (negCoefficents.size() > 0 && RecursiveCast<Real>(*negCoefficents.back()) != nullptr && RecursiveCast<Real>(*negCoefficents.back())->GetValue() == 0) {
        negCoefficents.pop_back();
    }
    while (posCoefficents.size() > 0 && RecursiveCast<Real>(*posCoefficents.back()) != nullptr && RecursiveCast<Real>(*posCoefficents.back())->GetValue() == 0) {
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
        auto realCase = RecursiveCast<Real>(*i);
        if (realCase == nullptr) {
            break;
        }
        double value = realCase->GetValue();
        if (value != std::round(value)) {
            break;
        } else {
            termsC.push_back(lround(value));
        }
    }
    if (termsC.size() == coefficents.size()) {
        std::reverse(termsC.begin(), termsC.end());
        for (auto pv : getAllFactors(termsC.back())) {
            for (auto qv : getAllFactors(termsC.front())) {
                if (gcf(pv, qv) == 1) {
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
    if (coefficents.size() == 2) {
        results.push_back(Divide(Multiply(Real(-1), *coefficents[0]), *coefficents[1]).Simplify());
    } else if (coefficents.size() == 3) {
        auto& a = coefficents[2];
        auto& b = coefficents[1];
        auto& c = coefficents[0];
        auto negB = Multiply(Real(-1.0), *b).Simplify();
        auto sqrt = Exponent(*Add(Multiply(*b, *b), Multiply(Real(-4), Multiply(*a, *c))).Simplify(), Divide(Real(1), Real(2))).Copy();
        auto twoA = Multiply(Real(2), *a).Simplify();
        results.push_back(Divide(Add(*negB, *sqrt), *twoA).Copy());
        results.push_back(Divide(Subtract(*negB, *sqrt), *twoA).Copy());
    }
    return results;
}

auto Expression::GetCategory() const -> uint32_t
{
    return 0;
}
auto Expression::Differentiate(const Expression&) const -> std::unique_ptr<Expression>
{
    return Copy();
}
auto Expression::GetType() const -> ExpressionType
{
    return ExpressionType::None;
}

auto Expression::Generalize() const -> std::unique_ptr<Expression>
{
    return Copy();
}

auto Expression::Integrate(const Expression& variable) const -> std::unique_ptr<Expression>
{
    Integral<Expression, Expression> integral { *(this->Copy()), *(variable.Copy()) };

    return integral.Copy();
}

auto Expression::IntegrateWithBounds(const Expression& variable, const Expression&, const Expression&) -> std::unique_ptr<Expression>
{
    Integral<Expression, Expression> integral { *(this->Copy()), *(variable.Copy()) };

    return integral.Copy();
}

auto Expression::Simplify() const -> std::unique_ptr<Expression>
{
    return Copy();
}

} // namespace Oasis
std::unique_ptr<Oasis::Expression> operator+(const std::unique_ptr<Oasis::Expression>& lhs, const std::unique_ptr<Oasis::Expression>& rhs)
{
    Oasis::SimplifyVisitor sV {};
    auto e = Oasis::Add { *lhs, *rhs };
    auto s = e.Accept(sV);
    if (!s) {
        return e.Generalize();
    }
    return std::move(s).value();
}
std::unique_ptr<Oasis::Expression> operator-(const std::unique_ptr<Oasis::Expression>& lhs, const std::unique_ptr<Oasis::Expression>& rhs)
{
    Oasis::SimplifyVisitor sV {};
    auto e = Oasis::Subtract { *lhs, *rhs };
    auto s = e.Accept(sV);
    if (!s) {
        return e.Generalize();
    }
    return std::move(s).value();
}
std::unique_ptr<Oasis::Expression> operator*(const std::unique_ptr<Oasis::Expression>& lhs, const std::unique_ptr<Oasis::Expression>& rhs)
{
    Oasis::SimplifyVisitor sV {};
    auto e = Oasis::Multiply { *lhs, *rhs };
    auto s = e.Accept(sV);
    if (!s) {
        return e.Generalize();
    }
    return std::move(s).value();
}
std::unique_ptr<Oasis::Expression> operator/(const std::unique_ptr<Oasis::Expression>& lhs, const std::unique_ptr<Oasis::Expression>& rhs)
{
    Oasis::SimplifyVisitor sV {};
    auto e = Oasis::Divide { *lhs, *rhs };
    auto s = e.Accept(sV);
    if (!s) {
        return e.Generalize();
    }
    return std::move(s).value();
}

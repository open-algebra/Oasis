#include "taskflow/taskflow.hpp"

#include "Oasis/Expression.hpp"

#include <Oasis/Add.hpp>
#include <Oasis/Divide.hpp>
#include <Oasis/Exponent.hpp>
#include <Oasis/Multiply.hpp>
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
    if (auto addCase = Add<Expression>::Specialize(*this); addCase != nullptr) {
        addCase->Flatten(termsE);
    } else {
        termsE.push_back(Copy());
    }
    std::string varName = "";
    std::vector<long long> termsC;
    for (const auto& i : termsE) {
        double coefficent;
        std::string variableName;
        double exponent;
        if (auto variableCase = Variable::Specialize(*i); variableCase != nullptr) {
            coefficent = 1;
            variableName = variableCase->GetName();
            exponent = 1;
        } else if (auto expCase = Exponent<Variable, Real>::Specialize(*i); expCase != nullptr) {
            coefficent = 1;
            variableName = expCase->GetMostSigOp().GetName();
            exponent = expCase->GetLeastSigOp().GetValue();
        } else if (auto prodCase = Multiply<Real, Variable>::Specialize(*i); prodCase != nullptr) {
            coefficent = prodCase->GetMostSigOp().GetValue();
            variableName = prodCase->GetLeastSigOp().GetName();
            exponent = 1;
        } else if (auto prodExpCase = Multiply<Real, Exponent<Variable, Real>>::Specialize(*i); prodExpCase != nullptr) {
            coefficent = prodExpCase->GetMostSigOp().GetValue();
            variableName = prodExpCase->GetLeastSigOp().GetMostSigOp().GetName();
            exponent = prodExpCase->GetLeastSigOp().GetLeastSigOp().GetValue();
        } else if (auto realCase = Real::Specialize(*i); realCase != nullptr) {
            coefficent = realCase->GetValue();
            variableName = varName;
            exponent = 0;
        } else {
            return {};
        }
        if (varName == "") {
            varName = variableName;
        }
        if (coefficent != round(coefficent) || exponent != round(exponent) || varName != variableName || exponent < 0) {
            return {};
        }
        if (termsC.size() <= exponent) {
            termsC.resize(lround(exponent) + 1, 0);
        }
        termsC[lround(exponent)] += lround(coefficent);
    }
    while (termsC.back() == 0) {
        termsC.pop_back();
    }
    std::reverse(termsC.begin(), termsC.end());
    std::vector<long long> q = getAllFactors(termsC.front());
    std::vector<long long> p = getAllFactors(termsC.back());
    for (auto pv : p) {
        for (auto qv : q) {
            if (gcf(pv, qv) == 1) {
                for (long long sign : { -1, 1 }) {
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
                        results.push_back(std::make_unique<Divide<Real>>(Real(mpv), Real(qv)));
                        do {
                            termsC.pop_back();
                        } while (termsC.back() == 0);
                        if (termsC.size() == 0) {
                            return results;
                        }
                    }
                }
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

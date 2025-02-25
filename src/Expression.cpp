#include <cmath>

#include <Oasis/Add.hpp>
#include <Oasis/Divide.hpp>
#include <Oasis/Exponent.hpp>
#include <Oasis/Integral.hpp>
#include <Oasis/Multiply.hpp>
#include <Oasis/RecursiveCast.hpp>
#include <Oasis/Subtract.hpp>
#include <Oasis/Variable.hpp>
#include <iostream>
#include <set>
#include <numeric>

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
    std::cout << "start of the function" << std::endl;
    std::vector<std::unique_ptr<Expression>> results;
    std::vector<std::unique_ptr<Expression>> termsE;
    if (auto subCase = RecursiveCast<Subtract<Expression>>(*this); subCase != nullptr) {
        // Check for x² - n pattern
        if (auto leftTerm = RecursiveCast<Exponent<Variable, Real>>(subCase->GetMostSigOp());
            leftTerm != nullptr) {
            if (leftTerm->GetLeastSigOp().GetValue() == 2) {
                if (auto rightTerm = RecursiveCast<Real>(subCase->GetLeastSigOp());
                    rightTerm != nullptr) {

                    double n = rightTerm->GetValue();
                    double sqrtN = std::sqrt(n);

                    if (n > 0 && sqrtN == std::floor(sqrtN)) {
                        // Instead of creating Real values directly, create proper Binary Expressions
                        // For positive root: n^(1/2)
                        auto posRoot = std::make_unique<Divide<Expression>>(
                            Real(sqrtN),     // MostSigOp
                            Real(1)  // LeastSigOp
                        );

                        // For negative root: -n^(1/2)
                        auto negRoot = std::make_unique<Divide<Expression>>(
                            Real(-sqrtN),    // MostSigOp
                            Real(1)  // LeastSigOp
                        );

                        results.push_back(std::move(posRoot));
                        results.push_back(std::move(negRoot));
                        return results;
                    }
                }
            }
        }
    }
    if (auto addCase = RecursiveCast<Add<Expression>>(*this); addCase != nullptr) {
        addCase->Flatten(termsE);
    } else if (auto subCase = RecursiveCast<Subtract<Expression>>(*this); subCase != nullptr) {
        // Handle subtraction by converting to addition
        termsE.push_back(subCase->GetMostSigOp().Copy());  // First term
        // Add negative of second term
        auto negTerm = Multiply(Real(-1), subCase->GetLeastSigOp()).Copy();
        termsE.push_back(std::move(negTerm));
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
    std::cout << "posCoefficents size: " << posCoefficents.size() << std::endl;
    std::cout << "negCoefficents size: " << negCoefficents.size() << std::endl;
    std::vector<std::unique_ptr<Expression>> coefficents;
    for (size_t i = negCoefficents.size(); i > 1; i--) {
        coefficents.push_back(negCoefficents[i - 1]->Simplify());
    }
    for (const std::unique_ptr<Expression>& i : posCoefficents) {
        coefficents.push_back(i->Simplify());
    }
    std::cout << "Final coefficients size: " << coefficents.size() << std::endl;
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
//    if (termsC.size() == coefficents.size()) {
//        std::reverse(termsC.begin(), termsC.end());
//        for (auto pv : getAllFactors(termsC.back())) {
//            for (auto qv : getAllFactors(termsC.front())) {
//                if (gcf(pv, qv) == 1) {
//                    for (long long sign : { -1, 1 }) {
//                        bool doAdd = true;
//                        while (true) {
//                            long long mpv = pv * sign;
//                            std::vector<long long> newTermsC;
//                            long long h = 0;
//                            for (long long i : termsC) {
//                                h *= mpv;
//                                if (h % qv != 0) {
//                                    break;
//                                }
//                                h /= qv;
//                                h += i;
//                                newTermsC.push_back(h);
//                            }
//                            if (newTermsC.size() == termsC.size() && newTermsC.back() == 0) {
//                                termsC = newTermsC;
//                                if (doAdd) {
//                                    results.push_back(std::make_unique<Divide<Real>>(Real(1.0 * mpv), Real(1.0 * qv)));
//                                    doAdd = false;
//                                }
//                                do {
//                                    termsC.pop_back();
//                                } while (termsC.back() == 0);
//                                if (termsC.size() <= 1) {
//                                    break;
//                                }
//                            } else {
//                                break;
//                            }
//                        }
//                    }
//                }
//                if (termsC.size() <= 1) {
//                    break;
//                }
//            }
//            if (termsC.size() <= 1) {
//                break;
//            }
//        }
//        coefficents.clear();
//        std::reverse(termsC.begin(), termsC.end());
//        for (auto i : termsC) {
//            coefficents.push_back(Real(i * 1.0).Copy());
//        }
//    }
    if (termsC.size() == coefficents.size()) {
        if (coefficents.size() == 2) {  // Linear equation ax + b = 0
            if (auto aReal = RecursiveCast<Real>(*coefficents[1]); aReal != nullptr) {
                if (auto bReal = RecursiveCast<Real>(*coefficents[0]); bReal != nullptr) {
                    // Use Oasis expressions: -b/a
                    results.push_back(Divide(Multiply(Real(-1), *coefficents[0]), *coefficents[1]).Simplify());
                }
            }
        }
        else if (coefficents.size() == 3) {  // Quadratic equation ax + b + c = 0
            auto& a = coefficents[2];
            auto& b = coefficents[1];
            auto& c = coefficents[0];

            // Calculate discriminant
            auto bSquared = Multiply(*b, *b).Simplify();
            auto fourAC = Multiply(Real(4), Multiply(*a, *c)).Simplify();
            auto discriminant = Subtract(*bSquared, *fourAC).Simplify();

            if (auto realDisc = RecursiveCast<Real>(*discriminant);
                realDisc != nullptr && realDisc->GetValue() >= 0) {

                auto negB = Multiply(Real(-1), *b).Simplify();
                auto sqrtDisc = Exponent(*discriminant, Divide(Real(1), Real(2))).Copy();
                auto twoA = Multiply(Real(2), *a).Simplify();

                // First, create the numerators for both roots
                auto numerator1 = Add(*negB, *sqrtDisc).Simplify();
                auto numerator2 = Subtract(*negB, *sqrtDisc).Simplify();

                // Now create the Divide expressions properly
                results.push_back(Divide(*numerator1, *twoA).Copy());
                results.push_back(Divide(*numerator2, *twoA).Copy());
//                results.push_back(Divide(Add(*negB, *sqrtDisc), *twoA).Copy());
//                results.push_back(Divide(Subtract(*negB, *sqrtDisc), *twoA).Copy());
            }
        }
        else if (coefficents.size() == 4) {  // Cubic equation: ax³ + bx² + cx + d = 0
            long long a = termsC[0];  // coefficient of x³
            long long b = termsC[1];  // coefficient of x²
            long long c = termsC[2];  // coefficient of x
            long long d = termsC[3];  // constant term

            // To track found roots and avoid duplicates
            std::set<std::pair<long long, long long>> found_roots;

            // Get factors of constant term for possible p values
            std::vector<long long> p_factors;
            for (long long i = 1; i <= std::abs(d); i++) {
                if (d % i == 0) {
                    p_factors.push_back(i);
                    p_factors.push_back(-i);
                }
            }

            // Get factors of leading coefficient for possible q values
            std::vector<long long> q_factors;
            for (long long i = 1; i <= std::abs(a); i++) {
                if (a % i == 0) {
                    q_factors.push_back(i);
                }
            }

            for (long long p : p_factors) {
                for (long long q : q_factors) {
                    // Skip if q is 0
                    if (q == 0) continue;

                    // Simplify the fraction p/q
                    long long g = std::gcd(std::abs(p), q);
                    long long num = p / g;
                    long long den = q / g;

                    // Ensure denominator is positive
                    if (den < 0) {
                        num = -num;
                        den = -den;
                    }

                    // Check if we've already found this root
                    if (found_roots.find({num, den}) != found_roots.end()) {
                        continue;
                    }

                    // For each potential root p/q, evaluate the polynomial
                    long long x_p3 = p * p * p;
                    long long x_p2 = p * p;
                    long long x_q3 = q * q * q;

                    // Evaluate ax³ + bx² + cx + d = 0 at x = p/q
                    // Multiply all terms by q³ to eliminate denominators
                    long long val = a * x_p3 + b * x_p2 * q + c * p * q * q + d * x_q3;

                    if (val == 0) {  // If this is a root
                        results.push_back(Divide(Real(num), Real(den)).Copy());
                        found_roots.insert({num, den});
                    }
                }
            }
        }
    }
    return results;
}

auto Expression::Polynomial_Real() const -> std::vector<std::unique_ptr<Expression>> {
    std::vector<std::unique_ptr<Expression>> results;
    std::vector<std::unique_ptr<Expression>> termsE;

    // First handle x² - n special case
    if (auto subCase = RecursiveCast<Subtract<Expression>>(*this); subCase != nullptr) {
        if (auto leftTerm = RecursiveCast<Exponent<Variable, Real>>(subCase->GetMostSigOp());
            leftTerm != nullptr) {
            if (leftTerm->GetLeastSigOp().GetValue() == 2) {
                if (auto rightTerm = RecursiveCast<Real>(subCase->GetLeastSigOp());
                    rightTerm != nullptr) {
                    double n = rightTerm->GetValue();
                    double sqrtN = std::sqrt(n);
                    if (n > 0 && sqrtN == std::floor(sqrtN)) {
                        results.push_back(Divide(Real(sqrtN), Real(1)).Copy());
                        results.push_back(Divide(Real(-sqrtN), Real(1)).Copy());
                        return results;
                    }
                }
            }
        }
    }

    // Process terms
    if (auto addCase = RecursiveCast<Add<Expression>>(*this); addCase != nullptr) {
        addCase->Flatten(termsE);
    } else {
        termsE.push_back(Copy());
    }

    // Collect coefficients
    std::vector<std::unique_ptr<Expression>> coefficients;
    for (const auto& term : termsE) {
        if (auto realTerm = RecursiveCast<Real>(*term); realTerm != nullptr) {
            coefficients.push_back(realTerm->Copy());
        }
    }

    // Handle linear equation (ax + b)
    if (coefficients.size() == 2) {
        if (auto aReal = RecursiveCast<Real>(*coefficients[1]); aReal != nullptr) {
            if (auto bReal = RecursiveCast<Real>(*coefficients[0]); bReal != nullptr) {
                double a = aReal->GetValue();
                double b = bReal->GetValue();

                // ax + b = 0 → x = -b/a
                if (a != 0) {
                    double root = -b / a;
                    results.push_back(Divide(Real(root), Real(1)).Copy());
                }
            }
        }
    }

    // Handle quadratic equation (for ax² + bx + c)
    if (coefficients.size() == 3) {
        if (auto aReal = RecursiveCast<Real>(*coefficients[2]); aReal != nullptr) {
            if (auto bReal = RecursiveCast<Real>(*coefficients[1]); bReal != nullptr) {
                if (auto cReal = RecursiveCast<Real>(*coefficients[0]); cReal != nullptr) {
                    double a = aReal->GetValue();
                    double b = bReal->GetValue();
                    double c = cReal->GetValue();

                    double discriminant = b * b - 4 * a * c;
                    if (discriminant >= 0) {
                        double sqrtDisc = std::sqrt(discriminant);
                        double root1 = (-b + sqrtDisc) / (2 * a);
                        double root2 = (-b - sqrtDisc) / (2 * a);

                        results.push_back(Divide(Real(root1), Real(1)).Copy());
                        results.push_back(Divide(Real(root2), Real(1)).Copy());
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

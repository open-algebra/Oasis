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
#include <numeric>
#include <set>

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

// Helper function to find expression roots for polynomials
auto FindExpressionRoots(const std::vector<std::unique_ptr<Expression>>& coefficients)
    -> std::vector<std::unique_ptr<Expression>>
{

    std::vector<std::unique_ptr<Expression>> results;

    // Check degree of polynomial based on coefficients.size()
    if (coefficients.size() == 3) { // Quadratic: ax² + bx + c
        auto& a = coefficients[2];
        auto& b = coefficients[1];
        auto& c = coefficients[0];

        // Calculate discriminant
        auto bSquared = Multiply(*b, *b).Simplify();
        auto fourAC = Multiply(Real(4), Multiply(*a, *c)).Simplify();
        auto discriminant = Subtract(*bSquared, *fourAC).Simplify();

        if (RecursiveCast<Real>(*discriminant) == nullptr) {
            // Special cases
        }
    } else if (coefficients.size() == 4) { // Cubic: ax³ + bx² + cx + d
        // cubic
    } else if (coefficients.size() == 5) { // Quartic: ax⁴ + bx³ + cx² + dx + e
        // quartic
    }

    return results;
}

// currently only supports polynomials of one variable.
/**
 * The FindZeros function finds all rational zeros of a polynomial. Currently assumes an expression of the form a+bx+cx^2+dx^3+... where a, b, c, d are a integers.
 *
 * @tparam origonalExpresion The expression for which all the factors will be found.
 */
auto Expression::FindZeros() const -> std::expected<std::vector<std::unique_ptr<Expression>>, std::string>
{
    std::vector<std::unique_ptr<Expression>> results;
    std::vector<std::unique_ptr<Expression>> termsE;
    std::vector<std::unique_ptr<Expression>> termsVariableCheck;
    std::string variName = "";

    // Process terms to collect variables
    if (auto addCase = RecursiveCast<Add<Expression>>(*this); addCase != nullptr) {
        addCase->Flatten(termsVariableCheck);
    } else {
        termsVariableCheck.push_back(Copy());
    }

    // Check variables in the terms
    for (const auto& i : termsVariableCheck) {
        std::string variableName = "";

        if (auto variableCase = RecursiveCast<Variable>(*i); variableCase != nullptr) {
            variableName = variableCase->GetName();
        } else if (auto expCase = RecursiveCast<Exponent<Variable, Real>>(*i); expCase != nullptr) {
            variableName = expCase->GetMostSigOp().GetName();
        } else if (auto prodCase = RecursiveCast<Multiply<Expression, Variable>>(*i); prodCase != nullptr) {
            variableName = prodCase->GetLeastSigOp().GetName();
        } else if (auto prodExpCase = RecursiveCast<Multiply<Expression, Exponent<Variable, Real>>>(*i); prodExpCase != nullptr) {
            variableName = prodExpCase->GetLeastSigOp().GetMostSigOp().GetName();
        } else if (auto divCase = RecursiveCast<Divide<Expression, Variable>>(*i); divCase != nullptr) {
            variableName = divCase->GetLeastSigOp().GetName();
        } else if (auto divExpCase = RecursiveCast<Divide<Expression, Exponent<Variable, Real>>>(*i); divExpCase != nullptr) {
            variableName = divExpCase->GetLeastSigOp().GetMostSigOp().GetName();
        }

        if (!variableName.empty()) {
            if (variName.empty()) {
                variName = variableName;
            } else if (variName != variableName) {
                return std::unexpected("Error: Polynomial only supports expressions with a single variable.");
            }
        }
    }

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
                            Real(sqrtN), // MostSigOp
                            Real(1) // LeastSigOp
                        );

                        // For negative root: -n^(1/2)
                        auto negRoot = std::make_unique<Divide<Expression>>(
                            Real(-sqrtN), // MostSigOp
                            Real(1) // LeastSigOp
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
        termsE.push_back(subCase->GetMostSigOp().Copy()); // First term
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
        if (coefficents.size() == 2) { // Linear equation ax + b = 0
            if (auto aReal = RecursiveCast<Real>(*coefficents[1]); aReal != nullptr) {
                if (auto bReal = RecursiveCast<Real>(*coefficents[0]); bReal != nullptr) {
                    // Use Oasis expressions: -b/a
                    results.push_back(Divide(Multiply(Real(-1), *coefficents[0]), *coefficents[1]).Simplify());
                }
            }
            //        } else if (coefficents.size() == 3) { // Quadratic equation ax + b + c = 0
            //            auto& a = coefficents[2];
            //            auto& b = coefficents[1];
            //            auto& c = coefficents[0];
            //
            //            // Calculate discriminant
            //            auto bSquared = Multiply(*b, *b).Simplify();
            //            auto fourAC = Multiply(Real(4), Multiply(*a, *c)).Simplify();
            //            auto discriminant = Subtract(*bSquared, *fourAC).Simplify();
            //
            //            if (auto realDisc = RecursiveCast<Real>(*discriminant);
            //                realDisc != nullptr && realDisc->GetValue() >= 0) {
            //
            //                auto negB = Multiply(Real(-1), *b).Simplify();
            //                auto sqrtDisc = Exponent(*discriminant, Divide(Real(1), Real(2))).Copy();
            //                auto twoA = Multiply(Real(2), *a).Simplify();
            //
            //                // First, create the numerators for both roots
            //                auto numerator1 = Add(*negB, *sqrtDisc).Simplify();
            //                auto numerator2 = Subtract(*negB, *sqrtDisc).Simplify();
            //
            //                // Now create the Divide expressions properly
            //                results.push_back(Divide(*numerator1, *twoA).Copy());
            //                results.push_back(Divide(*numerator2, *twoA).Copy());
            //            }
        } else if (coefficents.size() == 3) { // Quadratic equation ax² + bx + c = 0
            auto& a = coefficents[2];
            auto& b = coefficents[1];
            auto& c = coefficents[0];

            // Calculate discriminant
            auto bSquared = Multiply(*b, *b).Simplify();
            auto fourAC = Multiply(Real(4), Multiply(*a, *c)).Simplify();
            auto discriminant = Subtract(*bSquared, *fourAC).Simplify();

            if (auto realDisc = RecursiveCast<Real>(*discriminant);
                realDisc != nullptr && realDisc->GetValue() >= 0) {

                double discValue = realDisc->GetValue();
                double sqrtDiscValue = std::sqrt(discValue);

                // Get coefficient values
                double aVal = 0, bVal = 0;
                if (auto aReal = RecursiveCast<Real>(*a); aReal != nullptr) {
                    aVal = aReal->GetValue();
                }
                if (auto bReal = RecursiveCast<Real>(*b); bReal != nullptr) {
                    bVal = bReal->GetValue();
                }

                if (std::floor(sqrtDiscValue) == sqrtDiscValue) {
                    // Perfect square - roots will be rational
                    double root1 = (-bVal + sqrtDiscValue) / (2 * aVal);
                    double root2 = (-bVal - sqrtDiscValue) / (2 * aVal);

                    // Check if roots are integers
                    if (std::floor(root1) == root1 && std::floor(root2) == root2) {
                        results.push_back(Divide(Real(root1), Real(1)).Copy());
                        results.push_back(Divide(Real(root2), Real(1)).Copy());
                    } else {
                        // Roots are fractions
                        results.push_back(Divide(Real(root1), Real(1)).Copy());
                        results.push_back(Divide(Real(root2), Real(1)).Copy());
                    }
                } else {

                    auto negB = Multiply(Real(-1), *b).Simplify();
                    auto sqrtDisc = Exponent(*discriminant, Divide(Real(1), Real(2))).Copy();
                    auto twoA = Multiply(Real(2), *a).Simplify();

                    //  (-b + √discriminant)/(2a)
                    auto numerator1 = Add(*negB, *sqrtDisc).Simplify();
                    results.push_back(Divide(*numerator1, *twoA).Copy());

                    // (-b - √discriminant)/(2a)
                    auto numerator2 = Subtract(*negB, *sqrtDisc).Simplify();
                    results.push_back(Divide(*numerator2, *twoA).Copy());
                }
            }
        } else if (coefficents.size() == 4) { // Cubic equation: ax³ + bx² + cx + d = 0
            long long a = termsC[0]; // coefficient of x³
            long long b = termsC[1]; // coefficient of x²
            long long c = termsC[2]; // coefficient of x
            long long d = termsC[3]; // constant term

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
                    if (q == 0)
                        continue;

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
                    if (found_roots.find({ num, den }) != found_roots.end()) {
                        continue;
                    }

                    // For each potential root p/q, evaluate the polynomial
                    long long x_p3 = p * p * p;
                    long long x_p2 = p * p;
                    long long x_q3 = q * q * q;

                    // Evaluate ax³ + bx² + cx + d = 0 at x = p/q
                    // Multiply all terms by q³ to eliminate denominators
                    long long val = a * x_p3 + b * x_p2 * q + c * p * q * q + d * x_q3;

                    if (val == 0) { // If this is a root
                        results.push_back(Divide(Real(static_cast<double>(num)), Real(static_cast<double>(den))).Copy());
                        found_roots.insert({ num, den });
                    }
                }
            }
        } else if (coefficents.size() == 5) { // Quartic equation ax⁴ + bx³ + cx² + dx + e = 0
            // coefficients
            long long a = 0, b = 0, c = 0, d = 0, e = 0;

            // Convert coefficients to numbers if possible
            if (auto aReal = RecursiveCast<Real>(*coefficents[4]); aReal != nullptr)
                a = static_cast<long long>(aReal->GetValue());
            if (auto bReal = RecursiveCast<Real>(*coefficents[3]); bReal != nullptr)
                b = static_cast<long long>(bReal->GetValue());
            if (auto cReal = RecursiveCast<Real>(*coefficents[2]); cReal != nullptr)
                c = static_cast<long long>(cReal->GetValue());
            if (auto dReal = RecursiveCast<Real>(*coefficents[1]); dReal != nullptr)
                d = static_cast<long long>(dReal->GetValue());
            if (auto eReal = RecursiveCast<Real>(*coefficents[0]); eReal != nullptr)
                e = static_cast<long long>(eReal->GetValue());

            // Find potential rational roots using the rational root theorem
            // Possible rational roots are p/q where:
            // - p is a factor of the constant term (e)
            // - q is a factor of the leading coefficient (a)

            // Get factors of constant term for possible p values
            std::vector<long long> p_factors;
            for (long long i = 1; i <= std::abs(e); i++) {
                if (e % i == 0) {
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

            // To track found roots and avoid duplicates
            std::set<std::pair<long long, long long>> found_roots;

            for (long long p : p_factors) {
                for (long long q : q_factors) {
                    // Skip if q is 0
                    if (q == 0)
                        continue;

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
                    if (found_roots.find({ num, den }) != found_roots.end()) {
                        continue;
                    }

                    // Evaluate the polynomial at p/q using synthetic division
                    // For a quartic: a(p/q)⁴ + b(p/q)³ + c(p/q)² + d(p/q) + e

                    // Multiply by q⁴ to clear denominators:
                    // a*p⁴ + b*p³*q + c*p²*q² + d*p*q³ + e*q⁴
                    long long p2 = p * p;
                    long long p3 = p2 * p;
                    long long p4 = p3 * p;
                    long long q2 = q * q;
                    long long q3 = q2 * q;
                    long long q4 = q3 * q;

                    long long val = a * p4 + b * p3 * q + c * p2 * q2 + d * p * q3 + e * q4;

                    if (val == 0) { // If this is a root
                        results.push_back(Divide(Real(static_cast<double>(num)), Real(static_cast<double>(den))).Copy());
                        found_roots.insert({ num, den });
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

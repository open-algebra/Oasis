#include <cmath>

#include <Oasis/Add.hpp>
#include <Oasis/Divide.hpp>
#include <Oasis/Exponent.hpp>
#include <Oasis/Integral.hpp>
#include <Oasis/Multiply.hpp>
#include <Oasis/RecursiveCast.hpp>
#include <Oasis/Subtract.hpp>
#include <Oasis/Variable.hpp>
#include <Oasis/Imaginary.hpp>
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

                // Check if discriminant is a perfect square
                if (std::floor(sqrtDiscValue) == sqrtDiscValue) {
                    // Rational roots
                    double aVal = 0, bVal = 0;
                    if (auto aReal = RecursiveCast<Real>(*a); aReal != nullptr)
                        aVal = aReal->GetValue();
                    if (auto bReal = RecursiveCast<Real>(*b); bReal != nullptr)
                        bVal = bReal->GetValue();

                    double root1 = (-bVal + sqrtDiscValue) / (2 * aVal);
                    double root2 = (-bVal - sqrtDiscValue) / (2 * aVal);

                    results.push_back(Divide(Real(root1), Real(1)).Copy());
                    results.push_back(Divide(Real(root2), Real(1)).Copy());
                } else {
                    // Irrational roots, build the expression trees

                    auto negB = Multiply(Real(-1), *b).Copy();
                    auto twoA = Multiply(Real(2), *a).Copy();
                    auto firstTerm = Divide(*negB, *twoA).Copy();

                    // √discriminant/(2a) term
                    auto sqrtDisc = std::make_unique<Exponent<Expression, Expression>>(
                        *discriminant,
                        Divide(Real(1), Real(2)));
                    auto secondTerm = Divide(*sqrtDisc, *twoA).Copy();

                    // Build the first root as an Add expression
                    auto root1 = std::make_unique<Add<Expression>>(
                        *firstTerm,
                        *secondTerm);

                    // Build the second root as a Subtract expression
                    auto root2 = std::make_unique<Subtract<Expression>>(
                        *firstTerm,
                        *secondTerm);

                    results.push_back(std::move(root1));
                    results.push_back(std::move(root2));
                }
            }
        } else if (coefficents.size() == 4) { // Cubic equation: ax³ + bx² + cx + d = 0
            // First try to find rational roots using the rational root theorem
            long long a_ll = 0, b_ll = 0, c_ll = 0, d_ll = 0;
            double a_d = 0, b_d = 0, c_d = 0;

            // Convert coefficients to numbers if possible
            if (auto aReal = RecursiveCast<Real>(*coefficents[3]); aReal != nullptr) {
                a_ll = static_cast<long long>(aReal->GetValue());
                a_d = aReal->GetValue();
            }
            if (auto bReal = RecursiveCast<Real>(*coefficents[2]); bReal != nullptr) {
                b_ll = static_cast<long long>(bReal->GetValue());
                b_d = bReal->GetValue();
            }
            if (auto cReal = RecursiveCast<Real>(*coefficents[1]); cReal != nullptr) {
                c_ll = static_cast<long long>(cReal->GetValue());
                c_d = cReal->GetValue();
            }
            if (auto dReal = RecursiveCast<Real>(*coefficents[0]); dReal != nullptr) {
                d_ll = static_cast<long long>(dReal->GetValue());
            }

            // To track found roots and avoid duplicates
            std::set<std::pair<long long, long long>> found_roots;

            // Get factors of constant term for possible p values
            std::vector<long long> p_factors;
            for (long long i = 1; i <= std::abs(d_ll); i++) {
                if (d_ll % i == 0) {
                    p_factors.push_back(i);
                    p_factors.push_back(-i);
                }
            }

            // Get factors of leading coefficient for possible q values
            std::vector<long long> q_factors;
            for (long long i = 1; i <= std::abs(a_ll); i++) {
                if (a_ll % i == 0) {
                    q_factors.push_back(i);
                }
            }

            // Check potential rational roots
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
                    long long val = a_ll * x_p3 + b_ll * x_p2 * q + c_ll * p * q * q + d_ll * x_q3;

                    if (val == 0) {
                        results.push_back(Divide(Real(static_cast<double>(num)), Real(static_cast<double>(den))).Copy());
                        found_roots.insert({ num, den });
                    }
                }
            }

            if (results.size() == 3) {
                return results;
            }

            else if (!results.empty()) {
                if (results.size() == 1) {
                    double root = 0;
                    if (auto divExpr = RecursiveCast<Divide<Expression>>(*results[0]); divExpr != nullptr) {
                        if (auto numReal = RecursiveCast<Real>(divExpr->GetMostSigOp()); numReal != nullptr) {
                            if (auto denReal = RecursiveCast<Real>(divExpr->GetLeastSigOp()); denReal != nullptr) {
                                root = numReal->GetValue() / denReal->GetValue();
                            }
                        }
                    }

                    // Synthetic division to get a quadratic polynomial
                    double a2 = a_d;
                    double b2 = b_d + a2 * root;
                    double c2 = c_d + b2 * root;

                    // Solve the quadratic equation a2x² + b2x + c2 = 0
                    double discriminant = b2 * b2 - 4 * a2 * c2;

                    if (discriminant >= 0) {
                        double sqrtDisc = std::sqrt(discriminant);

                        // Check if we get integer or simple fraction roots
                        double root1 = (-b2 + sqrtDisc) / (2 * a2);
                        double root2 = (-b2 - sqrtDisc) / (2 * a2);

                        // Check if roots are integers
                        if (std::floor(root1) == root1 && std::floor(root2) == root2) {
                            results.push_back(Divide(Real(root1), Real(1)).Copy());
                            results.push_back(Divide(Real(root2), Real(1)).Copy());
                        } else {
                            // Create symbolic expressions for the roots
                            auto negB = std::make_unique<Real>(-b2);
                            auto twoA = std::make_unique<Real>(2 * a2);
                            auto discExpr = std::make_unique<Real>(discriminant);

                            // Create √discriminant expression
                            auto sqrtExpr = std::make_unique<Exponent<Expression, Expression>>(
                                *discExpr,
                                Divide(Real(1), Real(2)));

                            // Create the first root expression: (-b + √discriminant)/(2a)
                            auto rootPlus = std::make_unique<Divide<Expression>>(
                                Add(*negB, *sqrtExpr),
                                *twoA);

                            // Create the second root expression: (-b - √discriminant)/(2a)
                            auto rootMinus = std::make_unique<Divide<Expression>>(
                                Subtract(*negB, *sqrtExpr),
                                *twoA);

                            results.push_back(std::move(rootPlus));
                            results.push_back(std::move(rootMinus));
                        }
                    }
                }
                return results;
            }

            // First normalize the equation to the form x³ + px² + qx + r = 0
            auto& a_expr = coefficents[3];
            auto& b_expr = coefficents[2];
            auto& c_expr = coefficents[1];
            auto& d_expr = coefficents[0];

            // Normalize by dividing by a
            auto a_inv = Divide(Real(1), *a_expr).Copy();
            auto p_expr = Multiply(*b_expr, *a_inv).Copy();
            auto q_expr = Multiply(*c_expr, *a_inv).Copy();
            auto r_expr = Multiply(*d_expr, *a_inv).Copy();

            // Convert to depressed cubic t³ + pt + q = 0
            // via substitution x = t - p/3

            // Calculate p' = -p²/3 + q
            auto p_squared = Multiply(*p_expr, *p_expr).Copy();
            auto term1 = Multiply(Real(-1.0 / 3.0), *p_squared).Copy();
            auto p_prime = Add(*term1, *q_expr).Copy();

            // Calculate q' = 2p³/27 - pq/3 + r
            auto p_cubed = Multiply(*p_expr, *p_squared).Copy();
            auto term2 = Multiply(Real(2.0 / 27.0), *p_cubed).Copy();
            auto pq = Multiply(*p_expr, *q_expr).Copy();
            auto term3 = Multiply(Real(-1.0 / 3.0), *pq).Copy();
            auto q_prime = Add(Add(*term2, *term3), *r_expr).Copy();

            // Calculate the discriminant Δ = (q'/2)² + (p'/3)³
            auto q_prime_half = Multiply(Real(0.5), *q_prime).Copy();
            auto q_prime_half_squared = Multiply(*q_prime_half, *q_prime_half).Copy();

            auto p_prime_third = Multiply(Real(1.0 / 3.0), *p_prime).Copy();
            auto p_prime_third_cubed = Exponent(*p_prime_third, Real(3)).Copy();

            auto discriminant = Add(*q_prime_half_squared, *p_prime_third_cubed).Copy();

            if (auto discReal = RecursiveCast<Real>(*discriminant); discReal != nullptr) {
                double disc_val = discReal->GetValue();

                // Create the cubic root expressions for u and v
                // u = ∛(-q'/2 + √Δ) and v = ∛(-q'/2 - √Δ)

                auto neg_q_prime_half = Multiply(Real(-1), *q_prime_half).Copy();

                if (disc_val > 0) {
                    // Create the square root of discriminant expression
                    auto sqrt_disc = Exponent(*discriminant, Divide(Real(1), Real(2))).Copy();

                    // Create expressions for u and v
                    auto u_arg = Add(*neg_q_prime_half, *sqrt_disc).Copy();
                    auto v_arg = Subtract(*neg_q_prime_half, *sqrt_disc).Copy();

                    auto u = Exponent(*u_arg, Divide(Real(1), Real(3))).Copy();
                    auto v = Exponent(*v_arg, Divide(Real(1), Real(3))).Copy();

                    // The real root is u + v - p/3
                    auto uv = Add(*u, *v).Copy();
                    auto p_third = Multiply(Real(1.0 / 3.0), *p_expr).Copy();
                    auto root = Subtract(*uv, *p_third).Copy();

                    results.push_back(std::move(root));
                } else if (disc_val == 0) {

                    // First root is 3(q'/p')
                    auto three_q_p = Multiply(Real(3), Divide(*q_prime, *p_prime)).Copy();

                    // Second/third root is -3(q'/2p')
                    auto neg_three_q_2p = Multiply(Real(-3), Divide(*q_prime, Multiply(Real(2), *p_prime))).Copy();

                    results.push_back(std::move(three_q_p));
                    results.push_back(std::move(neg_three_q_2p));
                    results.push_back(Divide(*neg_three_q_2p, Real(1)).Copy()); // Duplicate the repeated root
                } else {
                    // Trigonometric form
                    // Let p'/3 = -a² (a real)
                    // Let q'/2 = -a³cos(3θ)
                    // Now the roots are: 2a·cos(θ), 2a·cos(θ+2π/3), 2a·cos(θ+4π/3)

                    auto cubic_root1 = std::make_unique<Add<Expression>>(
                        Multiply(Real(-1.0 / 3.0), *p_expr), // -p/3
                        Exponent(*discriminant, Divide(Real(1), Real(3))) // ∛Δ
                    );

                    auto cubic_root2 = std::make_unique<Add<Expression>>(
                        Multiply(Real(-1.0 / 3.0), *p_expr), // -p/3
                        Multiply(
                            Real(-0.5), // -1/2
                            Exponent(*discriminant, Divide(Real(1), Real(3))) // ∛Δ
                            ));

                    auto cubic_root3 = std::make_unique<Add<Expression>>(
                        Multiply(Real(-1.0 / 3.0), *p_expr), // -p/3
                        Multiply(
                            Real(-0.5), // -1/2
                            Exponent(*discriminant, Divide(Real(1), Real(3))) // ∛Δ
                            ));

                    results.push_back(std::move(cubic_root1));
                    results.push_back(std::move(cubic_root2));
                    results.push_back(std::move(cubic_root3));
                }
            }
        }
        else if (coefficents.size() == 5) {  // Quartic equation ax⁴ + bx³ + cx² + dx + e = 0
            auto& a = coefficents[4];
            auto& b = coefficents[3];
            auto& c = coefficents[2];
            auto& d = coefficents[1];
            auto& e = coefficents[0];

            long long a_ll = 0, b_ll = 0, c_ll = 0, d_ll = 0, e_ll = 0;

            // Convert coefficients to numbers if possible
            if (auto aReal = RecursiveCast<Real>(*a); aReal != nullptr) a_ll = static_cast<long long>(aReal->GetValue());
            if (auto bReal = RecursiveCast<Real>(*b); bReal != nullptr) b_ll = static_cast<long long>(bReal->GetValue());
            if (auto cReal = RecursiveCast<Real>(*c); cReal != nullptr) c_ll = static_cast<long long>(cReal->GetValue());
            if (auto dReal = RecursiveCast<Real>(*d); dReal != nullptr) d_ll = static_cast<long long>(dReal->GetValue());
            if (auto eReal = RecursiveCast<Real>(*e); eReal != nullptr) e_ll = static_cast<long long>(eReal->GetValue());

            // To track found roots and avoid duplicates
            std::set<std::pair<long long, long long>> found_roots;

            // Check for rational roots only if coefficients are integers
            if (a_ll != 0 && e_ll != 0) {
                // Get factors of constant term for possible p values
                std::vector<long long> p_factors;
                for (long long i = 1; i <= std::abs(e_ll); i++) {
                    if (e_ll % i == 0) {
                        p_factors.push_back(i);
                        p_factors.push_back(-i);
                    }
                }

                // Get factors of leading coefficient for possible q values
                std::vector<long long> q_factors;
                for (long long i = 1; i <= std::abs(a_ll); i++) {
                    if (a_ll % i == 0) {
                        q_factors.push_back(i);
                    }
                }

                // Check potential rational roots
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
                        if (found_roots.find({ num, den }) != found_roots.end()) {
                            continue;
                        }

                        // For each potential root p/q, evaluate the polynomial
                        double x = static_cast<double>(num) / den;
                        double x2 = x * x;
                        double x3 = x2 * x;
                        double x4 = x3 * x;

                        // Evaluate ax⁴ + bx³ + cx² + dx + e
                        double val = a_ll * x4 + b_ll * x3 + c_ll * x2 + d_ll * x + e_ll;

                        if (std::abs(val) < 1e-10) { // If this is a root (with small tolerance)
                            results.push_back(Divide(Real(static_cast<double>(num)), Real(static_cast<double>(den))).Copy());
                            found_roots.insert({ num, den });
                        }
                    }
                }
            }

            // If we found some rational roots, use polynomial division
            if (results.size() > 0 && results.size() < 4) {
                return results;
            }

            // Check for biquadratic case (ax⁴ + cx² + e = 0)
            bool isBiquadratic = false;

            if (auto bReal = RecursiveCast<Real>(*b); bReal != nullptr) {
                if (auto dReal = RecursiveCast<Real>(*d); dReal != nullptr) {
                    if (std::abs(bReal->GetValue()) < 1e-10 && std::abs(dReal->GetValue()) < 1e-10) {
                        isBiquadratic = true;
                    }
                }
            }

            if (isBiquadratic) {
                auto discriminant = Subtract(
                    Multiply(*c, *c),
                    Multiply(Real(4), Multiply(*a, *e))
                        ).Simplify();

                if (auto discReal = RecursiveCast<Real>(*discriminant); discReal != nullptr) {
                    double disc_val = discReal->GetValue();

                    // Calculate -c/(2a) term which appears in both roots
                    auto negC = Multiply(Real(-1), *c).Copy();
                    auto twoA = Multiply(Real(2), *a).Copy();
                    auto commonTerm = Divide(*negC, *twoA).Copy();

                    if (disc_val >= 0) {
                        // Real quadratic roots

                        // Create √discriminant term
                        auto sqrtDisc = Exponent(*discriminant, Divide(Real(1), Real(2))).Copy();
                        auto sqrtDiscOver2A = Divide(*sqrtDisc, *twoA).Copy();

                        // First root of the quadratic: y₁ = (-c + √disc)/(2a)
                        auto y1 = Add(*commonTerm, *sqrtDiscOver2A).Copy();

                        // Second root of the quadratic: y₂ = (-c - √disc)/(2a)
                        auto y2 = Subtract(*commonTerm, *sqrtDiscOver2A).Copy();


                        // For y₁:
                        if (auto y1Real = RecursiveCast<Real>(*y1); y1Real != nullptr) {
                            double y1_val = y1Real->GetValue();

                            if (y1_val > 0) {
                                // y₁ > 0: Two real roots x = ±√y₁
                                auto sqrt_y1 = Exponent(Real(y1_val), Divide(Real(1), Real(2))).Copy();
                                auto neg_sqrt_y1 = Multiply(Real(-1), *sqrt_y1).Copy();

                                results.push_back(std::move(sqrt_y1));
                                results.push_back(std::move(neg_sqrt_y1));
                            } else if (y1_val < 0) {
                                // y₁ < 0: Two imaginary roots x = ±i√|y₁|
                                auto abs_y1 = Real(std::abs(y1_val)).Copy();
                                auto sqrt_abs_y1 = Exponent(*abs_y1, Divide(Real(1), Real(2))).Copy();

                                auto img_root1 = Multiply(Imaginary(), *sqrt_abs_y1).Copy();
                                auto img_root2 = Multiply(Real(-1), *img_root1).Copy();

                                results.push_back(std::move(img_root1));
                                results.push_back(std::move(img_root2));
                            } else {
                                // y₁ = 0: One root with multiplicity 2
                                results.push_back(Real(0).Copy());
                            }
                        } else {
                            // y₁ is a symbolic expression
                            auto sqrt_y1 = Exponent(*y1, Divide(Real(1), Real(2))).Copy();
                            auto neg_sqrt_y1 = Multiply(Real(-1), *sqrt_y1).Copy();

                            results.push_back(std::move(sqrt_y1));
                            results.push_back(std::move(neg_sqrt_y1));
                        }

                        // For y₂:
                        if (auto y2Real = RecursiveCast<Real>(*y2); y2Real != nullptr) {
                            double y2_val = y2Real->GetValue();

                            if (y2_val > 0) {
                                // y₂ > 0: Two real roots x = ±√y₂
                                auto sqrt_y2 = Exponent(Real(y2_val), Divide(Real(1), Real(2))).Copy();
                                auto neg_sqrt_y2 = Multiply(Real(-1), *sqrt_y2).Copy();

                                results.push_back(std::move(sqrt_y2));
                                results.push_back(std::move(neg_sqrt_y2));
                            } else if (y2_val < 0) {
                                // y₂ < 0: Two imaginary roots x = ±i√|y₂|
                                auto abs_y2 = Real(std::abs(y2_val)).Copy();
                                auto sqrt_abs_y2 = Exponent(*abs_y2, Divide(Real(1), Real(2))).Copy();

                                auto img_root1 = Multiply(Imaginary(), *sqrt_abs_y2).Copy();
                                auto img_root2 = Multiply(Real(-1), *img_root1).Copy();

                                results.push_back(std::move(img_root1));
                                results.push_back(std::move(img_root2));
                            } else {
                                // y₂ = 0: One root with multiplicity 2 (already added)
                            }
                        } else {
                            // y₂ is a symbolic expression
                            auto sqrt_y2 = Exponent(*y2, Divide(Real(1), Real(2))).Copy();
                            auto neg_sqrt_y2 = Multiply(Real(-1), *sqrt_y2).Copy();

                            results.push_back(std::move(sqrt_y2));
                            results.push_back(std::move(neg_sqrt_y2));
                        }
                    } else {

                        // Create √|discriminant| term
                        auto absDisc = Multiply(Real(-1), *discriminant).Copy();
                        auto sqrtAbsDisc = Exponent(*absDisc, Divide(Real(1), Real(2))).Copy();
                        auto imagTerm = Divide(*sqrtAbsDisc, *twoA).Copy();

                        // First root: -c/(2a) + i·√|disc|/(2a)
                        auto y1_imag = Multiply(Imaginary(), *imagTerm).Copy();
                        auto y1 = Add(*commonTerm, *y1_imag).Copy();

                        // Second root: -c/(2a) - i·√|disc|/(2a)
                        auto y2_imag = Multiply(Real(-1), *y1_imag).Copy();
                        auto y2 = Add(*commonTerm, *y2_imag).Copy();

                        std::cout << "Complex biquadratic roots not fully implemented yet." << std::endl;
                    }
                }

                return results;
            }

            if (results.empty()) {
                // Convert to depressed quartic y⁴ + py² + qy + r
                // via substitution y = x - b/(4a)

                // Get a⁻¹ for normalization
                auto a_inv = Divide(Real(1), *a).Copy();

                // Calculate the substitution term b/(4a)
                auto b_over_4a = Divide(*b, Multiply(Real(4), *a)).Copy();

                // Calculate coefficients of depressed quartic

                // p = -3b²/(8a²) + c/a
                auto b_squared = Multiply(*b, *b).Copy();
                auto term1 = Multiply(Real(-3), Divide(*b_squared, Multiply(Real(8), Multiply(*a, *a)))).Copy();
                auto term2 = Multiply(*c, *a_inv).Copy();
                auto p = Add(*term1, *term2).Copy();

                // q = b³/(8a³) - bc/(2a²) + d/a
                auto b_cubed = Multiply(*b, *b_squared).Copy();
                auto term3 = Divide(*b_cubed, Multiply(Real(8), Exponent(*a, Real(3)))).Copy();
                auto bc = Multiply(*b, *c).Copy();
                auto term4 = Multiply(Real(-1), Divide(*bc, Multiply(Real(2), Multiply(*a, *a)))).Copy();
                auto term5 = Multiply(*d, *a_inv).Copy();
                auto q = Add(Add(*term3, *term4), *term5).Copy();

                // r = -3b⁴/(256a⁴) + b²c/(16a³) - bd/(4a²) + e/a
                auto b_fourth = Multiply(*b_squared, *b_squared).Copy();
                auto term6 = Multiply(Real(-3), Divide(*b_fourth, Multiply(Real(256), Exponent(*a, Real(4))))).Copy();
                auto b_squared_c = Multiply(*b_squared, *c).Copy();
                auto term7 = Divide(*b_squared_c, Multiply(Real(16), Exponent(*a, Real(3)))).Copy();
                auto bd = Multiply(*b, *d).Copy();
                auto term8 = Multiply(Real(-1), Divide(*bd, Multiply(Real(4), Multiply(*a, *a)))).Copy();
                auto term9 = Multiply(*e, *a_inv).Copy();
                auto r = Add(Add(Add(*term6, *term7), *term8), *term9).Copy();

                // Create resolvent cubic z³ + 2pz² + (p² - 4r)z - q²
                auto two_p = Multiply(Real(2), *p).Copy();
                auto p_squared = Multiply(*p, *p).Copy();
                auto four_r = Multiply(Real(4), *r).Copy();
                auto p_squared_minus_4r = Subtract(*p_squared, *four_r).Copy();
                auto q_squared = Multiply(*q, *q).Copy();
                auto neg_q_squared = Multiply(Real(-1), *q_squared).Copy();

                // Solve the resolvent cubic to find a value of z
                auto cubic_a = Real(1).Copy();
                auto cubic_b = two_p->Copy();
                auto cubic_c = p_squared_minus_4r->Copy();
                auto cubic_d = neg_q_squared->Copy();

                std::vector<std::unique_ptr<Expression>> cubic_coeffs;
                cubic_coeffs.push_back(std::move(cubic_d));
                cubic_coeffs.push_back(std::move(cubic_c));
                cubic_coeffs.push_back(std::move(cubic_b));
                cubic_coeffs.push_back(std::move(cubic_a));

                auto cubic_expr = std::make_unique<Add<Expression>>(
                    Exponent<Variable, Real>{ Variable("x"), Real(3) },
                    Multiply(Real(2), Multiply(*p, Exponent<Variable, Real>{ Variable("x"), Real(2) })),
                    Multiply(*p_squared_minus_4r, Variable("x")),
                    *neg_q_squared
                );


                // Let's assume we found a root z (for demonstration purposes)
                auto z = Real(1).Copy();  // Placeholder for an actual root

                // Step 3: Use z to factor the quartic into two quadratics
                // y⁴ + py² + qy + r = (y² + u*y + v) * (y² - u*y + w)
                // where u² = z, v + w = p + z, and v*w = r

                // Calculate u = ±√z
                auto u = Exponent(*z, Divide(Real(1), Real(2))).Copy();

                // v + w = p + z
                // v*w = r

                auto p_plus_z = Add(*p, *z).Copy();
                auto neg_p_plus_z = Multiply(Real(-1), *p_plus_z).Copy();

                // Calculate discriminant of this quadratic
                auto quad_disc = Subtract(
                    Multiply(*p_plus_z, *p_plus_z),
                    Multiply(Real(4), *r)
                        ).Copy();

                auto sqrt_quad_disc = Exponent(*quad_disc, Divide(Real(1), Real(2))).Copy();

                // v = (-(p+z) + √((p+z)² - 4r))/2
                auto v = Divide(Add(*neg_p_plus_z, *sqrt_quad_disc), Real(2)).Copy();

                // w = (-(p+z) - √((p+z)² - 4r))/2
                auto w = Divide(Subtract(*neg_p_plus_z, *sqrt_quad_disc), Real(2)).Copy();

                // (y² + u*y + v) and (y² - u*y + w)
                // First quadratic: y² + u*y + v = 0
                auto quad1_disc = Subtract(
                    Multiply(*u, *u),
                    Multiply(Real(4), *v)
                        ).Copy();

                auto sqrt_quad1_disc = Exponent(*quad1_disc, Divide(Real(1), Real(2))).Copy();
                auto neg_u = Multiply(Real(-1), *u).Copy();

                // Roots of first quadratic
                auto y1 = Divide(Add(*neg_u, *sqrt_quad1_disc), Real(2)).Copy();
                auto y2 = Divide(Subtract(*neg_u, *sqrt_quad1_disc), Real(2)).Copy();

                // Second quadratic: y² - u*y + w = 0
                auto quad2_disc = Subtract(
                    Multiply(*u, *u),
                    Multiply(Real(4), *w)
                        ).Copy();

                auto sqrt_quad2_disc = Exponent(*quad2_disc, Divide(Real(1), Real(2))).Copy();

                // Roots of second quadratic
                auto y3 = Divide(Add(*u, *sqrt_quad2_disc), Real(2)).Copy();
                auto y4 = Divide(Subtract(*u, *sqrt_quad2_disc), Real(2)).Copy();

                // Step 6: Convert back to original variable x = y + b/(4a)
                auto x1 = Add(*y1, *b_over_4a).Copy();
                auto x2 = Add(*y2, *b_over_4a).Copy();
                auto x3 = Add(*y3, *b_over_4a).Copy();
                auto x4 = Add(*y4, *b_over_4a).Copy();

                // Add all four roots to the results
                results.push_back(std::move(x1));
                results.push_back(std::move(x2));
                results.push_back(std::move(x3));
                results.push_back(std::move(x4));
            }
        }
//        else if (coefficents.size() == 5) { // Quartic equation ax⁴ + bx³ + cx² + dx + e = 0
//            // coefficients
//            long long a = 0, b = 0, c = 0, d = 0, e = 0;
//
//            // Convert coefficients to numbers if possible
//            if (auto aReal = RecursiveCast<Real>(*coefficents[4]); aReal != nullptr)
//                a = static_cast<long long>(aReal->GetValue());
//            if (auto bReal = RecursiveCast<Real>(*coefficents[3]); bReal != nullptr)
//                b = static_cast<long long>(bReal->GetValue());
//            if (auto cReal = RecursiveCast<Real>(*coefficents[2]); cReal != nullptr)
//                c = static_cast<long long>(cReal->GetValue());
//            if (auto dReal = RecursiveCast<Real>(*coefficents[1]); dReal != nullptr)
//                d = static_cast<long long>(dReal->GetValue());
//            if (auto eReal = RecursiveCast<Real>(*coefficents[0]); eReal != nullptr)
//                e = static_cast<long long>(eReal->GetValue());
//
//            // Find potential rational roots using the rational root theorem
//            // Possible rational roots are p/q where:
//            // - p is a factor of the constant term (e)
//            // - q is a factor of the leading coefficient (a)
//
//            // Get factors of constant term for possible p values
//            std::vector<long long> p_factors;
//            for (long long i = 1; i <= std::abs(e); i++) {
//                if (e % i == 0) {
//                    p_factors.push_back(i);
//                    p_factors.push_back(-i);
//                }
//            }
//
//            // Get factors of leading coefficient for possible q values
//            std::vector<long long> q_factors;
//            for (long long i = 1; i <= std::abs(a); i++) {
//                if (a % i == 0) {
//                    q_factors.push_back(i);
//                }
//            }
//
//            // To track found roots and avoid duplicates
//            std::set<std::pair<long long, long long>> found_roots;
//
//            for (long long p : p_factors) {
//                for (long long q : q_factors) {
//                    // Skip if q is 0
//                    if (q == 0)
//                        continue;
//
//                    // Simplify the fraction p/q
//                    long long g = std::gcd(std::abs(p), q);
//                    long long num = p / g;
//                    long long den = q / g;
//
//                    // Ensure denominator is positive
//                    if (den < 0) {
//                        num = -num;
//                        den = -den;
//                    }
//
//                    // Check if we've already found this root
//                    if (found_roots.find({ num, den }) != found_roots.end()) {
//                        continue;
//                    }
//
//                    // Evaluate the polynomial at p/q using synthetic division
//                    // For a quartic: a(p/q)⁴ + b(p/q)³ + c(p/q)² + d(p/q) + e
//
//                    // Multiply by q⁴ to clear denominators:
//                    // a*p⁴ + b*p³*q + c*p²*q² + d*p*q³ + e*q⁴
//                    long long p2 = p * p;
//                    long long p3 = p2 * p;
//                    long long p4 = p3 * p;
//                    long long q2 = q * q;
//                    long long q3 = q2 * q;
//                    long long q4 = q3 * q;
//
//                    long long val = a * p4 + b * p3 * q + c * p2 * q2 + d * p * q3 + e * q4;
//
//                    if (val == 0) { // If this is a root
//                        results.push_back(Divide(Real(static_cast<double>(num)), Real(static_cast<double>(den))).Copy());
//                        found_roots.insert({ num, den });
//                    }
//                }
//            }
//        }
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

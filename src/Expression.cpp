#include <cmath>

#include <Oasis/Add.hpp>
#include <Oasis/Derivative.hpp>
#include <Oasis/DifferentiateVisitor.hpp>
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
    SimplifyVisitor simplifyVisitor {};

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
        } else if (auto prodExpCase = RecursiveCast<Multiply<Expression, Exponent<Variable, Real>>>(*i); prodExpCase
            != nullptr) {
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
        coefficents.push_back(negCoefficents[i - 1]->Accept(simplifyVisitor).value());
    }
    for (const std::unique_ptr<Expression>& i : posCoefficents) {
        coefficents.push_back(i->Accept(simplifyVisitor).value());
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
                                    results.push_back(
                                        std::make_unique<Divide<Real>>(Real(1.0 * mpv), Real(1.0 * qv)));
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
        results.push_back(
            Divide(Multiply(Real(-1), *coefficents[0]), *coefficents[1]).Accept(simplifyVisitor).value());
    } else if (coefficents.size() == 3) {
        auto& a = coefficents[2];
        auto& b = coefficents[1];
        auto& c = coefficents[0];
        auto negB = Multiply(Real(-1.0), *b).Accept(simplifyVisitor).value();
        auto sqrt = Exponent(
            *Add(Multiply(*b, *b), Multiply(Real(-4), Multiply(*a, *c))).Accept(simplifyVisitor).value(),
            Divide(Real(1), Real(2)))
                        .Copy();
        auto twoA = Multiply(Real(2), *a).Accept(simplifyVisitor).value();
        results.push_back(Divide(Add(*negB, *sqrt), *twoA).Copy());
        results.push_back(Divide(Subtract(*negB, *sqrt), *twoA).Copy());
    }
    return results;
}

auto Expression::ApproximateZeros(const Expression& variable, const Expression& guess, int iterations) const -> std::unique_ptr<Expression>
{
    // Setup simplifyVisitor for later
    SimplifyVisitor simplifyVisitor {};

    // Helper variables

    // List of guesses that we've taken.
    // If any of these are duplicates, then we've run into a cycle and can't find a root.
    std::unique_ptr<Expression> guessList[iterations];

    // Variable representing the number 0.
    // Useful in determining a situation where the guess a results in f(a) = 0 or f'(a) = 0
    std::unique_ptr<Expression> zero = Real { 0.0f }.Copy();

    // Declare the function and its derivative
    std::unique_ptr<Expression> originalFunction = this->Copy();
    std::unique_ptr<Expression> derivative = originalFunction->Differentiate(variable);

    // Function fails to differentiate
    if (derivative == nullptr) return nullptr;

    // New guess (starts at the original guess's value)
    std::unique_ptr<Expression> x = guess.Copy();

    // Iterate for the given number of times
    // The higher the number of iterations, the more accurate the result (in theory)
    for (int i = 0; i < iterations; ++i)
    {
        // Evaluated version of the functions
        // f(a) and f'(a), for the guess a (stored in x)
        std::unique_ptr<Expression> evaluatedFunction = originalFunction->Substitute(variable, *x);
        std::unique_ptr<Expression> evaluatedDerivative = derivative->Substitute(variable, *x);

        evaluatedFunction = evaluatedFunction->Accept(simplifyVisitor).value();
        evaluatedDerivative = evaluatedDerivative->Accept(simplifyVisitor).value();

        // If this is true, then we are in a divide-by-0 case
        // (when evaluated_function = 0). We can't continue, so return nullptr.
        if (evaluatedDerivative->Equals(*zero)) return nullptr;

        // If evaluated_function = 0 (and evaluated_derivative != 0),
        // then we found an approximation. Return that value.
        if (evaluatedFunction->Equals(*zero))
        {
            // If our current approximation is just the original guess,
            // then we weren't able to make a new guess (either we went in
            // a loop, or f(guess) = 0). In that case, we didn't find
            // anything useful, so we should return nullptr instead.
            if (x->Equals(guess)) return nullptr;

            // Otherwise, we found an actual approximation, so return that.
            return x;
        }

        // We might not be able to evaluate this function as a number.
        // In that case, we can't get to a number, so we need to return nullptr.
        if (!evaluatedFunction->Is<Real>() || !evaluatedDerivative->Is<Real>())
        {
            return nullptr;
        }

        // Divide f'(a) /  f(a)

        std::unique_ptr<Expression> divided = Divide<Expression, Expression> { *evaluatedFunction, *evaluatedDerivative }.Copy();

        divided = divided->Accept(simplifyVisitor).value();

        // Subtract x - (f'(a) / f(a)) to get the new guess
        x = Subtract<Expression, Expression> { *x->Copy(), *divided->Copy() }.Accept(simplifyVisitor).value();

        // Add this to our list of guesses
        guessList[i] = std::move(x->Copy());

        // Make sure that we haven't already seen this value.
        // If we have, then we've gone into a cycle and won't be able to find a solution.
        for (int j = 0; j < i; ++j)
        {
            if (guessList[i]->Equals(*guessList[j]))
            {
                // We found a duplicate and will be going in a loop
                // We can't find anything from here, so we have to return nullptr.
                return nullptr;
            }
        }
    }

    return x->Copy();
}

auto Expression::GetCategory() const -> uint32_t
{
    return 0;
}

auto Expression::Differentiate(const Expression& differentiationVariable) const -> std::unique_ptr<Expression>
{
    DifferentiateVisitor dv { differentiationVariable.Copy() };
    auto diffed = Accept(dv);
    if (!diffed) {
        return Derivative<Expression, Expression> { *(this->Copy()), differentiationVariable }.Generalize();
    }
    return std::move(diffed).value();
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

auto Expression::IntegrateWithBounds(const Expression& variable, const Expression&,
    const Expression&) -> std::unique_ptr<Expression>
{
    Integral<Expression, Expression> integral { *(this->Copy()), *(variable.Copy()) };

    return integral.Copy();
}

auto Expression::Simplify() const -> std::unique_ptr<Expression>
{
    SimplifyVisitor sV {};
    return std::move(Accept(sV)).value();
}
} // namespace Oasis
std::unique_ptr<Oasis::Expression> operator+(const std::unique_ptr<Oasis::Expression>& lhs,
    const std::unique_ptr<Oasis::Expression>& rhs)
{
    Oasis::SimplifyVisitor sV {};
    auto e = Oasis::Add { *lhs, *rhs };
    auto s = e.Accept(sV);
    if (!s) {
        return e.Generalize();
    }
    return std::move(s).value();
}

std::unique_ptr<Oasis::Expression> operator-(const std::unique_ptr<Oasis::Expression>& lhs,
    const std::unique_ptr<Oasis::Expression>& rhs)
{
    Oasis::SimplifyVisitor sV {};
    auto e = Oasis::Subtract { *lhs, *rhs };
    auto s = e.Accept(sV);
    if (!s) {
        return e.Generalize();
    }
    return std::move(s).value();
}

std::unique_ptr<Oasis::Expression> operator*(const std::unique_ptr<Oasis::Expression>& lhs,
    const std::unique_ptr<Oasis::Expression>& rhs)
{
    Oasis::SimplifyVisitor sV {};
    auto e = Oasis::Multiply { *lhs, *rhs };
    auto s = e.Accept(sV);
    if (!s) {
        return e.Generalize();
    }
    return std::move(s).value();
}

std::unique_ptr<Oasis::Expression> operator/(const std::unique_ptr<Oasis::Expression>& lhs,
    const std::unique_ptr<Oasis::Expression>& rhs)
{
    Oasis::SimplifyVisitor sV {};
    auto e = Oasis::Divide { *lhs, *rhs };
    auto s = e.Accept(sV);
    if (!s) {
        return e.Generalize();
    }
    return std::move(s).value();
}

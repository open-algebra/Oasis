#include "Oasis/Util.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include <algorithm>
#include <cmath>
namespace Oasis::Util {

std::unique_ptr<Expression> IntegerComplex::getExpression() const
{
    if (imaginary == 0) {
        return std::make_unique<Real>(real);
    }
    std::unique_ptr<Expression> iPar;
    if (imaginary == 1) {
        iPar = std::make_unique<Imaginary>();
    } else {
        iPar = std::make_unique<Multiply<Real, Imaginary>>(Real(imaginary), Imaginary());
    }
    if (real == 0) {
        return iPar;
    }
    Real rPar(real);
    return std::make_unique<Add<Real, Expression>>(Real(real), *iPar);
}
IntegerComplex& IntegerComplex::operator+=(const IntegerComplex& rhs)
{
    real += rhs.real;
    imaginary += rhs.imaginary;
    return *this;
}
IntegerComplex& IntegerComplex::operator*=(const IntegerComplex& rhs)
{
    const auto oldReal = real;
    const auto rhsOldReal = rhs.real;
    real = oldReal * rhs.real - imaginary * rhs.imaginary;
    imaginary = oldReal * rhs.imaginary + imaginary * rhsOldReal;
    return *this;
}
IntegerComplex& IntegerComplex::operator-=(const IntegerComplex& rhs)
{
    real -= rhs.real;
    imaginary -= rhs.imaginary;
    return *this;
}
IntegerComplex& IntegerComplex::operator/=(const IntegerComplex& rhs)
{
    const long long base = rhs.absSquared();
    operator*=(rhs.conj());
    real /= base;
    imaginary /= base;
    return *this;
}
IntegerComplex& IntegerComplex::operator%=(const IntegerComplex& rhs)
{
    const IntegerComplex rConj = rhs.conj();
    const long long base = rhs.absSquared();
    operator*=(rConj);
    real %= base;
    imaginary %= base;
    if (real * 2 > base) {
        real -= base;
    }
    if (imaginary * 2 > base) {
        imaginary -= base;
    }
    return operator/=(rConj);
}
bool IntegerComplex::operator<(const IntegerComplex& rhs) const
{
    auto mAbs = absSquared();
    auto rhsAbs = rhs.absSquared();
    if (mAbs == rhsAbs) {
        return imaginary < rhs.imaginary;
    }
    return mAbs < rhsAbs;
}

IntegerComplex operator+(IntegerComplex lhs, const IntegerComplex& rhs) { return lhs += rhs; }
IntegerComplex operator-(IntegerComplex lhs, const IntegerComplex& rhs) { return lhs -= rhs; }
IntegerComplex operator*(IntegerComplex lhs, const IntegerComplex& rhs) { return lhs *= rhs; }
IntegerComplex operator/(IntegerComplex lhs, const IntegerComplex& rhs) { return lhs /= rhs; }
IntegerComplex operator%(IntegerComplex lhs, const IntegerComplex& rhs) { return lhs %= rhs; }

void PrimesList::generatePrimesUpToN(long long num)
{
    long long n = primes.back() + 2;
    while (primes.back() < num) {
        bool isPrime = true;
        for (size_t i2 = 1; primes[i2] * primes[i2] <= n; i2++) {
            if (!(n % primes[i2])) {
                isPrime = false;
                break;
            }
        }
        if (isPrime)
            primes.push_back(n);
        n += 2;
    }
}
std::vector<long long> PrimesList::primes = { 2, 3 };

long long gcf(long long a, long long b)
{
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

bool isInt(double n)
{
    return n == round(n);
}

Oasis::Real abs(const Oasis::Expression& exp)
{
    auto simpExp = exp.Simplify();
    if (auto realCase = Real::Specialize(*simpExp); realCase != nullptr) {
        return Real(std::abs(realCase->GetValue()));
    }
    if (auto imgCase = Imaginary::Specialize(*simpExp); imgCase != nullptr) {
        return Real(1);
    }
    if (auto imgCase = Multiply<Real, Imaginary>::Specialize(*simpExp); imgCase != nullptr) {
        return Real(std::abs(imgCase->GetMostSigOp().GetValue()));
    }
    if (auto comp1ImgCase = Add<Real, Imaginary>::Specialize(*simpExp); comp1ImgCase != nullptr) {
        double realValue = comp1ImgCase->GetMostSigOp().GetValue();
        return Real(std::sqrt(realValue * realValue + 1.0));
    }
    if (auto compCase = Add<Real, Multiply<Real, Imaginary>>::Specialize(*simpExp); compCase != nullptr) {
        double realValue = compCase->GetMostSigOp().GetValue();
        double imagValue = compCase->GetLeastSigOp().GetMostSigOp().GetValue();
        return Real(std::sqrt(realValue * realValue + imagValue * imagValue));
    }
    return Real(NAN);
}

template <class T>
T modularPow(T base, long long power, T mod)
{
    if (power == 0) {
        return T(1);
    }
    T answer(1);
    T cVal = base;
    while (true) {
        if (power % 2 == 1) {
            answer *= cVal;
            answer %= mod;
        }
        power /= 2;
        if (power == 0)
            return answer;
        cVal *= cVal;
        cVal %= mod;
    }
}

template <class T>
T pow(T base, long long power)
{
    if (power == 0) {
        return T(1);
    }
    T answer(1);
    T cVal = base;
    while (true) {
        if (power % 2 == 1)
            answer *= cVal;
        power /= 2;
        if (power == 0)
            return answer;
        cVal *= cVal;
    }
}

IntegerComplex getPrime1mod4Root(long long p)
{
    const long long pm1 = p - 1;
    const long long pm1o2 = pm1 / 2;
    long long a;
    for (size_t i = 0;; i++) {
        if (modularPow(PrimesList::get(i), pm1o2, p) == pm1) {
            a = modularPow(PrimesList::get(i), pm1o2 / 2, p);
            break;
        }
    }
    long long b = 1;
    while (true) {
        long long k = (a * a + b * b) / p;
        if (k == 1) {
            return IntegerComplex(a, -b);
        }
        long long ap = a % k;
        long long bp = b % k;
        long long an = (a * ap + b * bp) / k;
        b = std::abs(a * bp - b * ap) / k;
        a = an;
    }
}

std::vector<long long> getAllPrimeFactors(long long n)
{
    n = std::abs(n);
    std::vector<long long> answer;
    long long rootN = lround(std::ceil(std::sqrt(n)));
    PrimesList::generatePrimesUpToN(rootN + 1);
    for (long long i = 0; PrimesList::get(i) * PrimesList::get(i) <= n; i++) {
        while (n % PrimesList::get(i) == 0) {
            n /= PrimesList::get(i);
            answer.push_back(PrimesList::get(i));
        }
    }
    if (n != 1) {
        answer.push_back(n);
    }
    return answer;
}

std::vector<IntegerComplex> getAllPrimeFactors(IntegerComplex n)
{
    long long norm = n.absSquared();
    std::vector<long long> normPFactors = getAllPrimeFactors(norm);
    size_t i = 0;
    std::vector<IntegerComplex> answer;
    while (i < normPFactors.size() && normPFactors[i] == 2) {
        if (n % IntegerComplex(1, -1) == 0) {
            n /= IntegerComplex(1, -1);
            answer.push_back(IntegerComplex(1, -1));
        } else {
            n /= IntegerComplex(1, 1);
            answer.push_back(IntegerComplex(1, 1));
        }
        i++;
    }
    for (; i < normPFactors.size(); i++) {
        if (normPFactors[i] % 4 == 3) {
            answer.push_back(normPFactors[i]);
            n /= normPFactors[i];
            i++;
        } else {
            IntegerComplex primeRoot = getPrime1mod4Root(normPFactors[i]);
            if (n % primeRoot == 0) {
                n /= primeRoot;
                answer.push_back(primeRoot);
            } else {
                primeRoot = primeRoot.conj();
                n /= primeRoot;
                answer.push_back(primeRoot);
            }
        }
    }
    return answer;
}

template <class T>
std::vector<std::pair<T, T>> getAllFactorsDivs(T n, T n2)
{
    static_assert(std::is_same<T, long long>::value || std::is_same<T, IntegerComplex>::value, "Only long long and IntegerComplex are supported");
    std::vector<T> primeFactorsDuplicates = getAllPrimeFactors(n);
    std::vector<long long> maxFactor;
    std::vector<T> primeFactors;
    if constexpr (std::is_same<T, long long>::value) {
        maxFactor = { 1 };
        primeFactors = { -1 };
    } else {
        maxFactor = { 3 };
        primeFactors = { T(0, 1) };
    }
    for (auto cPrime : primeFactorsDuplicates) {
        if (cPrime == primeFactors.back()) {
            ++maxFactor.back();
        } else {
            maxFactor.push_back(1);
            primeFactors.push_back(cPrime);
        }
    }
    primeFactorsDuplicates = getAllPrimeFactors(n2);
    std::vector<long long> minFactor(primeFactors.size(), 0);
    size_t i2 = 0;
    for (auto cPrime : primeFactorsDuplicates) {
        while (primeFactors[i2] < cPrime) {
            ++i2;
        }
        if (primeFactors[i2] == cPrime) {
            ++minFactor[i2];
        } else if (cPrime == primeFactors.back()) {
            ++minFactor.back();
        } else {
            maxFactor.push_back(0);
            minFactor.push_back(1);
            primeFactors.push_back(cPrime);
        }
    }
    std::vector<std::pair<T, T>> answers = { std::make_pair(1, 1) };
    std::vector<long long> factorsUsed(primeFactors.size(), 0);
    while (true) {
        auto cAnswer = answers.back();
        for (size_t i = 0;; i++) {
            if (i == factorsUsed.size()) {
                return answers;
            }
            if (factorsUsed[i] >= 0) {
                if (factorsUsed[i] == maxFactor[i]) {
                    std::get<0>(cAnswer) /= pow(primeFactors[i], maxFactor[i]);
                    if (minFactor[i] == 0) {
                        factorsUsed[i] = 0;
                    } else {
                        factorsUsed[i] = -1;
                        std::get<1>(cAnswer) *= primeFactors[i];
                        answers.push_back(cAnswer);
                        break;
                    }
                } else {
                    factorsUsed[i]++;
                    std::get<0>(cAnswer) *= primeFactors[i];
                    answers.push_back(cAnswer);
                    break;
                }
            } else {
                if (-factorsUsed[i] == minFactor[i]) {
                    std::get<1>(cAnswer) /= pow(primeFactors[i], minFactor[i]);
                    factorsUsed[i] = 0;
                } else {
                    factorsUsed[i]--;
                    std::get<1>(cAnswer) *= primeFactors[i];
                    answers.push_back(cAnswer);
                    break;
                }
            }
        }
    }
}

template <class T>
std::vector<std::unique_ptr<Expression>> getAllRationalPolynomialRootsT(std::vector<T>& R)
{
    static_assert(std::is_same<T, long long>::value || std::is_same<T, IntegerComplex>::value, "Only long long and IntegerComplex are supported");
    if constexpr (std::is_same<T, IntegerComplex>::value) {
        std::vector<long long> realV;
        for (auto i : R) {
            if (i.getImaginary() != 0) {
                break;
            }
            realV.push_back(i.getReal());
        }
        if (R.size() == realV.size()) {
            auto answer = getAllRationalPolynomialRootsT(realV);
            R.clear();
            for (auto i : realV) {
                R.push_back(IntegerComplex(i));
            }
            return answer;
        }
    }
    std::vector<std::unique_ptr<Expression>> results;
    for (auto co : getAllFactorsDivs(R.back(), R.front())) {
        auto mpv = std::get<0>(co);
        auto qv = std::get<1>(co);
        bool doAdd = true;
        while (true) {
            std::vector<T> newTermsC;
            T h(0);
            for (auto i : R) {
                h *= mpv;
                if (h % qv != 0) {
                    break;
                }
                h /= qv;
                h += i;
                newTermsC.push_back(h);
            }
            if (newTermsC.size() == R.size() && newTermsC.back() == 0) {
                R = newTermsC;
                if (doAdd) {
                    if constexpr (std::is_same<T, long long>::value) {
                        results.push_back(std::make_unique<Divide<Real>>(Real(1.0 * mpv), Real(1.0 * qv)));
                    } else {
                        auto top = mpv * qv.conj();
                        long long bottom = qv.absSquared();
                        results.push_back(std::make_unique<Divide<Expression>>(*top.getExpression(), Real(1.0 * bottom)));
                    }
                    doAdd = false;
                }
                do {
                    R.pop_back();
                } while (R.back() == 0);
                if (R.size() <= 1) {
                    break;
                }
            } else {
                break;
            }
        }
        if (R.size() <= 1) {
            break;
        }
    }
    return results;
}
std::vector<std::unique_ptr<Expression>> getAllRationalPolynomialRoots(std::vector<IntegerComplex>& C)
{
    return getAllRationalPolynomialRootsT(C);
}
} // Oasis::Util

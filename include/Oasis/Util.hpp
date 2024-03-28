#ifndef OASIS_UTIL_HPP
#define OASIS_UTIL_HPP
#include "Oasis/Real.hpp"
namespace Oasis::Util {
class IntegerComplex {
public:
    IntegerComplex(long long R)
        : real(R)
        , imaginary(0)
    {
    }
    IntegerComplex(const IntegerComplex& rhs) = default;
    IntegerComplex(long long R, long long I)
        : real(R)
        , imaginary(I)
    {
    }
    long long getReal() { return real; }
    long long getImaginary() { return imaginary; }
    std::unique_ptr<Expression> getExpression() const;
    long long absSquared() const { return real * real + imaginary * imaginary; }
    IntegerComplex conj() const { return IntegerComplex(real, -imaginary); }
    bool operator==(IntegerComplex rhs) { return imaginary == rhs.imaginary && real == rhs.real; }
    IntegerComplex& operator+=(const IntegerComplex& rhs);
    IntegerComplex& operator*=(const IntegerComplex& rhs);
    IntegerComplex& operator-=(const IntegerComplex& rhs);
    IntegerComplex& operator/=(const IntegerComplex& rhs);
    IntegerComplex& operator%=(const IntegerComplex& rhs);
    bool operator<(const IntegerComplex& rhs) const;

private:
    long long real;
    long long imaginary;
};

class PrimesList {
public:
    static long long get(size_t n) { return primes[n]; }
    static void generatePrimesUpToN(long long num);

private:
    static std::vector<long long> primes;
};

IntegerComplex operator+(IntegerComplex lhs, const IntegerComplex& rhs);
IntegerComplex operator-(IntegerComplex lhs, const IntegerComplex& rhs);
IntegerComplex operator*(IntegerComplex lhs, const IntegerComplex& rhs);
IntegerComplex operator/(IntegerComplex lhs, const IntegerComplex& rhs);
IntegerComplex operator%(IntegerComplex lhs, const IntegerComplex& rhs);

long long gcf(long long a, long long b);
bool isInt(double n);
Oasis::Real abs(const Oasis::Expression& exp);
std::vector<std::unique_ptr<Expression>> getAllRationalPolynomialRoots(std::vector<IntegerComplex>& C);
} // Oasis::Util

#endif // OASIS_UTIL_HPP
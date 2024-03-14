#ifndef OASIS_UTIL_HPP
#define OASIS_UTIL_HPP
#include "Oasis/Real.hpp"
namespace Oasis::Util {
long long gcf(long long a, long long b);
bool isInt(double n);
Oasis::Real abs(const Oasis::Expression& exp);
} // Oasis::Util

#endif // OASIS_UTIL_HPP
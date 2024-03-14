#include "Oasis/Util.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include <algorithm>
#include <cmath>
namespace Oasis::Util {
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
} // Oasis::Util

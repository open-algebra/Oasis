#ifndef OASIS_MATHUTILS_HPP
#define OASIS_MATHUTILS_HPP

#include <cmath>
#include <memory> 
#include "Expression.hpp" 

namespace Oasis {

class MathUtils {
public:
    static bool isNumericConstant(const std::unique_ptr<Expression>& expr);
    static double toNumeric(const std::unique_ptr<Expression>& expr);
    static bool equals(double a, double b, double tolerance = 1e-9) {
        return std::abs(a - b) < tolerance;
    }
    static std::unique_ptr<Expression> createNumericExpression(double value);
    static constexpr double PI = 3.14159265358979323846;
};

} // namespace Oasis

#endif // OASIS_MATHUTILS_HPP

/**
 * Created by Andrew Nazareth on 6/25/24.
 */

#include "Oasis/EulerNumber.hpp"
#include "numbers"
#include "string"
#include <cmath>

namespace Oasis {

auto EulerNumber::Equals(const Expression& other) const -> bool
{
    return other.Is<EulerNumber>();
}

auto EulerNumber::GetValue() -> double
{
    return std::numbers::e;
}

}
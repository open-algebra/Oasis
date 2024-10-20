//
// Created by Andrew Nazareth on 6/25/24.
//

#include "Oasis/Pi.hpp"
#include "string"
#include <numbers>

namespace Oasis {

auto Pi::Equals(const Expression& other) const -> bool
{
    return other.Is<Pi>() == dynamic_cast<const Pi&>(other).Is<Pi>();
}



auto Pi::GetValue() -> double
{
    return std::numbers::pi;
}

}
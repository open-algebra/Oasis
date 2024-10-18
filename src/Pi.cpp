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

auto Pi::Specialize(const Expression& other) -> std::unique_ptr<Pi>
{
    return other.Is<Pi>() ? std::make_unique<Pi>(dynamic_cast<const Pi&>(other)) : nullptr;
}

auto Pi::GetValue() -> double
{
    return std::numbers::pi;
}

}
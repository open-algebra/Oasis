//
// Created by Andrew Nazareth on 6/25/24.
//

#include "Oasis/EulerNumber.hpp"
#include "numbers"
#include "string"
#include <cmath>

namespace Oasis {

auto EulerNumber::Equals(const Expression& other) const -> bool
{
    return other.Is<EulerNumber>();
}

auto EulerNumber::Specialize(const Expression& other) -> std::unique_ptr<EulerNumber>
{
    return other.Is<EulerNumber>() ? std::make_unique<EulerNumber>(dynamic_cast<const EulerNumber&>(other)) : nullptr;
}

auto EulerNumber::GetValue() -> double
{
    return std::numbers::e;
}

}
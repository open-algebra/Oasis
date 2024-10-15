//
// Created by Andrew Nazareth on 10/10/23.
//

#include "Oasis/Imaginary.hpp"
#include "string"

namespace Oasis {

auto Imaginary::Equals(const Expression& other) const -> bool
{
    return other.Is<Imaginary>();
}

auto Imaginary::Specialize(const Expression& other) -> std::unique_ptr<Imaginary>
{
    return other.Is<Imaginary>() ? std::make_unique<Imaginary>(dynamic_cast<const Imaginary&>(other)) : nullptr;
}

}
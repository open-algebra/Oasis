//
// Created by Blake Kessler on 10/13/23.
//

#include "Oasis/Undefined.hpp"

namespace Oasis {

auto Undefined::ToString() const -> std::string
{
    return "Undefined";
}

auto Undefined::Specialize(const Expression& other) -> std::unique_ptr<Undefined>
{
    return other.Is<Undefined>() ? std::make_unique<Undefined>() : nullptr;
}

auto Undefined::Specialize(const Expression& other, tf::Subflow&) -> std::unique_ptr<Undefined>
{
    return other.Is<Undefined>() ? std::make_unique<Undefined>() : nullptr;
}

} // namespace Oasis
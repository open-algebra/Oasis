/**
 * Created by Andrew Nazareth on 10/10/23.
 */

#include "Oasis/Imaginary.hpp"
#include "string"

namespace Oasis {

auto Imaginary::Equals(const Expression& other) const -> bool
{
    return other.Is<Imaginary>();
}

}
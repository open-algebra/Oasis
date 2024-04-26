//
// Created by Andrew Nazareth on 10/10/23.
//

#include "Oasis/Imaginary.hpp"
#include "string"

namespace Oasis {

auto Imaginary::Equals(const Expression& other) const -> bool
{
    return other.Is<Imaginary>() == dynamic_cast<const Imaginary&>(other).Is<Imaginary>();
}

auto Imaginary::ToString() const -> std::string
{
    return "i";
}

auto Imaginary::ToMathMLElement(tinyxml2::XMLDocument& doc) const -> tinyxml2::XMLElement*
{
    tinyxml2::XMLElement* const element = doc.NewElement("mi");
    element->SetText(ToString().c_str());
    return element;
}

auto Imaginary::Specialize(const Expression& other) -> std::unique_ptr<Imaginary>
{
    return other.Is<Imaginary>() ? std::make_unique<Imaginary>(dynamic_cast<const Imaginary&>(other)) : nullptr;
}

auto Imaginary::Specialize(const Expression& other, tf::Subflow&) -> std::unique_ptr<Imaginary>
{
    return other.Is<Imaginary>() ? std::make_unique<Imaginary>(dynamic_cast<const Imaginary&>(other)) : nullptr;
}

}
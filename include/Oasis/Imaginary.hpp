//
// Created by Andrew Nazareth on 10/10/23.
//

#ifndef OASIS_IMAGINARY_HPP
#define OASIS_IMAGINARY_HPP

#include "LeafExpression.hpp"

namespace Oasis {

/**
 * An 'imaginary' number.
 */
class Imaginary : public LeafExpression<Imaginary> {
public:
    Imaginary() = default;
    Imaginary(const Imaginary& other) = default;

    [[nodiscard]] auto Equals(const Expression& other) const -> bool final;

    EXPRESSION_TYPE(Imaginary)
    EXPRESSION_CATEGORY(UnExp)

    [[nodiscard]] auto ToString() const -> std::string final;

    static auto Specialize(const Expression& other) -> std::unique_ptr<Imaginary>;
    static auto Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Imaginary>;
};
}

#endif // OASIS_IMAGINARY_HPP

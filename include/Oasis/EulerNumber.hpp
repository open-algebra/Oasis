//
// Created by Andrew Nazareth on 6/25/24.
//

#ifndef OASIS_EULERNUMBER_HPP
#define OASIS_EULERNUMBER_HPP

#include "LeafExpression.hpp"

namespace Oasis {

/**
 * The EulerNumber class is a representation of the Euler Number.
 * It may be used when the Euler number is desired in computations.
 *
 * @section Parameters
 * No parameters are needed for the EulerNumber class.
 *
 * @section Examples
 *
 * @subsection simple Simple usage:
 * @code
 *  auto e = Oasis::EulerNumber{};

    Oasis::InFixSerializer result;

    auto resultant = e.Simplify();

    std::println("Result: {}", resultant->Accept(result).value());
    // Will print: e
 * @endcode
 *
 * @subsection comp Usage in other components:
 * @code
 * Oasis::InFixSerializer result;

    Oasis::Add myAddition {
        Oasis::EulerNumber{},
        Oasis::Variable{"x"}
    };

    auto resultant = myAddition.Simplify();
    std::println("Result: {}", resultant->Accept(result).value());
    // Will print: (e+x)
 * @endcode
 *
 * Another example shows its usage inside of the Exponent class before being passed into the Integral class.
 * @code
 * Oasis::InFixSerializer result;

    Oasis::Exponent eul {
        Oasis::EulerNumber{},
        Oasis::Variable{"x"}
    };

    Oasis::Integral in {
        eul,
        Oasis::Variable{"x"}
    };

    auto resultant = in.Simplify();
    std::println("Result: {}", resultant->Accept(result).value());
    // Will print: in((e^x),x)
 * @endcode
 */
class EulerNumber : public LeafExpression<EulerNumber> {
public:
    EulerNumber() = default;
    EulerNumber(const EulerNumber& other) = default;

    [[nodiscard]] auto Equals(const Expression& other) const -> bool final;

    [[nodiscard]] auto Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto Differentiate(const Expression&) const -> std::unique_ptr<Expression> final;

    EXPRESSION_TYPE(EulerNumber)
    EXPRESSION_CATEGORY(UnExp)

    static auto GetValue() -> double;
};
}

#endif // OASIS_EULERNUMBER_HPP

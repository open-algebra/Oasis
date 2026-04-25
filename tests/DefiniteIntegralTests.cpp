//
// Created by Justin Romanelli on 2/1/26.
//

#include "Oasis/Divide.hpp"
#include "Oasis/EulerNumber.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Expression.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Negate.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Variable.hpp"
#include "catch2/catch_test_macros.hpp"

TEST_CASE("Definite Integral with distinct bounds", "[Integrate][Real][Definite]")
{
    // Integrate 2x from x = 1 to x = 3
    // Result should be 8.

    Oasis::Variable x { "x" };

    Oasis::Multiply<Oasis::Real, Oasis::Variable> integrand {
        Oasis::Real { 2.0f }, x
    };

    Oasis::Real lowerBound { 1.0f }, upperBound { 3.0f }, answer { 8.0f };

    Oasis::Integral<Oasis::Expression, Oasis::Expression> integral { integrand, x };

    std::unique_ptr<Oasis::Expression> result = integral.IntegrateWithBounds(lowerBound, upperBound);

    REQUIRE(answer.Equals(*result));
}

TEST_CASE("Definite Integral with a non-trivial integrand", "[Integral][Definite][Real]")
{
    // Integrate (1/4) * x^3 from x = 2 to x = 4
    // Result should be 15
    Oasis::Variable x { "x" };

    Oasis::Multiply<> integrand = Oasis::Multiply<> {
        Oasis::Divide<Oasis::Real, Oasis::Real> {
            Oasis::Real { 1.0f }, Oasis::Real { 4.0f } },
        Oasis::Exponent<Oasis::Variable, Oasis::Real> {
            x, Oasis::Real { 3.0f } }
    };

    Oasis::Real lowerBound { 2.0f }, upperBound { 4.0f }, answer { 15.0f };

    Oasis::Integral<Oasis::Expression, Oasis::Expression> integral { integrand, x };

    std::unique_ptr<Oasis::Expression> result = integral.IntegrateWithBounds(lowerBound, upperBound);

    REQUIRE(answer.Equals(*result));
}

TEST_CASE("Definite Integral of 0", "[Integrate][Definite][Zero]")
{
    // Integrating 0 should always give 0
    // Check for that behavior here.

    Oasis::Variable x { "x" };
    Oasis::Real zero { 0.0f }, lowerBound { 2.0f }, upperBound { 5.0f }, answer { 0.0f };

    Oasis::Integral<Oasis::Expression, Oasis::Expression> integral { zero, x };

    std::unique_ptr<Oasis::Expression> result = integral.IntegrateWithBounds(lowerBound, upperBound);

    REQUIRE(answer.Equals(*result));
}

TEST_CASE("Definite Integral with equal bounds", "[Integrate][Real][Definite]")
{
    // Integrating with equal bounds should always give 0.
    // Check for that behavior here.

    Oasis::Variable x { "x" };

    Oasis::Exponent<Oasis::Variable, Oasis::Real> integrand {
        x, Oasis::Real { 2.0f }
    };

    Oasis::Real lowerBound { 3.0f }, upperBound { 3.0f }, answer { 0.0f };

    Oasis::Integral<Oasis::Expression, Oasis::Expression> integral { integrand, x };

    std::unique_ptr<Oasis::Expression> result = integral.IntegrateWithBounds(lowerBound, upperBound);

    REQUIRE(answer.Equals(*result));
}

TEST_CASE("Definite Integral with swapped bounds", "[Integral][Definite][Negative]")
{
    // An integral with swapped bounds (lowerBound > upperBound) should be negative
    // Integrating 4x from x = 6 to x = 2, answer should be -64

    Oasis::Variable x { "x" };

    Oasis::Multiply<Oasis::Real, Oasis::Variable> integrand {
        Oasis::Real { 4.0f }, x
    };

    Oasis::Real lowerBound { 6.0f }, upperBound { 2.0f }, answer { -64.0f };

    Oasis::Integral<Oasis::Expression, Oasis::Expression> integral { integrand, x };

    std::unique_ptr<Oasis::Expression> result = integral.IntegrateWithBounds(lowerBound, upperBound);

    REQUIRE(answer.Equals(*result));
}

TEST_CASE("Attempted Definite Integral of a non-integrable function", "[Integral][Definite]")
{
    // The Gaussian Integral does not have an antiderivative. While it has a defined value
    // for definite integrals, it requires tricks with integrals that can not be done with a computer.
    // By the implementation, we expect the result of IntegrateWithBounds() to be nullptr.

    Oasis::Variable x { "x" };

    Oasis::Exponent<> integrand {
        Oasis::EulerNumber(), Oasis::Exponent<> { Oasis::Negate<> { x }, Oasis::Real { 2.0f } }
    };

    // Bounds should not matter, as long as they are equal
    Oasis::Real lowerBound { 3.0f }, upperBound { 8.0f };

    Oasis::Integral<Oasis::Expression, Oasis::Expression> integral { integrand, x };

    std::unique_ptr<Oasis::Expression> result = integral.IntegrateWithBounds(lowerBound, upperBound);

    REQUIRE(result == nullptr);
}

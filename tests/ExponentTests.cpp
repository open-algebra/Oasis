//
// Created by Andrew Nazareth on 9/22/23.
//

#include "catch2/catch_test_macros.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Variable.hpp"

TEST_CASE("Zero Rule", "[Exponent][Zero]")
{
    Oasis::Exponent exponent {
        Oasis::Real { 5.0 },
        Oasis::Real { 0.0 }
    };

    auto simplified = exponent.Simplify();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 1.0);
}
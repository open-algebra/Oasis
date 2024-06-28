//
// Created by Andrew Nazareth on 6/28/24.
//

#include "catch2/catch_test_macros.hpp"
#include "Oasis/Magnitude.hpp"
#include "Oasis/Real.hpp"

TEST_CASE("Magnitude of Real", "[Magnitude][Real]")
{
    Oasis::Magnitude mag1{Oasis::Real{5.0}};
    Oasis::Magnitude mag2{Oasis::Real{-75.0}};

    auto simp1 = mag1.Simplify();
    auto simp2 = mag2.Simplify();

    REQUIRE(simp1->Equals(Oasis::Real{5.0}));
    REQUIRE(simp2->Equals(Oasis::Real{75.0}));
}
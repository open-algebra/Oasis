//
// Created by Andrew Nazareth on 10/8/24.
//

#include "catch2/catch_test_macros.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Variable.hpp"
#include "Oasis/Sine.hpp"
#include "Oasis/Pi.hpp"

TEST_CASE("Sine Test", "[Sine]")
{
    Oasis::Sine s1{Oasis::Pi{}};

    auto simplified = s1.Simplify();

    REQUIRE(simplified->Equals(Oasis::Real{0}));
}
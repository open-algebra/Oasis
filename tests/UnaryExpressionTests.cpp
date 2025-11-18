//
// Created by Matthew McCall on 4/30/24.
//

#include <catch2/catch_test_macros.hpp>

#include <Oasis/Add.hpp>
#include <Oasis/Multiply.hpp>
#include <Oasis/Negate.hpp>
#include <Oasis/Variable.hpp>

TEST_CASE("Substitute Unary", "[Substitute]")
{
    Oasis::Add before {
        Oasis::Multiply {
            Oasis::Real { 2.0 },
            Oasis::Negate { Oasis::Variable { "x" } } },
        Oasis::Multiply{
            Oasis::Real { 3.0 },
            Oasis::Variable { "x" } }
    }; // -2x+3x

    const auto after = before.Substitute(Oasis::Variable { "x" }, Oasis::Real { 4.0 }); // after should some std::unique_ptr<Expression> such that it equals 2(-4) + 3(4)
    REQUIRE(after->Equals(Oasis::Real { 4 }));
}
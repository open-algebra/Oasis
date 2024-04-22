//
// Created by Matthew McCall on 4/21/24.
//

#include "Oasis/Variable.hpp"

#include <Oasis/Add.hpp>
#include <catch2/catch_test_macros.hpp>

#include <Oasis/FromString.hpp>
#include <Oasis/Log.hpp>
#include <Oasis/Multiply.hpp>

TEST_CASE("In-Fix Parsing Works for Simple Trees", "[Sexp]")
{
    Oasis::Add expected {
        Oasis::Real { 1.0 },
        Oasis::Real { 2.0 }
    };

    const auto parsed = Oasis::FromInFix("1 + 2");

    REQUIRE(parsed->Equals(expected));
}

TEST_CASE("In-Fix Parsing Respects Order of Operations")
{
    const Oasis::Add expected {
        Oasis::Real { 1.0 },
        Oasis::Multiply {
            Oasis::Real { 2.0 },
            Oasis::Real { 3.0 } }
    };

    const auto parsed = Oasis::FromInFix("1 + 2 * 3");

    REQUIRE(parsed->Equals(expected));
}

TEST_CASE("In-Fix Parsing Works with Functions")
{
    const Oasis::Add expected {
        Oasis::Real { 1.0 },
        Oasis::Log {
            Oasis::Real { 2.0 },
            Oasis::Real { 3.0 } }
    };

    const auto parsed = Oasis::FromInFix("1 + log ( 2 , 3 )");

    REQUIRE(parsed->Equals(expected));
}

TEST_CASE("In-Fix Parsing Works with Variables")
{
    const Oasis::Add<> expected {
        Oasis::Multiply {
            Oasis::Real { 1.0 },
            Oasis::Variable { "x" } },
        Oasis::Multiply {
            Oasis::Variable { "y" },
            Oasis::Real { 3.0 } }
    };

    const auto parsed = Oasis::FromInFix("1x + y3");

    REQUIRE(parsed->Equals(expected));
}
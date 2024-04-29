//
// Created by Matthew McCall on 4/21/24.
//

#include "../../include/Oasis/Variable.hpp"

#include <../../cmake-build-debug/_deps/catch2-src/src/catch2/catch_test_macros.hpp>
#include <../../include/Oasis/Add.hpp>

#include <../../include/Oasis/Log.hpp>
#include <../../include/Oasis/Multiply.hpp>
#include <Oasis/FromString.hpp>

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
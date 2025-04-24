/**
 * Created by Matthew McCall on 4/21/24.
 */

#include "catch2/catch_test_macros.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/FromString.hpp"
#include "Oasis/Variable.hpp"

TEST_CASE("In-Fix Parsing Works for Simple Trees", "[Parsing]")
{
    Oasis::Add expected {
        Oasis::Real { 1.0 },
        Oasis::Real { 2.0 }
    };

    const auto result = Oasis::FromInFix("1 + 2");
    REQUIRE(result.has_value());

    const auto& parsed = result.value();
    REQUIRE(parsed->Equals(expected));
}

TEST_CASE("In-Fix Parsing Respects Order of Operations", "[Parsing]")
{
    const Oasis::Add expected {
        Oasis::Real { 1.0 },
        Oasis::Multiply {
            Oasis::Real { 2.0 },
            Oasis::Real { 3.0 } }
    };

    const auto result = Oasis::FromInFix("1 + 2 * 3");
    REQUIRE(result.has_value());

    const auto& parsed = result.value();
    REQUIRE(parsed->Equals(expected));
}

TEST_CASE("In-Fix Parsing Works with Functions", "[Parsing]")
{
    const Oasis::Add expected {
        Oasis::Real { 1.0 },
        Oasis::Log {
            Oasis::Real { 2.0 },
            Oasis::Real { 3.0 } }
    };

    const auto result = Oasis::FromInFix("1 + log ( 2 , 3 )");
    REQUIRE(result.has_value());

    const auto& parsed = result.value();
    REQUIRE(parsed->Equals(expected));
}

TEST_CASE("In-Fix Parsing Works with Variables", "[Parsing]")
{
    const Oasis::Add<> expected {
        Oasis::Multiply {
            Oasis::Real { 1.0 },
            Oasis::Variable { "x" } },
        Oasis::Multiply {
            Oasis::Variable { "y" },
            Oasis::Real { 3.0 } }
    };

    const auto preprocessed = Oasis::PreProcessInFix("1x+y3");
    const auto result = Oasis::FromInFix(preprocessed);
    REQUIRE(result.has_value());


    const auto& parsed = result.value();
    REQUIRE(parsed->Equals(expected));
}

TEST_CASE("In-Fix Preprocessor Works", "[Parsing]")
{
    const auto result = Oasis::PreProcessInFix("1+2");
    REQUIRE(result == "1 + 2");
}

TEST_CASE("In-Fix Preprocessor Works with Implicit Multiplication", "[Parsing]")
{
    const auto result = Oasis::PreProcessInFix("2x+3x");
    REQUIRE(result == "2 * x + 3 * x");
}

TEST_CASE("In-Fix Preprocessor Works with Implicit Multiplication and Functions", "[Parsing]")
{
    const auto result = Oasis::PreProcessInFix("2x+3x+log(10,x)");
    REQUIRE(result == "2 * x + 3 * x + log ( 10 , x ) ");
}
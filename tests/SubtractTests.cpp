//
// Created by Matthew McCall on 8/10/23.
//

#include "catch2/catch_test_macros.hpp"

#include "Oasis/Imaginary.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Subtract.hpp"

TEST_CASE("Subtraction", "[Subtract]")
{
    Oasis::Subtract subtract {
        Oasis::Subtract {
            Oasis::Real { 1.0 },
            Oasis::Real { 2.0 } },
        Oasis::Real { 3.0 }
    };

    auto simplified = subtract.Simplify();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == -4.0);
}

TEST_CASE("Generalized Subtraction", "[Subtract][Generalized]")
{
    Oasis::Subtract<Oasis::Expression> subtract {
        Oasis::Subtract<Oasis::Expression> {
            Oasis::Real { 1.0 },
            Oasis::Real { 2.0 } },
        Oasis::Real { 3.0 }
    };

    auto simplified = subtract.Simplify();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == -4.0);
}

TEST_CASE("Subtraction Async", "[Subtract][Async]")
{
    Oasis::Subtract subtract {
        Oasis::Subtract {
            Oasis::Real { 1.0 },
            Oasis::Real { 2.0 } },
        Oasis::Real { 3.0 }
    };

    std::unique_ptr<Oasis::Expression> simplified = subtract.SimplifyAsync();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == -4.0);
}

TEST_CASE("Imaginary Subtration", "[Subtract][Imaginary]")
{
    Oasis::Subtract s1 {
        Oasis::Multiply {
            Oasis::Real { 3.0 },
            Oasis::Imaginary {} },
        Oasis::Imaginary {}
    };

    auto spec1 = s1.Simplify();

    REQUIRE(Oasis::Multiply { Oasis::Real { 2.0 }, Oasis::Imaginary {} }.Equals(*spec1));
}

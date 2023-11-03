//
// Created by Matthew McCall on 8/10/23.
//

#include "catch2/catch_test_macros.hpp"

#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"

TEST_CASE("Multiplication", "[Multiply]")
{
    Oasis::Multiply subtract {
        Oasis::Multiply {
            Oasis::Real { 1.0 },
            Oasis::Real { 2.0 } },
        Oasis::Real { 3.0 }
    };

    auto simplified = subtract.Simplify();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 6.0);
}

TEST_CASE("Generalized Multiplication", "[Multiply][Generalized]")
{
    Oasis::Multiply<Oasis::Expression> subtract {
        Oasis::Multiply<Oasis::Expression> {
            Oasis::Real { 1.0 },
            Oasis::Real { 2.0 } },
        Oasis::Real { 3.0 }
    };

    auto simplified = subtract.Simplify();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 6.0);
}

TEST_CASE("Multiplication Async", "[Multiply][Async]")
{
    Oasis::Multiply subtract {
        Oasis::Multiply {
            Oasis::Real { 1.0 },
            Oasis::Real { 2.0 } },
        Oasis::Real { 3.0 }
    };

    std::unique_ptr<Oasis::Expression> simplified = subtract.SimplifyAsync();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 6.0);
}

TEST_CASE("Imaginary Multiplication", "[Imaginary][Multiplication]")
{
    Oasis::Multiply i2 {
        Oasis::Imaginary {},
        Oasis::Imaginary {}
    };
    Oasis::Multiply i3 {
        Oasis::Exponent {
            Oasis::Imaginary {},
            Oasis::Real { 2.0 } },
        Oasis::Imaginary {}
    };
    Oasis::Multiply i4 {
        Oasis::Exponent {
            Oasis::Imaginary {},
            Oasis::Real { 2.0 } },
        Oasis::Exponent {
            Oasis::Imaginary {},
            Oasis::Real { 2.0 } }
    };

    auto simplified2 = i2.Simplify();
    auto simplified3 = i3.Simplify();
    auto simplified4 = i4.Simplify();

    REQUIRE(Oasis::Multiply { Oasis::Real { -1 }, Oasis::Imaginary {} }.Equals(*simplified3));
    REQUIRE(Oasis::Real { -1.0 }.Equals(*simplified2));
    REQUIRE(Oasis::Real { 1.0 }.Equals(*simplified4));
}
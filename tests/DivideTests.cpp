//
// Created by Matthew McCall on 8/10/23.
//

#include "catch2/catch_test_macros.hpp"

#include "Oasis/Divide.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Variable.hpp"
#include "Oasis/Multiply.hpp"

TEST_CASE("Division", "[Divide]")
{
    Oasis::Divide subtract {
        Oasis::Divide {
            Oasis::Real { 2.0 },
            Oasis::Real { 1.0 } },
        Oasis::Real { 2.0 }
    };

    auto simplified = subtract.Simplify();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 1.0);
}


TEST_CASE("Symbolic Division", "[Division][Symbolic]")
{
    Oasis::Divide div {
        Oasis::Multiply {
            Oasis::Real { 2.0 },
            Oasis::Variable { "x"} },
        Oasis::Multiply {
                Oasis::Real { 1.0 },
                Oasis::Variable { "x" } }
        };


    Oasis::Divide div2 {
        Oasis::Multiply {
            Oasis::Real { 2.0 },
            Oasis::Multiply {
                Oasis::Multiply{
                    Oasis::Variable{ "y" },
                    Oasis::Variable{ "z" }
                },
                Oasis::Variable{ "x" }
            } },
        Oasis::Multiply {
                Oasis::Real { 1.0 },
                Oasis::Multiply {
                Oasis::Multiply{
                    Oasis::Variable{ "y" },
                    Oasis::Variable{ "z" }
                },
                Oasis::Variable{ "x" }
            } }
        };

    auto simplified = div.Simplify();
    REQUIRE(simplified->Is<Oasis::Real>());
    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 2.0);

    auto simplified2 = div2.Simplify();
    REQUIRE(simplified2->Is<Oasis::Real>());
    auto simplifiedReal2 = dynamic_cast<Oasis::Real&>(*simplified2);
    REQUIRE(simplifiedReal2.GetValue() == 2.0);
}

TEST_CASE("Symbolic Division, variables dont equal", "[Division][Symbolic]")
{
    Oasis::Divide div {
        Oasis::Multiply {
            Oasis::Real { 4.0 },
            Oasis::Variable { "x"} },
        Oasis::Multiply {
                Oasis::Real { 2.0 },
                Oasis::Variable { "y" } }
        };

    auto simplified = div.Simplify();

    CAPTURE(simplified->ToString());
    REQUIRE(Oasis::Divide {
        Oasis::Multiply {
            Oasis::Real { 2.0 },
            Oasis::Variable { "x"} },
        Oasis::Variable { "y" } }.Equals(*simplified));
}


/*
TEST_CASE("DivisionTest", "[Divide]")
{
    Oasis::Divide test {
        Oasis::Real { 2.0 },
        Oasis::Variable { "x" } 
    };

    auto simplified = test.Simplify();
    //REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 1.0);
}*/

TEST_CASE("Generalized Division", "[Divide][Generalized]")
{
    Oasis::Divide<Oasis::Expression> subtract {
        Oasis::Divide<Oasis::Expression> {
            Oasis::Real { 2.0 },
            Oasis::Real { 1.0 } },
        Oasis::Real { 2.0 }
    };

    auto simplified = subtract.Simplify();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 1.0);
}

TEST_CASE("Division Async", "[Divide][Async]")
{
    Oasis::Divide subtract {
        Oasis::Divide {
            Oasis::Real { 2.0 },
            Oasis::Real { 1.0 } },
        Oasis::Real { 2.0 }
    };

    std::unique_ptr<Oasis::Expression> simplified = subtract.SimplifyAsync();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 1.0);
}
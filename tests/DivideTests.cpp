//
// Created by Matthew McCall on 8/10/23.
//

#include "catch2/catch_test_macros.hpp"

#include "Oasis/Divide.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Variable.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Add.hpp"

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

TEST_CASE("Symbolic Division, equal variables", "[Division][Symbolic]")
{
    Oasis::Divide div {
        Oasis::Multiply {
            Oasis::Real { 2.0 },
            Oasis::Variable { "x" } },
        Oasis::Multiply {
                Oasis::Real { 1.0 },
            Oasis::Variable { "x" } }
    };


    Oasis::Divide div2 {
        Oasis::Multiply {
            Oasis::Real { 2.0 },
            Oasis::Multiply {
                Oasis::Variable{ "z" },
                Oasis::Multiply{
                    Oasis::Variable{ "y" },
                    Oasis::Variable{ "x" }
                }
            } },
        Oasis::Multiply {
                Oasis::Real { 1.0 },
                Oasis::Multiply {
                    Oasis::Variable{ "y" },
                    Oasis::Multiply{
                        Oasis::Variable{ "x" },
                        Oasis::Variable{ "z" }
                    }
            }   }
        };

    auto simplified = div.Simplify();
    REQUIRE(simplified->Is<Oasis::Real>());
    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 2.0);


    auto simplified2 = div2.Simplify();
    CAPTURE(simplified2->ToString());
    REQUIRE(simplified2->Is<Oasis::Real>());
    auto simplifiedReal2 = dynamic_cast<Oasis::Real&>(*simplified2);
    REQUIRE(simplifiedReal2.GetValue() == 2.0);
}


TEST_CASE("Symbolic Division, unequal variables", "[Division][Symbolic]")
{
    Oasis::Divide div {
        Oasis::Multiply {
            Oasis::Real { 4.0 },
            Oasis::Multiply {
                Oasis::Variable { "z" },
                Oasis::Variable { "x" } }},
        Oasis::Multiply {
                Oasis::Real { 2.0 },
                Oasis::Multiply {
                    Oasis::Variable { "y" },
                    Oasis::Variable { "z" } }}
        };

    auto simplified = div.Simplify();

    CAPTURE(simplified->ToString());
    REQUIRE(Oasis::Divide {
        Oasis::Multiply {
            Oasis::Real { 2.0 },
            Oasis::Variable { "x"} },
        Oasis::Variable { "y" } }.Equals(*simplified));
}


TEST_CASE("Symbolic Division of Expressions", "[Division][Symbolic]")
{
    //4(z^2+1)(x+1)/2(x+1)=2(z^2+1)

    Oasis::Divide div {
        Oasis::Multiply{
            Oasis::Multiply {
                Oasis::Add {
                    Oasis::Variable { "x" },
                    Oasis::Real { 1 } },
                Oasis::Add{
                    Oasis::Exponent {
                        Oasis::Variable { "z" },
                        Oasis::Real { 2 } },
                    Oasis::Real{ 1 }}},
            Oasis::Real { 4.0 }
        },
        Oasis::Multiply {
                Oasis::Real { 2.0 },
                Oasis::Add {
                    Oasis::Variable { "x" },
                    Oasis::Real { 1 } }}
        };

    auto simplified = div.Simplify();

    CAPTURE(simplified->ToString());
    REQUIRE(Oasis::Multiply {
        Oasis::Real{ 2 },
        Oasis::Add{
            Oasis::Exponent {
                Oasis::Variable { "z" },
                Oasis::Real { 2 } },
            Oasis::Real{ 1 }},
        }.Equals(*simplified));
}


TEST_CASE("Symbolic Division, unequal exponents", "[Division][Symbolic]")
{
    Oasis::Divide div {
        Oasis::Multiply {
            Oasis::Real { 4.0 },
            Oasis::Exponent {
                Oasis::Variable { "z" },
                Oasis::Real { 2 } }},
        Oasis::Multiply {
                Oasis::Real { 2.0 },
                Oasis::Multiply {
                    Oasis::Variable { "y" },
                    Oasis::Variable { "z" } }}
        };

    auto simplified = div.Simplify();

    CAPTURE(simplified->ToString());
    REQUIRE(Oasis::Divide {
        Oasis::Multiply {
            Oasis::Real { 2.0 },
            Oasis::Variable { "z"} },
        Oasis::Variable { "y" } }.Equals(*simplified));
}


TEST_CASE("Symbolic Division, equal exponents", "[Division][Symbolic]")
{
    Oasis::Divide div {
        Oasis::Multiply {
            Oasis::Real { 4.0 },
            Oasis::Exponent {
                Oasis::Variable { "z" },
                Oasis::Real { 3 } }},
        Oasis::Multiply {
                Oasis::Real { 2.0 },
                Oasis::Multiply {
                    Oasis::Variable { "y" },
                    Oasis::Variable { "z" } }}
        };

    auto simplified = div.Simplify();

    CAPTURE(simplified->ToString());
    REQUIRE(Oasis::Divide {
        Oasis::Multiply {
            Oasis::Real { 2.0 },
            Oasis::Exponent {
                Oasis::Variable { "z" },
                Oasis::Real { 2 } } },
        Oasis::Variable { "y" } }.Equals(*simplified));
}

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
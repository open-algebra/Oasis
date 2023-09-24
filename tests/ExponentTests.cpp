//
// Created by Andrew Nazareth on 9/22/23.
//

#include "catch2/catch_test_macros.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Variable.hpp"

TEST_CASE("Zero Rule", "[Exponent][Zero]")
{
    Oasis::Exponent exponent {
        Oasis::Real { 5.0 },
        Oasis::Real { 0.0 }
    };

    auto simplified = exponent.Simplify();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 1.0);
}

TEST_CASE("Symbolic Zero Rule", "[Exponent][Zero][Symbolic]")
{
    Oasis::Exponent exponent {
        Oasis::Variable { "x" },
        Oasis::Real { 0.0 }
    };

    auto simplified = exponent.Simplify();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 1.0);
}

TEST_CASE("Whole Number Exponentiation", "[Exponent][Numerical]")
{
    Oasis::Exponent exponent {
        Oasis::Real { 5.0 },
        Oasis::Real { 2.0 }
    };

    auto simplified = exponent.Simplify();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 25.0);
}

TEST_CASE("Fractional Exponentiation", "[Exponent][Root]")
{
    Oasis::Exponent exponent {
        Oasis::Real { 4.0 },
        Oasis::Real { 0.5 }
    };

    auto simplified = exponent.Simplify();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 2.0);
}

TEST_CASE("Equivalence", "[Exponent][Equivalence]")
{
    Oasis::Exponent exp1 {
        Oasis::Variable { "x" },
        Oasis::Real { 0.5 }
    };
    Oasis::Exponent exp2 {
        Oasis::Variable { "x" },
        Oasis::Real { 0.5 }
    };
    Oasis::Exponent exp3 {
        Oasis::Variable { "x" },
        Oasis::Real { 2 }
    };

    REQUIRE(exp1.Equals(exp2));
    REQUIRE(!exp1.Equals(exp3));
}

TEST_CASE("Addition of Exponents", "[Add][Exponent][Symbolic]")
{
    Oasis::Add add {
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } },
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } }
    };

    auto simplified = add.Simplify();
    REQUIRE(simplified->Is<Oasis::Multiply<Oasis::Real, Oasis::Exponent<Oasis::Variable, Oasis::Real>>>());

    REQUIRE(Oasis::Multiply {
        Oasis::Real { 2.0 },
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } } }
                .Equals(*simplified));
}

TEST_CASE("Variable Multiplication", "[Exponent][Variable][Multiplication]")
{
    Oasis::Multiply expr = Oasis::Multiply {
        Oasis::Variable { "x" },
        Oasis::Variable { "x" }
    };

    auto simplified = expr.Simplify();

    REQUIRE(simplified->Is<Oasis::Exponent<Oasis::Variable, Oasis::Real>>());
    REQUIRE(simplified->Equals(Oasis::Exponent<Oasis::Variable, Oasis::Real> { Oasis::Variable { "x" }, Oasis::Real { 2.0 } }));
}

TEST_CASE("Variable with power Multiplication", "[Exponent][Variable][Multiplication]")
{
    Oasis::Multiply expr = Oasis::Multiply {
        Oasis::Variable { "x" },
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 3.0 } }
    };

    Oasis::Multiply expr2 = Oasis::Multiply {
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 3.0 } },
        Oasis::Variable { "x" }
    };

    Oasis::Multiply expr3 = Oasis::Multiply {
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } },
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } }
    };

    auto simplified = expr.Simplify();
    auto simplified2 = expr2.Simplify();
    auto simplified3 = expr3.Simplify();

    REQUIRE(simplified->Is<Oasis::Exponent<Oasis::Variable, Oasis::Real>>());
    REQUIRE(simplified->Equals(Oasis::Exponent<Oasis::Variable, Oasis::Real> { Oasis::Variable { "x" }, Oasis::Real { 4.0 } }));
    REQUIRE(simplified2->Equals(Oasis::Exponent<Oasis::Variable, Oasis::Real> { Oasis::Variable { "x" }, Oasis::Real { 4.0 } }));
    REQUIRE(simplified3->Equals(Oasis::Exponent<Oasis::Variable, Oasis::Real> { Oasis::Variable { "x" }, Oasis::Real { 4.0 } }));
}

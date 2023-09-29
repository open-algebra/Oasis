//
// Created by Andrew Nazareth on 9/22/23.
//

#include "catch2/catch_test_macros.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Subtract.hpp"
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
    Oasis::Add add1 {
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } },
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } }
    };

    Oasis::Add add2 {
        Oasis::Multiply {
            Oasis::Real { 2.0 },
            Oasis::Exponent {
                Oasis::Variable { "x" },
                Oasis::Real { 2.0 } } },
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } }
    };

    Oasis::Add add3 {
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } },
        Oasis::Multiply {
            Oasis::Real { 2.0 },
            Oasis::Exponent {
                Oasis::Variable { "x" },
                Oasis::Real { 2.0 } } }
    };

    Oasis::Add add4 {
        Oasis::Multiply {
            Oasis::Real { 2.0 },
            Oasis::Exponent {
                Oasis::Variable { "x" },
                Oasis::Real { 2.0 } } },
        Oasis::Multiply {
            Oasis::Real { 2.0 },
            Oasis::Exponent {
                Oasis::Variable { "x" },
                Oasis::Real { 2.0 } } }
    };

    auto simplified = add1.Simplify();
    REQUIRE(simplified->Is<Oasis::Multiply<Oasis::Real, Oasis::Exponent<Oasis::Variable, Oasis::Real>>>());

    auto simplified2 = add2.Simplify();
    auto simplified3 = add3.Simplify();
    auto simplified4 = add4.Simplify();

    REQUIRE(Oasis::Multiply {
        Oasis::Real { 2.0 },
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } } }
                .Equals(*simplified));
    REQUIRE(Oasis::Multiply {
        Oasis::Real { 3.0 },
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } } }
                .Equals(*simplified2));
    REQUIRE(Oasis::Multiply {
        Oasis::Real { 3.0 },
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } } }
                .Equals(*simplified3));
    REQUIRE(Oasis::Multiply {
        Oasis::Real { 4.0 },
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } } }
                .Equals(*simplified4));
}

TEST_CASE("Variable Multiplication", "[Exponent][Variable][Multiplication]")
{
    Oasis::Multiply expr = Oasis::Multiply {
        Oasis::Variable { "x" },
        Oasis::Variable { "x" }
    };

    Oasis::Multiply expr2 = Oasis::Multiply {
        Oasis::Multiply {
            Oasis::Real { 5.0 },
            Oasis::Variable { "x" } },
        Oasis::Variable { "x" }
    };

    Oasis::Multiply expr3 = Oasis::Multiply {
        Oasis::Variable { "x" },
        Oasis::Multiply {
            Oasis::Real { 5.0 },
            Oasis::Variable { "x" } }
    };

    Oasis::Multiply expr4 = Oasis::Multiply {
        Oasis::Multiply {
            Oasis::Real { 4.0 },
            Oasis::Variable { "x" } },
        Oasis::Multiply {
            Oasis::Real { 6.0 },
            Oasis::Variable { "x" } }
    };

    auto simplified = expr.Simplify();
    auto simplified2 = expr2.Simplify();
    auto simplified3 = expr3.Simplify();
    auto simplified4 = expr4.Simplify();

    REQUIRE(simplified->Is<Oasis::Exponent<Oasis::Variable, Oasis::Real>>());
    REQUIRE(simplified->Equals(Oasis::Exponent<Oasis::Variable, Oasis::Real> { Oasis::Variable { "x" }, Oasis::Real { 2.0 } }));
    REQUIRE(simplified2->Equals(Oasis::Multiply<Oasis::Real, Oasis::Exponent<Oasis::Variable, Oasis::Real>> {
        Oasis::Real { 5.0 }, Oasis::Exponent { Oasis::Variable { "x" }, Oasis::Real { 2.0 } } }));
    REQUIRE(simplified3->Equals(Oasis::Multiply<Oasis::Real, Oasis::Exponent<Oasis::Variable, Oasis::Real>> {
        Oasis::Real { 5.0 }, Oasis::Exponent { Oasis::Variable { "x" }, Oasis::Real { 2.0 } } }));
    REQUIRE(simplified4->Equals(Oasis::Multiply<Oasis::Real, Oasis::Exponent<Oasis::Variable, Oasis::Real>> {
        Oasis::Real { 24.0 }, Oasis::Exponent { Oasis::Variable { "x" }, Oasis::Real { 2.0 } } }));
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

    Oasis::Multiply expr4 = Oasis::Multiply {
        Oasis::Variable { "x" },
        Oasis::Multiply {
            Oasis::Real { 3.0 },
            Oasis::Exponent {
                Oasis::Variable { "x" },
                Oasis::Real { 3.0 } } }

    };

    Oasis::Multiply expr5 = Oasis::Multiply {
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 3.0 } },
        Oasis::Multiply {
            Oasis::Real { 3.0 },
            Oasis::Variable { "x" } }
    };

    Oasis::Multiply expr6 = Oasis::Multiply {
        Oasis::Multiply {
            Oasis::Real { 3.0 },
            Oasis::Exponent {
                Oasis::Variable { "x" },
                Oasis::Real { 2.0 } } },
        Oasis::Multiply {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } }
    };

    Oasis::Multiply expr7 = Oasis::Multiply {
        Oasis::Multiply {
            Oasis::Real { 3.0 },
            Oasis::Exponent {
                Oasis::Variable { "x" },
                Oasis::Real { 2.0 } } },
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } }
    };

    Oasis::Multiply expr8 = Oasis::Multiply {
        Oasis::Multiply {
            Oasis::Real { 3.0 },
            Oasis::Exponent {
                Oasis::Variable { "x" },
                Oasis::Real { 2.0 } } },
        Oasis::Multiply {
            Oasis::Exponent {
                Oasis::Variable { "x" },
                Oasis::Real { 2.0 } },
            Oasis::Real { 2.0 } }
    };

    auto simplified = expr.Simplify();
    auto simplified2 = expr2.Simplify();
    auto simplified3 = expr3.Simplify();
    auto simplified4 = expr4.Simplify();
    auto simplified5 = expr5.Simplify();
    auto simplified6 = expr6.Simplify();
    auto simplified7 = expr7.Simplify();
    auto simplified8 = expr8.Simplify();

    REQUIRE(simplified->Is<Oasis::Exponent<Oasis::Variable, Oasis::Real>>());
    REQUIRE(simplified->Equals(Oasis::Exponent<Oasis::Variable, Oasis::Real> { Oasis::Variable { "x" }, Oasis::Real { 4.0 } }));
    REQUIRE(simplified2->Equals(Oasis::Exponent<Oasis::Variable, Oasis::Real> { Oasis::Variable { "x" }, Oasis::Real { 4.0 } }));
    REQUIRE(simplified3->Equals(Oasis::Exponent<Oasis::Variable, Oasis::Real> { Oasis::Variable { "x" }, Oasis::Real { 4.0 } }));
    REQUIRE(simplified4->Equals(Oasis::Multiply<Oasis::Real, Oasis::Exponent<Oasis::Variable, Oasis::Real>> {
        Oasis::Real { 3.0 }, Oasis::Exponent { Oasis::Variable { "x" }, Oasis::Real { 4.0 } } }));
    REQUIRE(simplified5->Equals(Oasis::Multiply<Oasis::Real, Oasis::Exponent<Oasis::Variable, Oasis::Real>> {
        Oasis::Real { 3.0 }, Oasis::Exponent { Oasis::Variable { "x" }, Oasis::Real { 4.0 } } }));
    REQUIRE(simplified6->Equals(Oasis::Multiply<Oasis::Real, Oasis::Exponent<Oasis::Variable, Oasis::Real>> {
        Oasis::Real { 6.0 }, Oasis::Exponent { Oasis::Variable { "x" }, Oasis::Real { 3.0 } } }));
    REQUIRE(simplified7->Equals(Oasis::Multiply<Oasis::Real, Oasis::Exponent<Oasis::Variable, Oasis::Real>> {
        Oasis::Real { 3.0 }, Oasis::Exponent { Oasis::Variable { "x" }, Oasis::Real { 4.0 } } }));
    REQUIRE(simplified8->Equals(Oasis::Multiply<Oasis::Real, Oasis::Exponent<Oasis::Variable, Oasis::Real>> {
        Oasis::Real { 6.0 }, Oasis::Exponent { Oasis::Variable { "x" }, Oasis::Real { 4.0 } } }));
}

TEST_CASE("Subtraction of Exponents", "[Subtract][Exponent][Symbolic]")
{
    Oasis::Subtract sub1 {
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } },
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } }
    };

    Oasis::Subtract sub2 {
        Oasis::Multiply {
            Oasis::Real { 2.0 },
            Oasis::Exponent {
                Oasis::Variable { "x" },
                Oasis::Real { 2.0 } } },
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } }
    };

    Oasis::Subtract sub3 {
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } },
        Oasis::Multiply {
            Oasis::Real { 2.0 },
            Oasis::Exponent {
                Oasis::Variable { "x" },
                Oasis::Real { 2.0 } } }
    };

    Oasis::Subtract sub4 {
        Oasis::Multiply {
            Oasis::Real { 6.0 },
            Oasis::Exponent {
                Oasis::Variable { "x" },
                Oasis::Real { 2.0 } } },
        Oasis::Multiply {
            Oasis::Real { 2.0 },
            Oasis::Exponent {
                Oasis::Variable { "x" },
                Oasis::Real { 2.0 } } }
    };

    auto simplified = sub1.Simplify();
    auto simplified2 = sub2.Simplify();
    auto simplified3 = sub3.Simplify();
    auto simplified4 = sub4.Simplify();

    REQUIRE(Oasis::Real { 0.0 }.Equals(*simplified));
    REQUIRE(Oasis::Multiply {
        Oasis::Real { 1.0 },
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } } }
                .Equals(*simplified2));
    REQUIRE(Oasis::Multiply {
        Oasis::Real { -1.0 },
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } } }
                .Equals(*simplified3));
    REQUIRE(Oasis::Multiply {
        Oasis::Real { 4.0 },
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } } }
                .Equals(*simplified4));
}

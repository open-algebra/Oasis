//
// Created by Andrew Nazareth on 9/22/23.
//

#include "catch2/catch_test_macros.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
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

TEST_CASE("Equals", "[Exponent]"){
    Oasis::Exponent e1{
        Oasis::Variable{"x"},
            Oasis::Real{2.0}
    };
    Oasis::Exponent e2{
        Oasis::Real{2.0},
        Oasis::Variable{"x"}
    };

    REQUIRE(!e1.Equals(e2));
}

TEST_CASE("Single Addition of Exponents", "[Add][Exponent]")
{
    Oasis::Add add1 {
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } },
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } }
    };
    auto simplified = add1.Simplify();
    REQUIRE(Oasis::Multiply {
        Oasis::Real { 2.0 },
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } } }
                .Equals(*simplified));
}

TEST_CASE("Addition of Exponents with multiplication", "[Add][Multiply][Exponent]")
{
    Oasis::Add add {
        Oasis::Multiply {
            Oasis::Real { 2.0 },
            Oasis::Exponent {
                Oasis::Variable { "x" },
                Oasis::Real { 2.0 } } },
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } }
    };
    auto simplified = add.Simplify();
    REQUIRE(Oasis::Multiply {
        Oasis::Real { 3.0 },
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } } }
                .Equals(*simplified));
}

TEST_CASE("Addition of exponent multiples", "[Add][Multiply][Exponent]")
{
    Oasis::Add add{
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
    auto simplified = add.Simplify();
    REQUIRE(Oasis::Multiply {
        Oasis::Real { 4.0 },
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
    REQUIRE(Oasis::Exponent<Oasis::Variable, Oasis::Real> { Oasis::Variable { "x" }, Oasis::Real { 2.0 } }.Equals(*simplified));
}

TEST_CASE("Variable multiplication with a multiple", "[Exponent][Variable][Multiplication]")
{
    Oasis::Multiply expr = Oasis::Multiply {
        Oasis::Variable { "x" },
        Oasis::Multiply {
            Oasis::Real { 5.0 },
            Oasis::Variable { "x" } }
    };
    auto simplified = expr.Simplify();
    REQUIRE(Oasis::Multiply<Oasis::Real, Oasis::Exponent<Oasis::Variable, Oasis::Real>> {
        Oasis::Real { 5.0 }, Oasis::Exponent { Oasis::Variable { "x" }, Oasis::Real { 2.0 } } }
                .Equals(*simplified));
}

TEST_CASE("Variable multiplication of multiples", "[Exponent][Variable][Multiplication]")
{
    Oasis::Multiply expr = Oasis::Multiply {
        Oasis::Multiply {
            Oasis::Real { 4.0 },
            Oasis::Variable { "x" } },
        Oasis::Multiply {
            Oasis::Real { 6.0 },
            Oasis::Variable { "x" } }
    };

    auto simplified = expr.Simplify();

    REQUIRE(Oasis::Multiply<Oasis::Real, Oasis::Exponent<Oasis::Variable, Oasis::Real>> {
        Oasis::Real { 24.0 }, Oasis::Exponent { Oasis::Variable { "x" }, Oasis::Real { 2.0 } } }
                .Equals(*simplified));
}

TEST_CASE("Exponent Multiplication: n", "[Exponent][Variable][Multiplication]")
{
    Oasis::Multiply expr = Oasis::Multiply {
        Oasis::Variable { "x" },
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 3.0 } }
    };
    auto simplified = expr.Simplify();
    REQUIRE(Oasis::Exponent<Oasis::Variable, Oasis::Real> { Oasis::Variable { "x" }, Oasis::Real { 4.0 } }.Equals(*simplified));
}

TEST_CASE("Exponent Multiplication: n,m", "[Exponent][Variable][Multiplication]")
{
    Oasis::Multiply expr3 = Oasis::Multiply {
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } },
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 3.0 } }
    };
    auto simplified3 = expr3.Simplify();
    REQUIRE(Oasis::Exponent<Oasis::Variable, Oasis::Real> { Oasis::Variable { "x" }, Oasis::Real { 5.0 } }.Equals(*simplified3));
}

TEST_CASE("Exponent Multiplication: b, m", "[Exponent][Variable][Multiplication]")
{
    Oasis::Multiply expr4 = Oasis::Multiply {
        Oasis::Variable { "x" },
        Oasis::Multiply {
            Oasis::Real { 3.0 },
            Oasis::Exponent {
                Oasis::Variable { "x" },
                Oasis::Real { 3.0 } } }

    };
    auto simplified4 = expr4.Simplify();
    REQUIRE(Oasis::Multiply<Oasis::Real, Oasis::Exponent<Oasis::Variable, Oasis::Real>> {
        Oasis::Real { 3.0 }, Oasis::Exponent { Oasis::Variable { "x" }, Oasis::Real { 4.0 } } }
                .Equals(*simplified4));
}

TEST_CASE("Exponent Multiplication: n, b", "[Exponent][Variable][Multiplication]")
{
    Oasis::Multiply expr5 = Oasis::Multiply {
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 3.0 } },
        Oasis::Multiply {
            Oasis::Real { 3.0 },
            Oasis::Variable { "x" } }
    };
    auto simplified5 = expr5.Simplify();
    REQUIRE(Oasis::Multiply<Oasis::Real, Oasis::Exponent<Oasis::Variable, Oasis::Real>> {
        Oasis::Real { 3.0 }, Oasis::Exponent { Oasis::Variable { "x" }, Oasis::Real { 4.0 } } }
                .Equals(*simplified5));
}

TEST_CASE("Exponent Multiplication: a, b, m", "[Exponent][Variable][Multiplication]")
{
    Oasis::Multiply expr6 = Oasis::Multiply {
        Oasis::Multiply {
            Oasis::Real { 2.0 },
            Oasis::Variable { "x" } },
        Oasis::Multiply {
            Oasis::Real { 3.0 },
            Oasis::Exponent {
                Oasis::Variable { "x" },
                Oasis::Real { 2.0 } } }
    };
    auto simplified6 = expr6.Simplify();
    REQUIRE(Oasis::Multiply<Oasis::Real, Oasis::Exponent<Oasis::Variable, Oasis::Real>> {
        Oasis::Real { 6.0 }, Oasis::Exponent { Oasis::Variable { "x" }, Oasis::Real { 3.0 } } }
                .Equals(*simplified6));
}

TEST_CASE("Exponent Multiplication: a, n, m", "[Exponent][Variable][Multiplication]")
{
    Oasis::Multiply expr7 = Oasis::Multiply {
        Oasis::Multiply {
            Oasis::Real { 3.0 },
            Oasis::Exponent {
                Oasis::Variable { "x" },
                Oasis::Real { 2.0 } } },
        Oasis::Exponent {
            Oasis::Real { 2.0 },
            Oasis::Variable { "x" } }
    };
    auto simplified7 = expr7.Simplify();
    REQUIRE(Oasis::Multiply<Oasis::Real, Oasis::Exponent<Oasis::Variable, Oasis::Real>> {
        Oasis::Real { 3.0 }, Oasis::Exponent { Oasis::Variable { "x" }, Oasis::Real { 4.0 } } }
                .Equals(*simplified7));
}

TEST_CASE("Exponent Multiplication: a, n, b, m", "[Exponent][Variable][Multiplication]")
{
    Oasis::Multiply expr8 = Oasis::Multiply {
        Oasis::Multiply {
            Oasis::Real { 3.0 },
            Oasis::Exponent {
                Oasis::Variable { "x" },
                Oasis::Real { 2.0 } } },
        Oasis::Multiply {
            Oasis::Real { 2.0 },
            Oasis::Exponent {
                Oasis::Variable { "x" },
                Oasis::Real { 2.0 } } }
    };
    auto simplified8 = expr8.Simplify();
    REQUIRE(Oasis::Multiply<Oasis::Real, Oasis::Exponent<Oasis::Variable, Oasis::Real>> {
        Oasis::Real { 6.0 }, Oasis::Exponent { Oasis::Variable { "x" }, Oasis::Real { 4.0 } } }
                .Equals(*simplified8));
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
    auto simplified = sub1.Simplify();
    REQUIRE(Oasis::Real { 0.0 }.Equals(*simplified));
}
TEST_CASE("Subtraction of Exponents: a", "[Subtract][Exponent][Symbolic]")
{
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
    auto simplified2 = sub2.Simplify();
    REQUIRE(Oasis::Multiply {
        Oasis::Real { 1.0 },
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } } }
                .Equals(*simplified2));
}

TEST_CASE("Subtraction of Exponents: b", "[Subtract][Exponent][Symbolic]")
{
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
    auto simplified3 = sub3.Simplify();
    REQUIRE(Oasis::Multiply {
        Oasis::Real { -1.0 },
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } } }
                .Equals(*simplified3));
}

TEST_CASE("Subtraction of Exponents: a, b", "[Subtract][Exponent][Symbolic]")
{
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
    auto simplified4 = sub4.Simplify();

    REQUIRE(Oasis::Multiply {
        Oasis::Real { 4.0 },
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } } }
                .Equals(*simplified4));
}

TEST_CASE("Imaginary Exponent Rule: 1", "[Imaginary][Exponent]")
{
    Oasis::Imaginary i {};
    Oasis::Exponent i1 {
        Oasis::Imaginary {},
        Oasis::Real { 1.0 }
    };
    auto simplified1 = i1.Simplify();
    REQUIRE(i.Equals(*simplified1));
}

TEST_CASE("Imaginary Exponent Rule: 2", "[Imaginary][Exponent]")
{
    Oasis::Exponent i2 {
        Oasis::Imaginary {},
        Oasis::Real { 2.0 }
    };
    auto simplified2 = i2.Simplify();
    REQUIRE(Oasis::Real { -1.0 }.Equals(*simplified2));
}

TEST_CASE("Imaginary Exponent Rule: 3", "[Imaginary][Exponent]")
{
    Oasis::Exponent i3 {
        Oasis::Imaginary {},
        Oasis::Real { 3.0 }
    };
    auto simplified3 = i3.Simplify();
    REQUIRE(Oasis::Multiply { Oasis::Real { -1 }, Oasis::Imaginary {} }.Equals(*simplified3));
}

TEST_CASE("Imaginary Exponent Rule: 4", "[Imaginary][Exponent]")
{
    Oasis::Exponent i4 {
        Oasis::Imaginary {},
        Oasis::Real { 4.0 }
    };
    auto simplified4 = i4.Simplify();
    REQUIRE(Oasis::Real { 1.0 }.Equals(*simplified4));
}

TEST_CASE("Imaginary Exponent Rule: Exponential Creation", "[Imaginary][Exponent]")
    {
    Oasis::Exponent img {
        Oasis::Multiply{
            Oasis::Real{-4.0},
            Oasis::Variable{"x"}
        },
        Oasis::Real {0.5}
    };
    auto simplifiedimg = img.Simplify();

    REQUIRE(Oasis::Multiply {Oasis::Multiply{
                                  Oasis::Real{2.0}, Oasis::Exponent{Oasis::Variable{"x"}, Oasis::Real{0.5}}},
        Oasis::Imaginary{}}.Equals(*simplifiedimg));
}
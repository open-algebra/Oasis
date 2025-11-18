//
// Created by Andrew Nazareth on 9/22/23.
//

#include "catch2/catch_test_macros.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Negate.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"
#include "Oasis/SimplifyVisitor.hpp"

inline Oasis::SimplifyVisitor simplifyVisitor{};

TEST_CASE("Zero Rule", "[Exponent][Zero]")
{
    Oasis::Exponent exponent {
        Oasis::Real { 5.0 },
        Oasis::Real { 0.0 }
    };

    auto simplified = exponent.Accept(simplifyVisitor).value();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 1.0);
}

TEST_CASE("Second Zero Rule", "[Exponent][Zero]")
{
    Oasis::Exponent exponent {
        Oasis::Real { 0.0 },
        Oasis::Real { 5.0 }
    };

    auto simplified = exponent.Accept(simplifyVisitor).value();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 0.0);
}

TEST_CASE("Symbolic Zero Rule", "[Exponent][Zero][Symbolic]")
{
    Oasis::Exponent exponent {
        Oasis::Variable { "x" },
        Oasis::Real { 0.0 }
    };

    auto simplified = exponent.Accept(simplifyVisitor).value();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 1.0);
}

TEST_CASE("Second Symbolic Zero Rule", "[Exponent][Zero][Symbolic]")
{
    Oasis::Exponent exponent {
        Oasis::Real { 0.0 },
        Oasis::Variable { "x" }
    };

    auto simplified = exponent.Accept(simplifyVisitor).value();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 0.0);
}

TEST_CASE("Symbolic One Rule", "[Exponent][One][Symbolic]")
{
    Oasis::Exponent exponent {
        Oasis::Real { 1.0 },
        Oasis::Variable { "x" }
    };

    auto simplified = exponent.Accept(simplifyVisitor).value();
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

    auto simplified = exponent.Accept(simplifyVisitor).value();
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

    auto simplified = exponent.Accept(simplifyVisitor).value();
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

    auto simplified = add1.Accept(simplifyVisitor).value();
    REQUIRE(simplified->Is<Oasis::Multiply>());

    auto simplified2 = add2.Accept(simplifyVisitor).value();
    auto simplified3 = add3.Accept(simplifyVisitor).value();
    auto simplified4 = add4.Accept(simplifyVisitor).value();

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

    auto simplified = expr.Accept(simplifyVisitor).value();
    auto simplified2 = expr2.Accept(simplifyVisitor).value();
    auto simplified3 = expr3.Accept(simplifyVisitor).value();
    auto simplified4 = expr4.Accept(simplifyVisitor).value();

    REQUIRE(simplified->Is<Oasis::Exponent>());
    REQUIRE(Oasis::Exponent<Oasis::Variable, Oasis::Real> { Oasis::Variable { "x" }, Oasis::Real { 2.0 } }.Equals(*simplified));
    REQUIRE(Oasis::Multiply<Oasis::Real, Oasis::Exponent<Oasis::Variable, Oasis::Real>> {
        Oasis::Real { 5.0 }, Oasis::Exponent { Oasis::Variable { "x" }, Oasis::Real { 2.0 } } }
                .Equals(*simplified2));
    REQUIRE(Oasis::Multiply<Oasis::Real, Oasis::Exponent<Oasis::Variable, Oasis::Real>> {
        Oasis::Real { 5.0 }, Oasis::Exponent { Oasis::Variable { "x" }, Oasis::Real { 2.0 } } }
                .Equals(*simplified3));
    REQUIRE(Oasis::Multiply<Oasis::Real, Oasis::Exponent<Oasis::Variable, Oasis::Real>> {
        Oasis::Real { 24.0 }, Oasis::Exponent { Oasis::Variable { "x" }, Oasis::Real { 2.0 } } }
                .Equals(*simplified4));
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
            Oasis::Real { 3.0 } }
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
            Oasis::Real { 2.0 },
            Oasis::Variable { "x" } },
        Oasis::Multiply {
            Oasis::Real { 3.0 },
            Oasis::Exponent {
                Oasis::Variable { "x" },
                Oasis::Real { 2.0 } } }
    };

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

    auto simplified = expr.Accept(simplifyVisitor).value();
    auto simplified2 = expr2.Accept(simplifyVisitor).value();
    auto simplified3 = expr3.Accept(simplifyVisitor).value();
    auto simplified4 = expr4.Accept(simplifyVisitor).value();
    auto simplified5 = expr5.Accept(simplifyVisitor).value();
    auto simplified6 = expr6.Accept(simplifyVisitor).value();
    auto simplified7 = expr7.Accept(simplifyVisitor).value();
    auto simplified8 = expr8.Accept(simplifyVisitor).value();

    REQUIRE(simplified->Is<Oasis::Exponent>());
    REQUIRE(Oasis::Exponent<Oasis::Variable, Oasis::Real> { Oasis::Variable { "x" }, Oasis::Real { 4.0 } }.Equals(*simplified));
    REQUIRE(Oasis::Exponent<Oasis::Variable, Oasis::Real> { Oasis::Variable { "x" }, Oasis::Real { 4.0 } }.Equals(*simplified2));
    REQUIRE(Oasis::Exponent<Oasis::Variable, Oasis::Real> { Oasis::Variable { "x" }, Oasis::Real { 5.0 } }.Equals(*simplified3));
    REQUIRE(Oasis::Multiply<Oasis::Real, Oasis::Exponent<Oasis::Variable, Oasis::Real>> {
        Oasis::Real { 3.0 }, Oasis::Exponent { Oasis::Variable { "x" }, Oasis::Real { 4.0 } } }
                .Equals(*simplified4));
    REQUIRE(Oasis::Multiply<Oasis::Real, Oasis::Exponent<Oasis::Variable, Oasis::Real>> {
        Oasis::Real { 3.0 }, Oasis::Exponent { Oasis::Variable { "x" }, Oasis::Real { 4.0 } } }
                .Equals(*simplified5));
    REQUIRE(Oasis::Multiply<Oasis::Real, Oasis::Exponent<Oasis::Variable, Oasis::Real>> {
        Oasis::Real { 6.0 }, Oasis::Exponent { Oasis::Variable { "x" }, Oasis::Real { 3.0 } } }
                .Equals(*simplified6));
    REQUIRE(Oasis::Multiply<Oasis::Real, Oasis::Exponent<Oasis::Variable, Oasis::Real>> {
        Oasis::Real { 3.0 }, Oasis::Exponent { Oasis::Variable { "x" }, Oasis::Real { 4.0 } } }
                .Equals(*simplified7));
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

    auto simplified = sub1.Accept(simplifyVisitor).value();
    auto simplified2 = sub2.Accept(simplifyVisitor).value();
    auto simplified3 = sub3.Accept(simplifyVisitor).value();
    auto simplified4 = sub4.Accept(simplifyVisitor).value();

    REQUIRE(Oasis::Real { 0.0 }.Equals(*simplified));
    REQUIRE((Oasis::Multiply {
        Oasis::Real { 1.0 },
        Oasis::Exponent {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } } }).Accept(simplifyVisitor).value()->Equals(*simplified2));
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

TEST_CASE("Imaginary Exponent Rule", "[Imaginary][Exponent]")
{
    Oasis::Imaginary i {};
    Oasis::Exponent i1 {
        Oasis::Imaginary {},
        Oasis::Real { 1.0 }
    };
    Oasis::Exponent i2 {
        Oasis::Imaginary {},
        Oasis::Real { 2.0 }
    };
    Oasis::Exponent i3 {
        Oasis::Imaginary {},
        Oasis::Real { 3.0 }
    };
    Oasis::Exponent i4 {
        Oasis::Imaginary {},
        Oasis::Real { 4.0 }
    };
    Oasis::Exponent img {
        Oasis::Multiply {
            Oasis::Real { -4.0 },
            Oasis::Variable { "x" } },
        Oasis::Real { 0.5 }
    };

    auto simplified1 = i1.Accept(simplifyVisitor).value();
    auto simplified2 = i2.Accept(simplifyVisitor).value();
    auto simplified3 = i3.Accept(simplifyVisitor).value();
    auto simplified4 = i4.Accept(simplifyVisitor).value();
    auto simplifiedimg = img.Accept(simplifyVisitor).value();

    REQUIRE(i.Is<Oasis::Imaginary>());
    REQUIRE(Oasis::Imaginary {}.Equals(*simplified1));
    REQUIRE(Oasis::Negate { Oasis::Imaginary {} }.Equals(*simplified3));
    REQUIRE(Oasis::Real { -1.0 }.Equals(*simplified2));
    REQUIRE(Oasis::Real { 1.0 }.Equals(*simplified4));
    REQUIRE(Oasis::Multiply { Oasis::Multiply {
                                  Oasis::Real { 2.0 }, Oasis::Exponent { Oasis::Variable { "x" }, Oasis::Real { 0.5 } } },
        Oasis::Imaginary {} }
                .Equals(*simplifiedimg));
}
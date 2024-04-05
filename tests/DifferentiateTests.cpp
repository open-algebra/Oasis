//
// Created by bachia on 4/5/2024.
//
#include "catch2/catch_test_macros.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"

TEST_CASE("Nonzero number", "[Differentiate][Real][Nonzero]")
{
    Oasis::Real zero { 0.0f };

    Oasis::Real base { 2.0f };
    Oasis::Variable var { "x" };

    auto diff2 = base.Differentiate(var);
    REQUIRE(zero.Equals(*diff2));
}

TEST_CASE("Zero", "[Differentiate][Real][Zero]")
{
    Oasis::Variable constant { "C" };
    Oasis::Real zero { 0.0f };
    Oasis::Variable var { "x" };

    auto diff = zero.Differentiate(var);
    REQUIRE(zero.Equals(*diff));
}

TEST_CASE("Same Variable", "[Differentiate][Variable][Same]")
{
    Oasis::Variable var { "x" };
    Oasis::Real result {1.0};

    auto integrated = var.Differentiate(var);
    REQUIRE(result.Equals(*integrated));
}

TEST_CASE("Different Variable", "[Differentiate][Variable][Different]")
{
    Oasis::Variable var { "x" };
    Oasis::Variable var2 { "y" };
    Oasis::Real zero { 0.0f };

    auto diff = var2.Differentiate(var);
    REQUIRE(zero.Equals(*diff));
}

TEST_CASE("Power Rule", "[Differentiate][Exponent][Power]")
{
    Oasis::Variable var { "x" };
    Oasis::Exponent<Oasis::Variable, Oasis::Real> differentiate { Oasis::Variable { var.GetName() }, Oasis::Real { 3.0f } };
    Oasis::Multiply<Oasis::Exponent<Oasis::Variable, Oasis::Real>, Oasis::Real> diff1 {
        Oasis::Multiply {
            Oasis::Exponent { Oasis::Variable { var.GetName() }, Oasis::Real { 2.0f } },
            Oasis::Real { 3.0f } }};
    auto ptr = differentiate.Simplify();
    auto diffed = diff1.Differentiate(var);
    REQUIRE((*diffed).Equals(*ptr));
}

TEST_CASE("Constant Rule Multiply", "[Differentiate][Multiply][Constant]")
{
    Oasis::Variable var { "x" };
    Oasis::Multiply<Oasis::Real, Oasis::Variable> diff1 { Oasis::Real { 3.0f }, Oasis::Variable { var.GetName() } };
    Oasis::Add<Oasis::Multiply<Oasis::Real, Oasis::Divide<Oasis::Exponent<Oasis::Variable, Oasis::Real>, Oasis::Real>>, Oasis::Variable> integral {
        Oasis::Add {
            Oasis::Multiply {
                Oasis::Real { 3.0f },
                Oasis::Divide {
                    Oasis::Exponent { Oasis::Variable { var.GetName() }, Oasis::Real { 2.0f } },
                    Oasis::Real { 2.0f } },
            },
            Oasis::Variable {
                "C" } }
    };

    auto ptr = integral.Simplify();
    auto integrated = integrand.Integrate(var);
    REQUIRE((integrated->Equals(*ptr)));
}

TEST_CASE("Constant Rule Divide", "[Integrate][Divide][Constant]")
{
    Oasis::Variable var { "x" };
    Oasis::Divide<Oasis::Variable, Oasis::Real> integrand { Oasis::Variable { var.GetName() }, Oasis::Real { 3.0f } };
    Oasis::Add<Oasis::Multiply<Oasis::Divide<Oasis::Real>, Oasis::Exponent<Oasis::Variable, Oasis::Real>>, Oasis::Variable> integral {
        Oasis::Add {
            Oasis::Multiply {
                Oasis::Divide {
                    Oasis::Real { 1.0f }, Oasis::Real { 6.0f } },
                Oasis::Exponent {
                    Oasis::Variable { var.GetName() },
                    Oasis::Real { 2.0f } } },
            Oasis::Variable { "C" } }
    };
    auto integrated = integrand.Integrate(var);
    auto simplified = integrated->Simplify();

    REQUIRE((simplified->Equals(*(integral.Simplify()))));
}

TEST_CASE("Add Rule Different Terms", "[Integrate][Add][Different]")
{
    Oasis::Variable var { "x" };
    Oasis::Add<Oasis::Variable, Oasis::Real> integrand { Oasis::Variable { var.GetName() }, Oasis::Real { 2.0f } };

    Oasis::Add<Oasis::Add<Oasis::Divide<Oasis::Exponent<Oasis::Variable, Oasis::Real>, Oasis::Real>, Oasis::Multiply<Oasis::Real, Oasis::Variable>>, Oasis::Variable> integral {
        Oasis::Add {
            Oasis::Add {
                Oasis::Divide {
                    Oasis::Exponent { Oasis::Variable { var.GetName() }, Oasis::Real { 2.0f } },
                    Oasis::Real { 2.0f } },
                Oasis::Multiply {
                    Oasis::Real { 2.0f },
                    Oasis::Variable { var.GetName() } } },
            Oasis::Variable { "C" } }
    };
    auto integrated = integrand.Integrate(var);
    auto simplified = integrated->Simplify();

    REQUIRE(simplified->Equals(*(integral.Simplify())));
}

TEST_CASE("Subtract Rule Different Terms", "[Integrate][Subtract][Different]")
{
    Oasis::Variable var { "x" };
    Oasis::Subtract<Oasis::Variable, Oasis::Real> integrand { Oasis::Variable { var.GetName() }, Oasis::Real { 2.0f } };

    Oasis::Add<Oasis::Subtract<Oasis::Divide<Oasis::Exponent<Oasis::Variable, Oasis::Real>, Oasis::Real>, Oasis::Multiply<Oasis::Real, Oasis::Variable>>, Oasis::Variable> integral {
        Oasis::Add {
            Oasis::Subtract {
                Oasis::Divide {
                    Oasis::Exponent { Oasis::Variable { var.GetName() }, Oasis::Real { 2.0f } },
                    Oasis::Real { 2.0f } },
                Oasis::Multiply {
                    Oasis::Real { 2.0f },
                    Oasis::Variable { var.GetName() } } },
            Oasis::Variable { "C" } }
    };
    auto integrated = integrand.Integrate(var);
    auto simplified = integrated->Simplify();

    REQUIRE(simplified->Equals(*(integral.Simplify())));
}

TEST_CASE("Add Rule Like Terms", "[Integrate][Add][Like]")
{
    Oasis::Variable var { "x" };
    Oasis::Add<Oasis::Variable, Oasis::Variable> integrand { Oasis::Variable { var.GetName() }, Oasis::Variable { var.GetName() } };

    Oasis::Add<Oasis::Exponent<Oasis::Variable, Oasis::Real>, Oasis::Variable> integral {
        Oasis::Add {
            Oasis::Exponent { Oasis::Variable { var.GetName() }, Oasis::Real { 2.0f } },
            Oasis::Variable { "C" } }
    };

    auto integrated = integrand.Integrate(var);
    auto simplified = integrated->Simplify();

    REQUIRE(simplified->Equals(*(integral.Simplify())));
}
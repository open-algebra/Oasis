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
    auto ptr = diff1.Simplify();
    auto diffed = differentiate.Differentiate(var);
    REQUIRE((*diffed).Equals(*ptr));
}

TEST_CASE("Constant Rule Multiply", "[Differentiate][Multiply][Constant]")
{
    Oasis::Variable var { "x" };
    Oasis::Multiply<Oasis::Real, Oasis::Variable> diff1 { Oasis::Real { 3.0f }, Oasis::Variable { var.GetName() } };
    Oasis::Real three {3.0};

    auto ptr = three.Simplify();
    auto diffed = diff1.Differentiate(var);
    REQUIRE((diffed->Equals(*ptr)));
}

TEST_CASE("Constant Rule Divide", "[Differentiate][Divide][Constant]")
{
    Oasis::Variable var { "x" };
    Oasis::Divide<Oasis::Variable, Oasis::Real> diff1 { Oasis::Variable { var.GetName() }, Oasis::Real { 2.0f } };
    Oasis::Real half {0.5f};
    auto diffed = diff1.Differentiate(var);
    auto simplified = diffed->Simplify();
    REQUIRE((simplified->Equals(*(half.Simplify()))));
}

TEST_CASE("Add Rule Different Terms", "[Differentiate][Add][Different]")
{
    Oasis::Variable var { "x" };
    Oasis::Add<Oasis::Variable, Oasis::Real>diff1 { Oasis::Variable { var.GetName() }, Oasis::Real { 2.0f } };

    Oasis::Real one {1};
    auto diffed = diff1.Differentiate(var);
    auto simplified = diffed->Simplify();

    REQUIRE(simplified->Equals(*(one.Simplify())));
}

TEST_CASE("Subtract Rule Different Terms", "[Differentiate][Subtract][Different]")
{
    Oasis::Variable var { "x" };
    Oasis::Subtract<Oasis::Variable, Oasis::Real> diff1 { Oasis::Variable { var.GetName() }, Oasis::Real { 2.0f } };
    Oasis::Real one {1};
    auto diffed = diff1.Differentiate(var);
    auto simplified = diffed->Simplify();

    REQUIRE(simplified->Equals(*(one.Simplify())));
}

TEST_CASE("Add Rule Like Terms", "[Differentiate][Add][Like]")
{
    Oasis::Variable var { "x" };
    Oasis::Add<Oasis::Variable, Oasis::Variable> diff1 { Oasis::Variable { var.GetName() }, Oasis::Variable { var.GetName() } };

    Oasis::Real two {2};

    auto diffed = diff1.Differentiate(var);
    auto simplified = diffed->Simplify();

    REQUIRE(simplified->Equals(*(two.Simplify())));
}

TEST_CASE("Quotient Rule Like Terms", "[Differentiate][Divide][Like]")
{
    Oasis::Variable var { "x"};
    Oasis::Divide<Oasis::Add<Oasis::Variable, Oasis::Real>, Oasis::Add<Oasis::Variable, Oasis::Real>> diff1
    {Oasis::Add<Oasis::Variable, Oasis::Real> {Oasis::Variable {var.GetName() },
                                               Oasis::Real {2}},Oasis::Add<Oasis::Variable, Oasis::Real>
                                                       {Oasis::Variable {var.GetName() }, Oasis::Real
                                                       {3}}};
    Oasis::Divide<Oasis::Expression, Oasis::Multiply<Oasis::Expression, Oasis::Expression>> answer
            {Oasis::Subtract<Oasis::Multiply<Oasis::Add<Oasis::Variable, Oasis::Real>, Oasis::Real>>
            {Oasis::Multiply<Oasis::Add<Oasis::Variable, Oasis::Real>, Oasis::Real>
                    {Oasis::Add<Oasis::Variable, Oasis::Real> {Oasis::Variable {var.GetName()}, Oasis::Real {3}},
                     Oasis::Real {1}},
             Oasis::Multiply<Oasis::Add<Oasis::Variable, Oasis::Real>, Oasis::Real> {
                Oasis::Add<Oasis::Variable, Oasis::Real> {Oasis::Variable {var.GetName()}, Oasis::Real {2}},
                Oasis::Real {1}}},
             Oasis::Multiply<Oasis::Expression, Oasis::Expression> {Oasis::Add<Oasis::Variable, Oasis::Real>
            {Oasis::Variable {var.GetName() }, Oasis::Real
            {3}}, Oasis::Add<Oasis::Variable, Oasis::Real>
            {Oasis::Variable {var.GetName() }, Oasis::Real
            {3}}}};
    auto diffed = diff1.Differentiate(var);
    auto simplified = diffed->Simplify();

    REQUIRE(simplified->Equals(*(answer.Simplify())));
}
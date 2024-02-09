#include "catch2/catch_test_macros.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Variable.hpp"

TEST_CASE("Nonzero number", "[Integrate][Real][Nonzero]")
{
    Oasis::Add<Oasis::Multiply<Oasis::Real, Oasis::Variable>, Oasis::Variable> integral {
        Oasis::Multiply {
            Oasis::Real { 2.0f },
            Oasis::Variable { "x" } },
        Oasis::Variable { "C" }
    };

    Oasis::Real base { 2.0f };
    Oasis::Variable var { "x" };

    auto integrated = base.Integrate(var);
    REQUIRE(integral.Equals(*integrated));
}

TEST_CASE("Zero", "[Integrate][Real][Zero]")
{
    Oasis::Variable constant { "C" };
    Oasis::Real zero { 0.0f };
    Oasis::Variable var { "x" };

    auto integrated = zero.Integrate(var);
    REQUIRE(constant.Equals(*integrated));
}

TEST_CASE("Same Variable", "[Integrate][Variable][Same]")
{
    Oasis::Variable var { "x" };
    Oasis::Add<Oasis::Divide<Oasis::Exponent<Oasis::Variable, Oasis::Real>, Oasis::Real>, Oasis::Variable> integral {
        Oasis::Divide {
            Oasis::Exponent { Oasis::Variable { var.GetName() }, Oasis::Real { 2.0f } },
            Oasis::Real { 2.0f } },
        Oasis::Variable { "C" }
    };

    auto integrated = var.Integrate(var);
    REQUIRE(integral.Equals(*integrated));
}

TEST_CASE("Different Variable", "[Integrate][Variable][Different]")
{
    Oasis::Variable var { "x" };
    Oasis::Variable var2 { "y" };
    Oasis::Add<Oasis::Multiply<Oasis::Variable, Oasis::Variable>, Oasis::Variable> integral {
        Oasis::Multiply {
            Oasis::Variable { var2.GetName() },
            Oasis::Variable { var.GetName() } },
        Oasis::Variable { "C" }
    };

    auto integrated = var2.Integrate(var);
    REQUIRE(integral.Equals(*integrated));
}
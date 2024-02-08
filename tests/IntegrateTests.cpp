#include "catch2/catch_test_macros.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Variable.hpp"

TEST_CASE("Nonzero number", "[Real][Nonzero]")
{
    Oasis::Add<Oasis::Multiply<Oasis::Real, Oasis::Variable>, Oasis::Variable> integral {
        Oasis::Multiply<Oasis::Real, Oasis::Variable> {
            Oasis::Real { 2.0f },
            Oasis::Variable { "x" } },
        Oasis::Variable { "C" }
    };

    Oasis::Real base { 2.0f };
    Oasis::Variable var { "x" };

    auto integrated = base.Integrate(var);
    REQUIRE(integral.Equals(*integrated));
}

TEST_CASE("Zero", "[Real][Zero]")
{
    Oasis::Variable constant { "C" };
    Oasis::Real zero { 0.0f };
    Oasis::Variable var { "x" };

    auto integrated = zero.Integrate(var);
    REQUIRE(constant.Equals(*integrated));
}
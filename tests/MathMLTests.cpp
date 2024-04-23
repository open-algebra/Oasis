//
// Created by Matthew McCall on 3/26/24.
//
#include "catch2/catch_test_macros.hpp"

#include <Oasis/Add.hpp>
#include <Oasis/Divide.hpp>
#include <Oasis/Multiply.hpp>
#include <Oasis/Variable.hpp>

TEST_CASE("ToMathML Works", "[MathML]")
{
    Oasis::Divide divide {
        Oasis::Add {
            Oasis::Variable { "x" },
            Oasis::Variable { "y" } },
        Oasis::Multiply {
            Oasis::Variable { "z" },
            Oasis::Variable { "w" } }
    };

    auto mathml = divide.ToMathML();
    std::string expected = R"(<mfrac>
    <mrow>
        <mi>x</mi>
        <mo>+</mo>
        <mi>y</mi>
    </mrow>
    <mrow>
        <mi>z</mi>
        <mi>w</mi>
    </mrow>
</mfrac>
)";

    REQUIRE(mathml == expected);
}
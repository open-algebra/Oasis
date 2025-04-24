/**
 * Created by Matthew McCall on 4/5/24.
 */

#include "catch2/catch_test_macros.hpp"

#include <Oasis/Negate.hpp>
#include "Oasis/Subtract.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Variable.hpp"
#include "Oasis/Real.hpp"

TEST_CASE("Negate", "[Negate]")
{
    const Oasis::Negate negativeOne {
        Oasis::Real { 1.0 }
    };

    const auto simplified = negativeOne.Simplify();

    const Oasis::Real expected { -1.0 };
    REQUIRE(simplified->Equals(expected));
}
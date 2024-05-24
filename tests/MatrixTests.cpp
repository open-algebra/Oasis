//
// Created by Andrew Nazareth on 5/24/24.
//

#include "catch2/catch_test_macros.hpp"
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "Oasis/Matrix.hpp"

#define EPSILON 10E-6

TEST_CASE("Create Empty Matrix", "[Matrix]")
{
    Oasis::Matrix a = {3,4};

    REQUIRE(a.GetMatrix().cols() == 4);
    REQUIRE(a.GetMatrix().rows() == 3);
}
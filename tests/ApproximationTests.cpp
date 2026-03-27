//
// Created by Justin Romanelli on 3/20/26.
//

#include "catch2/catch_test_macros.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Variable.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Expression.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Subtract.hpp"

TEST_CASE("Impossible Approximation of a Polynomial (Stuck)", "[Approximation]")
{
    Oasis::Variable x { "x" };
    Oasis::Subtract<Oasis::Exponent<Oasis::Variable, Oasis::Real>, Oasis::Real> polynomial {
        Oasis::Exponent<Oasis::Variable, Oasis::Real> {
            x, Oasis::Real { 2.0f }
        },
        Oasis::Real { 1.0f }
    };
    Oasis::Real guess { 0.0f };

    // This should get "stuck" at x = 0, such that it can't find a better value to use
    // So, it should return nullptr. Make sure that's the case.
    std::unique_ptr<Oasis::Expression> result = polynomial.ApproximateZeros(x, *guess.Copy(), 5);

    REQUIRE(result == nullptr);
}




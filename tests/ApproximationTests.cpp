//
// Created by Justin Romanelli on 3/20/26.
//

#include "Oasis/Add.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Expression.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"
#include "catch2/catch_test_macros.hpp"

TEST_CASE("Approximation of a Linear function", "[Real][Approximation]")
{
    // Approximating f(x) = 5x - 10, looking for the root x = 2
    Oasis::Variable x { "x" };
    Oasis::Add<> linear {
        Oasis::Multiply<> { Oasis::Real { 5.0f }, x },
        Oasis::Real { -10.0f }
    };
    Oasis::Real guess { 5.0f }, ans { 2.0f };

    // Approximate a root
    std::unique_ptr<Oasis::Expression> result = linear.ApproximateZeros(x, guess, 10);

    // Make sure the approximation worked correctly

    // After a certain level of precision, the approximation becomes "exact", in that
    // it truncates the decimal to the regular value if the result is precise enough.
    // So, the approximations should be equal to the roots themselves.
    REQUIRE(ans.Equals(*result));
}

TEST_CASE("Approximation of a Polynomial", "[Real][Approximation]")
{
    // Approximate a root for f(x) = x^2 - 4
    // Looking for both roots, x = 2 and x = -2
    Oasis::SimplifyVisitor sV {};
    Oasis::Variable x { "x" };
    Oasis::Subtract<> polynomial {
        Oasis::Exponent<> {
            x, Oasis::Real { 2.0f } },
        Oasis::Real { 4.0f }
    };

    // Actual roots to the polynomial
    Oasis::Real rootNeg { -2.0f }, rootPos { 2.0f };

    // Guesses to start the approximations
    Oasis::Real guessNeg { -5.0f }, guessPos { 5.0f };

    // Approximations of the roots
    // Make sure these are equal to the original roots
    std::unique_ptr<Oasis::Expression> approximationNeg = polynomial.ApproximateZeros(x, guessNeg, 10);
    std::unique_ptr<Oasis::Expression> approximationPos = polynomial.ApproximateZeros(x, guessPos, 10);

    // Simplify the approximations
    approximationNeg = *approximationNeg->Accept(sV);
    approximationPos = *approximationPos->Accept(sV);

    // Check that they equal the actual roots (root1Ans and root2Ans)
    REQUIRE(rootNeg.Equals(*approximationNeg));
    REQUIRE(rootPos.Equals(*approximationPos));
}

TEST_CASE("Approximation of a Higher-Degree Polynomial", "[Real][Approximation]")
{
    // Testing and setup variables
    Oasis::Variable x { "x" };

    // Approximating 256x^10 - 16x^6 = 0
    // One root is x = 0.5 (looking for it here)
    Oasis::Subtract<> polynomial {
        Oasis::Multiply<> {
            Oasis::Real { 256.0f },
            Oasis::Exponent<> {
                x, Oasis::Real { 10.0f } } },
        Oasis::Multiply<> {
            Oasis::Real { 16.0f },
            Oasis::Exponent<> {
                x, Oasis::Real { 6.0f } } }
    };

    // Initial guess and answer
    Oasis::Real guess { 3.0f }, answer { 0.5f };

    // Approximate the answer here
    std::unique_ptr<Oasis::Expression> result = polynomial.ApproximateZeros(x, guess, 25);

    // Check to make sure it's equal to the actual answer (with many iterations)
    REQUIRE(answer.Equals(*result));
}

TEST_CASE("Impossible Approximation of a Polynomial (Stuck)", "[Approximation]")
{
    // Testing variables
    // Attempt to approximate x^2 - 1 = 0
    Oasis::Variable x { "x" };
    Oasis::Subtract<> polynomial {
        Oasis::Exponent<> {
            x, Oasis::Real { 2.0f } },
        Oasis::Real { 1.0f }
    };
    Oasis::Real guess { 0.0f };

    // This should get "stuck" at x = 0, such that it can't find a better value to use
    // So, it should return the original function. Make sure that's the case.
    std::unique_ptr<Oasis::Expression> result = polynomial.ApproximateZeros(x, guess, 5);

    REQUIRE(polynomial.Equals(*result));
}
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
    Oasis::Real guess { 5.0f };
    std::unique_ptr<Oasis::Expression> ans = Oasis::Real { 2.0f }.Copy();

    // Approximate a root
    std::unique_ptr<Oasis::Expression> result = linear.ApproximateZeros(x, *guess.Copy(), 10);

    // After a certain level of precision, the approximation becomes "exact", in that
    // it truncates the decimal to the regular value if the result is precise enough.
    // So, the approximations should be equal to the roots themselves.
    REQUIRE(result->Equals(*ans));
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
    std::unique_ptr<Oasis::Expression> root1Ans = Oasis::Real { -2.0f }.Copy();
    std::unique_ptr<Oasis::Expression> root2Ans = Oasis::Real { 2.0f }.Copy();

    // Approximations (to test)
    std::unique_ptr<Oasis::Expression> root1Approximation = polynomial.Copy()->ApproximateZeros(*x.Copy(), *Oasis::Real { -5.0f }.Copy(), 10);
    std::unique_ptr<Oasis::Expression> root2Approximation = polynomial.Copy()->ApproximateZeros(*x.Copy(), *Oasis::Real { 5.0f }.Copy(), 10);

    // Make sure the approximations completed correctly
    REQUIRE(root1Approximation != nullptr);
    REQUIRE(root2Approximation != nullptr);

    // Simplify the approximation
    root1Approximation = *root1Approximation->Accept(sV);
    root2Approximation = *root2Approximation->Accept(sV);

    // Check that they equal the actual roots (root1Ans and root2Ans)
    REQUIRE(root1Approximation->Equals(*root1Ans));
    REQUIRE(root2Approximation->Equals(*root2Ans));
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
    Oasis::Real guess { 3.0f };
    std::unique_ptr<Oasis::Expression> answer = Oasis::Real { 0.5f }.Copy();

    // Approximate the answer here
    std::unique_ptr<Oasis::Expression> result = polynomial.ApproximateZeros(*x.Copy(), *guess.Copy(), 25);

    // Check to make sure it's equal to the actual answer (with many iterations)
    REQUIRE(result->Equals(*answer));
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
    // So, it should return nullptr. Make sure that's the case.
    std::unique_ptr<Oasis::Expression> result = polynomial.ApproximateZeros(x, *guess.Copy(), 5);

    REQUIRE(result == nullptr);
}
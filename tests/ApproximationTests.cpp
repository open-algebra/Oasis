//
// Created by Justin Romanelli on 3/20/26.
//

#include <iostream>

#include "catch2/catch_test_macros.hpp"
#include "Oasis/Variable.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Expression.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Subtract.hpp"

TEST_CASE("Approximation of a Polynomial", "[Real][Approximation]")
{
    // Testing variables
    Oasis::SimplifyVisitor sV {};
    Oasis::Variable x { "x" };
    Oasis::Subtract<Oasis::Exponent<Oasis::Variable, Oasis::Real>, Oasis::Real> polynomial {
        Oasis::Exponent<Oasis::Variable, Oasis::Real> {
            x, Oasis::Real { 2.0f }
        },
        Oasis::Real { 4.0f }
    };

    // Actual roots to the polynomial
    std::unique_ptr<Oasis::Expression> root1_ans = Oasis::Real { -2.0f }.Copy();
    std::unique_ptr<Oasis::Expression> root2_ans = Oasis::Real {  2.0f }.Copy();

    // Approximations (to test)
    std::unique_ptr<Oasis::Expression> root1_approximation = polynomial.Copy()->ApproximateZeros(*x.Copy(), *Oasis::Real { -5.0f }.Copy(), 10);
    std::unique_ptr<Oasis::Expression> root2_approximation = polynomial.Copy()->ApproximateZeros(*x.Copy(), *Oasis::Real {  5.0f }.Copy(), 10);

    // Make sure the approximations completed correctly
    REQUIRE(root1_approximation != nullptr);
    REQUIRE(root2_approximation != nullptr);

    // Simplify the approximation
    root1_approximation = *root1_approximation->Accept(sV);
    root2_approximation = *root2_approximation->Accept(sV);

    // After a certain level of precision, the approximation becomes "exact", in that
    // it truncates the decimal to the regular value if the result is precise enough.
    // So, the approximations should be equal to the roots themselves.
    REQUIRE(root1_approximation->Equals(*root1_ans));
    REQUIRE(root2_approximation->Equals(*root2_ans));
}

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

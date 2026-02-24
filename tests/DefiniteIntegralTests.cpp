//
// Created by Justin Romanelli on 2/1/26.
//

#include "Oasis/Add.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Expression.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Variable.hpp"
#include "catch2/catch_test_macros.hpp"

TEST_CASE("Definite Integral with distinct bounds", "[Integrate][Real][Definite]")
{

    Oasis::Variable x { "x" };

    Oasis::Multiply<Oasis::Real, Oasis::Variable> integrand {
        Oasis::Real { 2.0f }, x
    };

    // Correct answer for an indefinite integral of the integrand
    Oasis::Add<Oasis::Exponent<Oasis::Variable, Oasis::Real>, Oasis::Variable> integrated = {
        Oasis::Exponent<Oasis::Variable, Oasis::Real> {
            x, Oasis::Real { 2.0f }
        },
        Oasis::Variable { "C" }
    };

    Oasis::Real lower_bound { 1.0f }, upper_bound { 3.0f };
    std::unique_ptr<Oasis::Expression> answer = Oasis::Real { 8.0f }.Copy();

    Oasis::Integral<Oasis::Expression, Oasis::Expression> integral { *integrand.Copy(), *x.Copy() };

    std::unique_ptr<Oasis::Expression> result = integral.IntegrateWithBounds(*x.Copy(), *lower_bound.Copy(), *upper_bound.Copy());

    REQUIRE(answer->Equals(*result));
}

TEST_CASE("Definite Integral with equal bounds", "[Integrate][Definite][Zero]")
{
    Oasis::Variable x { "x" };

    Oasis::Exponent<Oasis::Variable, Oasis::Real> integrand {
        x, Oasis::Real { 2.0f }
    };

    Oasis::Real lower_bound { 3.0f }, upper_bound { 3.0f };

    std::unique_ptr<Oasis::Expression> answer = Oasis::Real { 0.0f }.Copy();

    Oasis::Integral<Oasis::Expression, Oasis::Expression> integral { *integrand.Copy(), *x.Copy() };

    std::unique_ptr<Oasis::Expression> result = integral.IntegrateWithBounds(*x.Copy(), *lower_bound.Copy(), *upper_bound.Copy());

    REQUIRE(answer->Equals(*result));
}
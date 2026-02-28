//
// Created by Justin Romanelli on 2/1/26.
//

#include "Oasis/Add.hpp"
#include "Oasis/Divide.hpp"
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

    Oasis::Real lower_bound { 1.0f }, upper_bound { 3.0f };
    std::unique_ptr<Oasis::Expression> answer = Oasis::Real { 8.0f }.Copy();

    Oasis::Integral<Oasis::Expression, Oasis::Expression> integral { *integrand.Copy(), *x.Copy() };

    std::unique_ptr<Oasis::Expression> result = integral.IntegrateWithBounds(*x.Copy(), *lower_bound.Copy(), *upper_bound.Copy());

    REQUIRE(answer->Equals(*result));
}

TEST_CASE("Definite Integral with a non-trivial integrand", "[Integral][Definite][Real]")
{
    Oasis::Variable x { "x" };

    // TODO: Figure out a better test case!
    Oasis::Multiply<> integrand = Oasis::Multiply<> {
        Oasis::Divide<Oasis::Real, Oasis::Real> {
            Oasis::Real { 1.0f }, Oasis::Real { 4.0f }
        },
        Oasis::Exponent<Oasis::Variable, Oasis::Real> {
            x, Oasis::Real { 3.0f }
        }
    };

    Oasis::Real lower_bound { 2.0f }, upper_bound { 4.0f };
    std::unique_ptr<Oasis::Expression> answer = Oasis::Real { 15.0f }.Copy();

    Oasis::Integral<Oasis::Expression, Oasis::Expression> integral { *integrand.Copy(), *x.Copy() };

    std::unique_ptr<Oasis::Expression> result = integral.IntegrateWithBounds(*x.Copy(), *lower_bound.Copy(), *upper_bound.Copy());

    REQUIRE(answer->Equals(*result));
}

TEST_CASE("Definite Integral of 0", "[Integrate][Definite][Zero]")
{
    Oasis::Variable x { "x" };
    Oasis::Real zero { 0.0f }, lower_bound { 2.0f }, upper_bound { 5.0f };

    std::unique_ptr<Oasis::Expression> answer = Oasis::Real { 0.0f }.Copy();

    Oasis::Integral<Oasis::Expression, Oasis::Expression> integral { *zero.Copy(), *x.Copy() };

    std::unique_ptr<Oasis::Expression> result = integral.IntegrateWithBounds(*x.Copy(), *lower_bound.Copy(), *upper_bound.Copy());

    REQUIRE(answer->Equals(*result));
}

TEST_CASE("Definite Integral with equal bounds", "[Integrate][Real][Definite]")
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

TEST_CASE("Definite Integral with swapped bounds", "[Integral][Definite][Negative]")
{
    Oasis::Variable x { "x" };

    Oasis::Multiply<Oasis::Real, Oasis::Variable> integrand {
        Oasis::Real { 4.0f }, x
    };

    Oasis::Real lower_bound { 6.0f }, upper_bound { 2.0f };

    std::unique_ptr<Oasis::Expression> answer = Oasis::Real { -64.0f }.Copy();

    Oasis::Integral<Oasis::Expression, Oasis::Expression> integral { *integrand.Copy(), *x.Copy() };

    std::unique_ptr<Oasis::Expression> result = integral.IntegrateWithBounds(*x.Copy(), *lower_bound.Copy(), *upper_bound.Copy());

    REQUIRE(answer->Equals(*result));
}

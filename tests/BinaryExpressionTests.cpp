//
// Created by Matthew McCall on 10/6/23.
//
#include "catch2/catch_test_macros.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Variable.hpp"

TEST_CASE("Specialize Considers Commutative Property", "[Symbolic]")
{
    Oasis::Add add {
        Oasis::Add {
            Oasis::Real { 1.0 },
            Oasis::Real { 2.0 } },
        Oasis::Real { 3.0 }
    };

    auto generalizedAdd = add.Generalize();
    auto result1 = Oasis::Add<Oasis::Real, Oasis::Add<Oasis::Real>>::Specialize(*generalizedAdd);
    REQUIRE(result1 != nullptr);

    Oasis::Multiply multiply {
        Oasis::Multiply {
            Oasis::Variable { "x" },
            Oasis::Variable { "y" } },
        Oasis::Variable { "z" }
    };

    auto generalizedMultiply = multiply.Generalize();
    auto result2 = Oasis::Multiply<Oasis::Variable, Oasis::Expression>::Specialize(*generalizedMultiply);
    REQUIRE(result2 != nullptr);
}

TEST_CASE("Specialize Recursively Considers Commutative Property", "[Symbolic]")
{
    Oasis::Add add {
        Oasis::Add {
            Oasis::Variable { "x" },
            Oasis::Real { 2.0 } },
        Oasis::Real { 3.0 }
    };

    auto generalizedAdd = add.Generalize();
    auto result1 = Oasis::Add<Oasis::Real, Oasis::Add<Oasis::Real, Oasis::Expression>>::Specialize(*generalizedAdd);
    REQUIRE(result1 != nullptr);

    Oasis::Multiply multiply {
        Oasis::Multiply {
            Oasis::Variable { "x" },
            Oasis::Real { 2 } },
        Oasis::Variable { "z" }
    };

    auto generalizedMultiply = multiply.Generalize();

    // intentionally out of order
    auto result2 = Oasis::Multiply<Oasis::Variable, Oasis::Multiply<Oasis::Real, Oasis::Variable>>::Specialize(*generalizedMultiply);
    REQUIRE(result2 != nullptr);
}

TEST_CASE("Flatten Function", "[Symbolic]")
{
    Oasis::Real real1 { 1.0 };
    Oasis::Real real2 { 2.0 };
    Oasis::Real real3 { 3.0 };

    Oasis::Add add {
        Oasis::Add {
            real1,
            real2 },
        real3
    };

    std::vector<std::unique_ptr<Oasis::Expression>> flattened;

    add.Flatten(flattened);

    std::vector<std::unique_ptr<Oasis::Expression>> expected;

    expected.emplace_back(real1.Copy());
    expected.emplace_back(real2.Copy());
    expected.emplace_back(real3.Copy());

    REQUIRE(flattened.size() == expected.size());

    for (int i = 0; i < flattened.size(); i++) {
        REQUIRE(flattened[i]->Equals(*expected[i]));
    }
}
//
// Created by Matthew McCall on 10/6/23.
//
#include "catch2/catch_test_macros.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Negate.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/RecursiveCast.hpp"
#include "Oasis/Variable.hpp"
#include "Oasis/SimplifyVisitor.hpp"

inline Oasis::SimplifyVisitor simplifyVisitor{};

TEST_CASE("Recursive Cast Considers Commutative Property", "[Symbolic]")
{
    Oasis::Add add {
        Oasis::Add {
            Oasis::Real { 1.0 },
            Oasis::Real { 2.0 } },
        Oasis::Real { 3.0 }
    };

    auto generalizedAdd = add.Generalize();
    auto result1 = Oasis::RecursiveCast<Oasis::Add<Oasis::Real, Oasis::Add<Oasis::Real>>>(*generalizedAdd);
    REQUIRE(result1 != nullptr);

    Oasis::Multiply multiply {
        Oasis::Multiply {
            Oasis::Variable { "x" },
            Oasis::Variable { "y" } },
        Oasis::Variable { "z" }
    };

    auto generalizedMultiply = multiply.Generalize();
    auto result2 = Oasis::RecursiveCast<Oasis::Multiply<Oasis::Variable, Oasis::Expression>>(*generalizedMultiply);
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
    auto result1 = Oasis::RecursiveCast<Oasis::Add<Oasis::Real, Oasis::Add<Oasis::Real, Oasis::Expression>>>(*generalizedAdd);
    REQUIRE(result1 != nullptr);

    Oasis::Multiply multiply {
        Oasis::Multiply {
            Oasis::Variable { "x" },
            Oasis::Real { 2 } },
        Oasis::Variable { "z" }
    };

    auto generalizedMultiply = multiply.Generalize();

    // intentionally out of order
    auto result2 = Oasis::RecursiveCast<Oasis::Multiply<Oasis::Variable, Oasis::Multiply<Oasis::Real, Oasis::Variable>>>(*generalizedMultiply);
    REQUIRE(result2 != nullptr);
}

TEST_CASE("Flatten Function", "[TreeManip]")
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

    for (unsigned int i = 0; i < flattened.size(); i++) {
        REQUIRE(flattened[i]->Equals(*expected[i]));
    }
}

TEST_CASE("BuildFromVector Function", "[TreeManip]")
{
    Oasis::Real real1 { 1.0 };
    Oasis::Real real2 { 2.0 };
    Oasis::Real real3 { 3.0 };
    Oasis::Real real4 { 4.0 };

    std::vector<std::unique_ptr<Oasis::Expression>> input;

    input.emplace_back(real1.Copy());
    input.emplace_back(real2.Copy());
    input.emplace_back(real3.Copy());

    SECTION("Vector who's size is odd")
    {
        Oasis::Add expected {
            Oasis::Add {
                real1,
                real2 },
            real3
        };

        auto result = Oasis::BuildFromVector<Oasis::Add>(input);

        REQUIRE(result != nullptr);
        REQUIRE(result->StructurallyEquivalent(expected));
    }

    input.emplace_back(real4.Copy());

    SECTION("Vector who's size is a power of 2")
    {
        Oasis::Add expected {
            Oasis::Add {
                real1,
                real2 },
            Oasis::Add {
                real3,
                real4 }
        };

        auto result = Oasis::BuildFromVector<Oasis::Add>(input);

        REQUIRE(result != nullptr);
        REQUIRE(result->StructurallyEquivalent(expected));
    }

    SECTION("Vector who's size is even, but not a power of 2")
    {
        Oasis::Real real5 { 5.0 };
        Oasis::Real real6 { 6.0 };

        input.emplace_back(real5.Copy());
        input.emplace_back(real6.Copy());

        Oasis::Add expected {
            Oasis::Add {
                Oasis::Add {
                    real1,
                    real2 },
                Oasis::Add {
                    real3,
                    real4 } },
            Oasis::Add {
                real5,
                real6 }
        };

        auto result = Oasis::BuildFromVector<Oasis::Add>(input);

        REQUIRE(result != nullptr);
        REQUIRE(result->StructurallyEquivalent(expected));
    }
}

TEST_CASE("Equals follows associativity and commutativity")
{
    Oasis::Real real1 { 1.0 };
    Oasis::Real real2 { 2.0 };
    Oasis::Real real3 { 3.0 };

    Oasis::Add add1 {
        Oasis::Add {
            real1,
            real2 },
        real3
    };

    // (1 + 2) + 3 == 1 + (2 + 3)
    SECTION("Associativity")
    {
        Oasis::Add add2 {
            real1,
            Oasis::Add {
                real2,
                real3 }
        };

        REQUIRE(add1.Equals(add2));
    }

    // (1 + 2) + 3 == 3 + (1 + 2)
    SECTION("Commutativity")
    {
        Oasis::Add add2 {
            real3,
            Oasis::Add {
                real1,
                real2 }
        };

        REQUIRE(add1.Equals(add2));
    }

    // (1 + 2) + 3 == (3 + 2) + 1
    SECTION("Associativity and Commutativity")
    {
        Oasis::Add add2 {
            Oasis::Add {
                real3,
                real2 },
            real1
        };

        REQUIRE(add1.Equals(add2));
    }
}
TEST_CASE("Substitute Binary", "[Substitute]")
{
    Oasis::Add<Oasis::Multiply<Oasis::Real, Oasis::Variable>> before {
            Oasis::Multiply<Oasis::Real, Oasis::Variable> {
                    Oasis::Real { 2.0 },
                    Oasis::Variable { "x" } },
            Oasis::Multiply<Oasis::Real, Oasis::Variable> {
                    Oasis::Real { 3.0 },
                    Oasis::Variable { "x" } } }; // 2x+3x

                    auto after = before.Substitute(Oasis::Variable { "x" }, Oasis::Real { 4.0 }); // after should some std::unique_ptr<Expression> such that it equals 2(4) + 3(4)
                    Oasis::Real twenty {20};
    REQUIRE(after->Equals(*(twenty.Accept(simplifyVisitor).value())));
}
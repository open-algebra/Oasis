//
// Created by Matthew McCall on 8/10/23.
//

#include "catch2/catch_test_macros.hpp"

#include "Oasis/Divide.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/RecursiveCast.hpp"
#include "Oasis/Variable.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/SimplifyVisitor.hpp"

inline Oasis::SimplifyVisitor simplifyVisitor{};

TEST_CASE("Division", "[Divide]")
{
    Oasis::Divide subtract {
        Oasis::Divide {
            Oasis::Real { 2.0 },
            Oasis::Real { 1.0 } },
        Oasis::Real { 2.0 }
    };

    auto simplified = subtract.Accept(simplifyVisitor).value();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 1.0);
}

TEST_CASE("Symbolic Division, equal variables", "[Division][Symbolic]")
{
    const Oasis::Divide div {
        Oasis::Multiply {
            Oasis::Real { 2.0 },
            Oasis::Variable { "x" } },
        Oasis::Multiply {
                Oasis::Real { 1.0 },
            Oasis::Variable { "x" } }
    };


    const Oasis::Divide div2 {
        Oasis::Multiply<> {
            Oasis::Real { 2.0 },
            Oasis::Variable{ "z" },
            Oasis::Variable{ "y" },
            Oasis::Variable{ "x" }
        },
        Oasis::Multiply<> {
            Oasis::Real { 1.0 },
            Oasis::Variable{ "y" },
            Oasis::Variable{ "x" },
            Oasis::Variable{ "z" }
            }
        };

    auto simplified = div.Accept(simplifyVisitor).value();
    REQUIRE(simplified->Equals(Oasis::Real { 2.0 }));

    auto simplified2 = div2.Accept(simplifyVisitor).value();
    REQUIRE(simplified2->Equals(Oasis::Real { 2.0 }));
}


TEST_CASE("Symbolic Division, unequal variables", "[Division][Symbolic]")
{
    Oasis::Divide div {
        Oasis::Multiply {
            Oasis::Real { 4.0 },
            Oasis::Multiply {
                Oasis::Variable { "z" },
                Oasis::Variable { "x" } }},
        Oasis::Multiply {
                Oasis::Real { 2.0 },
                Oasis::Multiply {
                    Oasis::Variable { "y" },
                    Oasis::Variable { "z" } }}
        };

    auto simplified = div.Accept(simplifyVisitor).value();

    REQUIRE(Oasis::Divide {
        Oasis::Multiply {
            Oasis::Real { 2.0 },
            Oasis::Variable { "x"} },
        Oasis::Variable { "y" } }.Equals(*simplified));
}


TEST_CASE("Symbolic Division of Expressions", "[Division][Symbolic]")
{
    //4(z^2+1)(x+1)/2(x+1)=2(z^2+1)

    Oasis::Divide div {
        Oasis::Multiply{
            Oasis::Multiply {
                Oasis::Add {
                    Oasis::Variable { "x" },
                    Oasis::Real { 1 } },
                Oasis::Add{
                    Oasis::Exponent {
                        Oasis::Variable { "z" },
                        Oasis::Real { 2 } },
                    Oasis::Real{ 1 }}},
            Oasis::Real { 4.0 }
        },
        Oasis::Multiply {
                Oasis::Real { 2.0 },
                Oasis::Add {
                    Oasis::Variable { "x" },
                    Oasis::Real { 1 } }}
        };

    auto simplified = div.Accept(simplifyVisitor).value();

    REQUIRE(Oasis::Multiply {
        Oasis::Real{ 2 },
        Oasis::Add{
            Oasis::Exponent {
                Oasis::Variable { "z" },
                Oasis::Real { 2 } },
            Oasis::Real{ 1 }},
        }.Equals(*simplified));
}


TEST_CASE("Symbolic Division, unequal exponents", "[Division][Symbolic]")
{
    Oasis::Divide div {
        Oasis::Multiply {
            Oasis::Real { 4.0 },
            Oasis::Exponent {
                Oasis::Variable { "z" },
                Oasis::Real { 2 } }},
        Oasis::Multiply {
                Oasis::Real { 2.0 },
                Oasis::Multiply {
                    Oasis::Variable { "y" },
                    Oasis::Variable { "z" } }}
        };

    auto simplified = div.Accept(simplifyVisitor).value();

    REQUIRE(Oasis::Divide {
        Oasis::Multiply {
            Oasis::Real { 2.0 },
            Oasis::Variable { "z"} },
        Oasis::Variable { "y" } }.Equals(*simplified));
}


TEST_CASE("Symbolic Division, equal exponents", "[Division][Symbolic]")
{
    Oasis::Divide div {
        Oasis::Multiply {
            Oasis::Real { 4.0 },
            Oasis::Exponent {
                Oasis::Variable { "z" },
                Oasis::Real { 3 } }},
        Oasis::Multiply {
                Oasis::Real { 2.0 },
                Oasis::Multiply {
                    Oasis::Variable { "y" },
                    Oasis::Variable { "z" } }}
        };

    auto simplified = div.Accept(simplifyVisitor).value();

    REQUIRE(Oasis::Divide {
        Oasis::Multiply {
            Oasis::Real { 2.0 },
            Oasis::Exponent {
                Oasis::Variable { "z" },
                Oasis::Real { 2 } } },
        Oasis::Variable { "y" } }.Equals(*simplified));
}

TEST_CASE("Division of equal variables", "[Division][Symbolic]")
{
    // x/x = 1

    Oasis::Divide div { Oasis::Variable { "x" }, Oasis::Variable { "x" } };
    auto simplified = div.Accept(simplifyVisitor).value();
    REQUIRE(Oasis::Real { 1 }.Equals(*simplified));
}


TEST_CASE("Generalized Division", "[Divide][Generalized]")
{
    Oasis::Divide<Oasis::Expression> subtract {
        Oasis::Divide<Oasis::Expression> {
            Oasis::Real { 2.0 },
            Oasis::Real { 1.0 } },
        Oasis::Real { 2.0 }
    };

    auto simplified = subtract.Accept(simplifyVisitor).value();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 1.0);
}

TEST_CASE("Divide Operator Overload", "[Divide][Operator Overload]")
{
    const std::unique_ptr<Oasis::Expression> a = std::make_unique<Oasis::Real>(1.0);
    const std::unique_ptr<Oasis::Expression> b = std::make_unique<Oasis::Real>(2.0);

    const auto sum = b/a;
    auto realSum = Oasis::RecursiveCast<Oasis::Real>(*sum);

    REQUIRE(realSum != nullptr);
    REQUIRE(realSum->GetValue() == 2.0);

}
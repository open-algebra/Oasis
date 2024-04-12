//
// Created by Matthew McCall on 8/10/23.
//

#include "catch2/catch_test_macros.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"

TEST_CASE("Multiplication", "[Multiply]")
{
    Oasis::Multiply subtract {
        Oasis::Multiply {
            Oasis::Real { 1.0 },
            Oasis::Real { 2.0 } },
        Oasis::Real { 3.0 }
    };

    auto simplified = subtract.Simplify();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 6.0);
}

TEST_CASE("Generalized Multiplication", "[Multiply][Generalized]")
{
    Oasis::Multiply<Oasis::Expression> subtract {
        Oasis::Multiply<Oasis::Expression> {
            Oasis::Real { 1.0 },
            Oasis::Real { 2.0 } },
        Oasis::Real { 3.0 }
    };

    auto simplified = subtract.Simplify();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 6.0);
}

TEST_CASE("Multiplication Async", "[Multiply][Async]")
{
    Oasis::Multiply subtract {
        Oasis::Multiply {
            Oasis::Real { 1.0 },
            Oasis::Real { 2.0 } },
        Oasis::Real { 3.0 }
    };

    std::unique_ptr<Oasis::Expression> simplified = subtract.SimplifyAsync();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 6.0);
}

TEST_CASE("Imaginary Multiplication", "[Imaginary][Multiplication]")
{
    Oasis::Multiply i2 {
        Oasis::Imaginary {},
        Oasis::Imaginary {}
    };
    Oasis::Multiply i3 {
        Oasis::Exponent {
            Oasis::Imaginary {},
            Oasis::Real { 2.0 } },
        Oasis::Imaginary {}
    };
    Oasis::Multiply i4 {
        Oasis::Exponent {
            Oasis::Imaginary {},
            Oasis::Real { 2.0 } },
        Oasis::Exponent {
            Oasis::Imaginary {},
            Oasis::Real { 2.0 } }
    };

    auto simplified2 = i2.Simplify();
    auto simplified3 = i3.Simplify();
    auto simplified4 = i4.Simplify();

    REQUIRE(Oasis::Multiply { Oasis::Real { -1 }, Oasis::Imaginary {} }.Equals(*simplified3));
    REQUIRE(Oasis::Real { -1.0 }.Equals(*simplified2));
    REQUIRE(Oasis::Real { 1.0 }.Equals(*simplified4));
}

TEST_CASE("Multiply Associativity", "[Multiply][Associativity]")
{
    Oasis::Multiply m1 {
        Oasis::Multiply {
            Oasis::Variable { "x" },
            Oasis::Real { 3.0 } },
        Oasis::Multiply {
            Oasis::Variable { "y" },
            Oasis::Exponent {
                Oasis::Variable { "x" },
                Oasis::Real { 4.0 } } }
    };

    auto simplified1 = m1.Simplify();

    REQUIRE(Oasis::Multiply {
        Oasis::Multiply {
            Oasis::Exponent {
                Oasis::Variable { "x" },
                Oasis::Real { 5.0 } },
            Oasis::Real { 3.0 } },
        Oasis::Variable { "y" } }
                .Equals(*simplified1));
}
TEST_CASE("Multiply Operator Overload", "[Multiply][Operator Overload]")
{
    const std::unique_ptr<Oasis::Expression> a = std::make_unique<Oasis::Real>(1.0);
    const std::unique_ptr<Oasis::Expression> b = std::make_unique<Oasis::Real>(2.0);

    const auto sum = a*b;
    auto realSum = Oasis::Real::Specialize(*sum);

    REQUIRE(realSum != nullptr);
    REQUIRE(realSum->GetValue() == 2.0);
}

TEST_CASE("Variadic Multiply Constructor", "[Multiply]")
{
    const Oasis::Multiply<> multiply {
        Oasis::Real { 1.0 },
        Oasis::Real { 2.0 },
        Oasis::Real { 3.0 },
        Oasis::Real { 4.0 },
        Oasis::Add<> {
            Oasis::Real { 5.0 },
            Oasis::Real { 6.0 },
                Oasis::Real { 7.0 }}
    };

    const Oasis::Real expected { 432.0 };

    const auto simplified = multiply.Simplify();
    REQUIRE(expected.Equals(*simplified));
}
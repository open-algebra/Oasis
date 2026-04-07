//
// Created by Matthew McCall on 8/10/23.
//

#include "catch2/catch_test_macros.hpp"
#include "Common.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/InFixSerializer.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/RecursiveCast.hpp"
#include <Oasis/Divide.hpp>

inline Oasis::SimplifyVisitor simplifyVisitor{};

TEST_CASE("Multiplication", "[Multiply]")
{
    Oasis::Multiply subtract {
        Oasis::Multiply {
            Oasis::Real { 1.0 },
            Oasis::Real { 2.0 } },
        Oasis::Real { 3.0 }
    };

    auto simplified = subtract.Accept(simplifyVisitor).value();
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

    auto simplified = subtract.Accept(simplifyVisitor).value();
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

    auto simplified2 = i2.Accept(simplifyVisitor).value();
    auto simplified3 = i3.Accept(simplifyVisitor).value();
    auto simplified4 = i4.Accept(simplifyVisitor).value();

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

    auto simplified1 = m1.Accept(simplifyVisitor).value();

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
    auto realSum = Oasis::RecursiveCast<Oasis::Real>(*sum);

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

    const auto simplified = multiply.Accept(simplifyVisitor).value();
    REQUIRE(expected.Equals(*simplified));
}

TEST_CASE("Multiplying Fractions", "[Multiply]")
{
    const Oasis::Multiply<Oasis::Divide<>,Oasis::Divide<>> multiply {
            Oasis::Divide<> { Oasis::Multiply{ Oasis::Variable{ "a"},
                        Oasis::Variable{"b"} },
                Oasis::Multiply{ Oasis::Variable{ "c"},
                        Oasis::Real{4.0} } },
            Oasis::Divide<> { Oasis::Multiply{ Oasis::Variable{ "c"},
                    Oasis::Real{12.0} },
                Oasis::Multiply{ Oasis::Variable{ "b"},
                        Oasis::Variable{"a"} } }
    };

    const Oasis::Real expected { 3.0 };

    const auto simplified = multiply.Accept(simplifyVisitor).value();
    REQUIRE(expected.Equals(*simplified));
}

TEST_CASE("Multiplying Linear Expressions", "[Multiply]")
{
    // 2*(4x-5)
    Oasis::Multiply multiply {
        Oasis::Real{2},
            Oasis::Add{
                Oasis::Multiply{
                    Oasis::Real{4},
                    Oasis::Variable{"x"}},
                Oasis::Real{-5}}
    };

    Oasis::SimplifyOpts opts = {.distributivePolicy = Oasis::SimplifyOpts::DistributivePolicy::PREFER};
    Oasis::SimplifyVisitor distributiveSimplifyVisitor { opts};
    const auto multiply_simplified = multiply.Accept(distributiveSimplifyVisitor).value();

    // 8x-10
    const Oasis::Add expected {

            Oasis::Multiply{
                Oasis::Real{8},
                Oasis::Variable{"x"}},
            Oasis::Real{-10}
    };

    OASIS_CAPTURE_WITH_SERIALIZER((*multiply_simplified));

    REQUIRE(multiply_simplified->Equals(expected));
}

TEST_CASE("Multiplying Quadratics", "[Multiply]")
{
    // 2*(3x^2+4x-5)
    Oasis::Multiply polynom {
        Oasis::Real{2},
        Oasis::Add {
            Oasis::Multiply{
                Oasis::Real{3},
                Oasis::Multiply{ Oasis::Variable{"x"}, Oasis::Variable{"x"}}},
            Oasis::Add{
                Oasis::Multiply{
                    Oasis::Real{4},
                    Oasis::Variable{"x"}},
                Oasis::Real{-5}}
        }
    };

    Oasis::SimplifyOpts opts = {.distributivePolicy = Oasis::SimplifyOpts::DistributivePolicy::PREFER};
    Oasis::SimplifyVisitor distributiveSimplifyVisitor { opts};
    const auto simplified = polynom.Accept(distributiveSimplifyVisitor).value();

    // 6x^2+8x-10
    const Oasis::Add expected {
        Oasis::Multiply{
            Oasis::Real{6},
            Oasis::Multiply{ Oasis::Variable{"x"}, Oasis::Variable{"x"}}},
        Oasis::Add{
            Oasis::Multiply{
                Oasis::Real{8},
                Oasis::Variable{"x"}},
            Oasis::Real{-10}}
    };

    auto expected_simpl = expected.Accept(simplifyVisitor).value();

    OASIS_CAPTURE_WITH_SERIALIZER(*simplified);
    OASIS_CAPTURE_WITH_SERIALIZER(*expected_simpl);

    REQUIRE(simplified->Equals(*expected_simpl));
}
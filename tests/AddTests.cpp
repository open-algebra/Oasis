//
// Created by Matthew McCall on 8/7/23.
//
#include "Common.hpp"

#include "catch2/catch_test_macros.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/RecursiveCast.hpp"
#include "Oasis/Variable.hpp"
#include "Oasis/SimplifyVisitor.hpp"

namespace { Oasis::SimplifyVisitor simplifyVisitor{}; }

TEST_CASE("Addition", "[Add]")
{
    Oasis::Add add {
        Oasis::Add {
            Oasis::Real { 1.0 },
            Oasis::Real { 2.0 } },
        Oasis::Real { 3.0 }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(add);

    auto simplified = add.Accept(simplifyVisitor).value();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 6.0);
}

TEST_CASE("Symbolic Addition", "[Add][Symbolic]")
{
    Oasis::Add add {
        Oasis::Multiply {
            Oasis::Real { 1.0 },
            Oasis::Variable { "x"} },
        Oasis::Multiply {
                Oasis::Real { 2.0 },
                Oasis::Variable { "x" } }
        };

    auto simplified = add.Accept(simplifyVisitor).value();
    REQUIRE(simplified->Is<Oasis::Multiply>());

    REQUIRE(Oasis::Multiply {
        Oasis::Real { 3.0 },
        Oasis::Variable { "x" } }.Equals(*simplified));
}

TEST_CASE("Symbolic Addition, variable case", "[Add][Symbolic]")
{
    Oasis::Add add {
        Oasis::Add {
            Oasis::Real { 1.0 },
            Oasis::Variable { "x" } },
        Oasis::Multiply {
                Oasis::Real { 2.0 },
                Oasis::Variable { "x" } }
        };

    auto simplified = add.Accept(simplifyVisitor).value();

    REQUIRE(Oasis::Add {
        Oasis::Real { 1.0 },
        Oasis::Multiply {
            Oasis::Real { 3.0 },
            Oasis::Variable { "x" } }}.Equals(*simplified));
}

TEST_CASE("Generalized Addition", "[Add][Generalized]")
{
    Oasis::Add<Oasis::Expression> add {
        Oasis::Add<Oasis::Expression> {
            Oasis::Real { 1.0 },
            Oasis::Real { 2.0 } },
        Oasis::Real { 3.0 }
    };

    auto simplified = add.Accept(simplifyVisitor).value();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 6.0);
}

TEST_CASE("Structurally Equivalent", "[StructurallyEquivalent]")
{
    REQUIRE(
        Oasis::Add {
            Oasis::Real {},
            Oasis::Real {} }
            .StructurallyEquivalent(
                Oasis::Add {
                    Oasis::Real {},
                    Oasis::Real {} }));
}

TEST_CASE("Generalized Structurally Equivalent", "[StructurallyEquivalent][Generalized]")
{
    REQUIRE(
        Oasis::Add<Oasis::Expression> {
            Oasis::Real {},
            Oasis::Real {} }
            .StructurallyEquivalent(
                Oasis::Add<Oasis::Expression> {
                    Oasis::Real {},
                    Oasis::Real {} }));
}

TEST_CASE("Specialization", "[Specialization]")
{
    REQUIRE(Oasis::RecursiveCast<Oasis::Add<Oasis::Real>>(
        Oasis::Add<Oasis::Expression> {
            Oasis::Real {},
            Oasis::Real {} }
        ));
}

TEST_CASE("Imaginary Addition", "[Imaginary][Add]")
{
    Oasis::Add a1 {
        Oasis::Imaginary {},
        Oasis::Imaginary {}
    };

    auto spec1 = a1.Accept(simplifyVisitor).value();

    REQUIRE(Oasis::Multiply { Oasis::Real { 2.0 }, Oasis::Imaginary {} }.Equals(*spec1));
}

TEST_CASE("Addition Associativity", "[Add][Associative]")
{
    Oasis::Add assoc1 {
        Oasis::Real { 2.0 },
        Oasis::Add {
            Oasis::Variable { "x" },
            Oasis::Add {
                Oasis::Add {
                    Oasis::Real { 3.0 },
                    Oasis::Variable { "x" } },
                Oasis::Multiply {
                    Oasis::Real { 4.0 },
                    Oasis::Variable { "x" } } } }
    };

    auto simplified1 = assoc1.Accept(simplifyVisitor).value();

    REQUIRE(Oasis::Add {
        Oasis::Real { 5.0 },
        Oasis::Multiply {
            Oasis::Real { 6.0 },
            Oasis::Variable { "x" } } }
                .Equals(*simplified1));
}

TEST_CASE("Add Associativity with wider tree", "[Add][Associativity]")
{
    Oasis::Add assoc2 {
        Oasis::Multiply {
            Oasis::Real { 2.0 },
            Oasis::Exponent {
                Oasis::Variable { "x" },
                Oasis::Real { 2.0 } } },
        Oasis::Add {
            Oasis::Exponent {
                Oasis::Variable { "x" },
                Oasis::Real { 2.0 } },
            Oasis::Add {
                Oasis::Add {
                    Oasis::Real { 3.0 },
                    Oasis::Variable { "x" } },
                Oasis::Multiply {
                    Oasis::Real { 4.0 },
                    Oasis::Variable { "x" } } } }
    };

    auto simplified2 = assoc2.Accept(simplifyVisitor).value();

    REQUIRE(Oasis::Add {
        Oasis::Add {

            Oasis::Multiply {
                Oasis::Real { 3.0 },
                Oasis::Exponent {
                    Oasis::Variable { "x" },
                    Oasis::Real { 2.0 } } },
            Oasis::Real { 3.0 },
        },
        Oasis::Multiply {
            Oasis::Real { 5.0 },
            Oasis::Variable { "x" } } }
                .Equals(*simplified2));
}


TEST_CASE("Add Operator Overload", "[Add][Operator Overload]")
{
    const std::unique_ptr<Oasis::Expression> a = std::make_unique<Oasis::Real>(1.0);
    const std::unique_ptr<Oasis::Expression> b = std::make_unique<Oasis::Real>(2.0);

    const auto sum = a+b;
    auto realSum = Oasis::RecursiveCast<Oasis::Real>(*sum);

    REQUIRE(realSum != nullptr);
    REQUIRE(realSum->GetValue() == 3.0);

}

TEST_CASE("Variadic Add Constructor", "[Add]")
{
    const Oasis::Add<> add {
        Oasis::Real { 1.0 },
        Oasis::Real { 2.0 },
        Oasis::Real { 3.0 },
        Oasis::Real { 4.0 },
        Oasis::Multiply {
            Oasis::Real { 5.0 },
            Oasis::Real { 6.0 } }
    };

    const Oasis::Real expected { 40.0 };

    const auto simplified = add.Accept(simplifyVisitor).value();
    REQUIRE(expected.Equals(*simplified));
}
//
// Created by Matthew McCall on 4/5/24.
//

#include "Common.hpp"
#include "catch2/catch_test_macros.hpp"

#include <Oasis/Negate.hpp>
#include "Oasis/Subtract.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Variable.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/SimplifyVisitor.hpp"

inline Oasis::SimplifyVisitor simplifyVisitor{};

TEST_CASE("Negate", "[Negate]")
{
    const Oasis::Negate negativeOne {
        Oasis::Real { 1.0 }
    };

    const auto simplified = negativeOne.Accept(simplifyVisitor).value();

    const Oasis::Real expected { -1.0 };
    REQUIRE(simplified->Equals(expected));
}

TEST_CASE("Quadratic Negation", "[Negate]")
{
    // -(3x^2+4x-5)
    Oasis::Negate polynom_negated {
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

    const auto polynom_negated_simpl = polynom_negated.Accept(simplifyVisitor).value();

    // -3x^2-4x+5
    const auto expected = Oasis::Add {
        Oasis::Multiply{
            Oasis::Real{-3},
            Oasis::Multiply{ Oasis::Variable{"x"}, Oasis::Variable{"x"}}},
        Oasis::Add{
            Oasis::Multiply{
                Oasis::Real{-4},
                Oasis::Variable{"x"}},
            Oasis::Real{5}}
    }.Accept(simplifyVisitor).value();

    OASIS_CAPTURE_WITH_SERIALIZER(*polynom_negated_simpl);
    OASIS_CAPTURE_WITH_SERIALIZER(*expected);

    REQUIRE(polynom_negated_simpl->Equals(*expected));
}
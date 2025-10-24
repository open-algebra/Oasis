//
// Created by Matthew McCall on 8/10/23.
//

#include "catch2/catch_test_macros.hpp"

#include "Oasis/Imaginary.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/RecursiveCast.hpp"
#include "Oasis/SimplifyVisitor.hpp"

inline Oasis::SimplifyVisitor simplifyVisitor{};

TEST_CASE("Subtraction", "[Subtract]")
{
    Oasis::Subtract subtract {
        Oasis::Subtract {
            Oasis::Real { 1.0 },
            Oasis::Real { 2.0 } },
        Oasis::Real { 3.0 }
    };

    auto simplified = subtract.Accept(simplifyVisitor).value();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == -4.0);
}

TEST_CASE("Generalized Subtraction", "[Subtract][Generalized]")
{
    Oasis::Subtract<Oasis::Expression> subtract {
        Oasis::Subtract<Oasis::Expression> {
            Oasis::Real { 1.0 },
            Oasis::Real { 2.0 } },
        Oasis::Real { 3.0 }
    };

    auto simplified = subtract.Accept(simplifyVisitor).value();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == -4.0);
}

TEST_CASE("Imaginary Subtration", "[Subtract][Imaginary]")
{
    Oasis::Subtract s1 {
        Oasis::Multiply {
            Oasis::Real { 3.0 },
            Oasis::Imaginary {} },
        Oasis::Imaginary {}
    };

    auto spec1 = s1.Accept(simplifyVisitor).value();

    REQUIRE(Oasis::Multiply { Oasis::Real { 2.0 }, Oasis::Imaginary {} }.Equals(*spec1));
}

TEST_CASE("Subtract Operator Overload", "[Subtract][Operator Overload]")
{
    const std::unique_ptr<Oasis::Expression> a = std::make_unique<Oasis::Real>(1.0);
    const std::unique_ptr<Oasis::Expression> b = std::make_unique<Oasis::Real>(2.0);

    const auto sum = a-b;
    auto realSum = Oasis::RecursiveCast<Oasis::Real>(*sum);

    REQUIRE(realSum != nullptr);
    REQUIRE(realSum->GetValue() == -1.0);

}

TEST_CASE("Simplify Equation with subtraction", "[Subtract]") {
    Oasis::Add add1{ // 4x - 7y + -5
            Oasis::Subtract{
                    Oasis::Multiply{
                            Oasis::Real{4.0},
                            Oasis::Variable{"x"}},
                    Oasis::Multiply{
                            Oasis::Real{7.0},
                            Oasis::Variable{"y"}}},
            Oasis::Real{-5.0}
    };

    auto simplified = add1.Accept(simplifyVisitor).value();
}
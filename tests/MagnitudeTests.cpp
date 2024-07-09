//
// Created by Andrew Nazareth on 6/28/24.
//

#include "catch2/catch_test_macros.hpp"
#include "Oasis/Magnitude.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Subtract.hpp"

TEST_CASE("Magnitude of Real", "[Magnitude][Real]")
{
    Oasis::Magnitude mag1{Oasis::Real{5.0}};
    Oasis::Magnitude mag2{Oasis::Real{-75.0}};
    Oasis::Magnitude mag3{Oasis::Add{Oasis::Real{5}, Oasis::Real{6}}};
    Oasis::Magnitude mag4{Oasis::Subtract{Oasis::Real{5}, Oasis::Real{6}}};

    auto simp1 = mag1.Simplify();
    auto simp2 = mag2.Simplify();
    auto simp3 = mag3.Simplify();
    auto simp4 = mag4.Simplify();

    REQUIRE(simp1->Equals(Oasis::Real{5.0}));
    REQUIRE(simp2->Equals(Oasis::Real{75.0}));
    REQUIRE(simp3->Equals(Oasis::Real{11.0}));
    REQUIRE(simp4->Equals(Oasis::Real{1.0}));
}

TEST_CASE("Magnitude of Imaginary", "[Magnitude][Imaginary]")
{
    Oasis::Magnitude mag1{Oasis::Imaginary{}};
    Oasis::Magnitude mag2{Oasis::Multiply{Oasis::Real{5.0}, Oasis::Imaginary{}}};
    Oasis::Magnitude mag3{Oasis::Multiply{Oasis::Real{-5.0}, Oasis::Imaginary{}}};

    auto simp1 = mag1.Simplify();
    auto simp2 = mag2.Simplify();
    auto simp3 = mag3.Simplify();

    REQUIRE(simp1->Equals(Oasis::Real{1.0}));
    REQUIRE(simp2->Equals(Oasis::Real{5}));
    REQUIRE(simp3->Equals(Oasis::Real{5}));
}
// Magnitude of Complex
// Magnitude of Matrices and Vectors
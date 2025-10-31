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
#include "Oasis/Matrix.hpp"
#include "Oasis/SimplifyVisitor.hpp"

inline Oasis::SimplifyVisitor simplifyVisitor{};

TEST_CASE("Magnitude of Real", "[Magnitude][Real]")
{
    Oasis::Magnitude mag1{Oasis::Real{5.0}};
    Oasis::Magnitude mag2{Oasis::Real{-75.0}};
    Oasis::Magnitude mag3{Oasis::Add{Oasis::Real{5}, Oasis::Real{6}}};
    Oasis::Magnitude mag4{Oasis::Subtract{Oasis::Real{5}, Oasis::Real{6}}};

    auto simp1 = mag1.Accept(simplifyVisitor).value();
    auto simp2 = mag2.Accept(simplifyVisitor).value();
    auto simp3 = mag3.Accept(simplifyVisitor).value();
    auto simp4 = mag4.Accept(simplifyVisitor).value();

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

    auto simp1 = mag1.Accept(simplifyVisitor).value();
    auto simp2 = mag2.Accept(simplifyVisitor).value();
    auto simp3 = mag3.Accept(simplifyVisitor).value();

    REQUIRE(simp1->Equals(Oasis::Real{1.0}));
    REQUIRE(simp2->Equals(Oasis::Real{5}));
    REQUIRE(simp3->Equals(Oasis::Real{5}));
}

TEST_CASE("Magnitude of Complex", "[Magnitude][Real][Imaginary]")
{
    Oasis::Magnitude mag1{Oasis::Add{Oasis::Real{5.0}, Oasis::Imaginary{}}};
    Oasis::Magnitude mag2{Oasis::Subtract{Oasis::Real{5.0}, Oasis::Imaginary{}}};

    auto simp1 = mag1.Accept(simplifyVisitor).value();
    auto simp2 = mag2.Accept(simplifyVisitor).value();

    REQUIRE(simp1->Equals(*Oasis::Exponent{Oasis::Real{26.0}, Oasis::Real{0.5}}.Accept(simplifyVisitor).value()));
    REQUIRE(simp2->Equals(*Oasis::Exponent{Oasis::Real{26.0}, Oasis::Real{0.5}}.Accept(simplifyVisitor).value()));

    Oasis::Magnitude mag3{Oasis::Add{Oasis::Real{5.0}, Oasis::Multiply{Oasis::Real{4.0}, Oasis::Imaginary{}}}};
    Oasis::Magnitude mag4{Oasis::Subtract{Oasis::Real{7.0}, Oasis::Multiply{Oasis::Real{20.0}, Oasis::Imaginary{}}}};

    auto simp3 = mag3.Accept(simplifyVisitor).value();
    auto simp4 = mag4.Accept(simplifyVisitor).value();

    REQUIRE(simp3->Equals(*Oasis::Exponent{Oasis::Real{41.0}, Oasis::Real{0.5}}.Accept(simplifyVisitor).value()));
    REQUIRE(simp4->Equals(*Oasis::Exponent{Oasis::Real{449.0}, Oasis::Real{0.5}}.Accept(simplifyVisitor).value()));
}

TEST_CASE("Magnitude of Vector", "[Magnitude][Matrix1D]")
{
    std::vector<double> vals = {5,6,7,8};
    Oasis::Magnitude mag1{Oasis::Matrix{4, 1, vals}};
    Oasis::Magnitude mag2{Oasis::Matrix{1, 4, vals}};
    auto expected = Oasis::Exponent{Oasis::Real{174}, Oasis::Real{0.5}}.Accept(simplifyVisitor).value();
    REQUIRE((mag1.Accept(simplifyVisitor).value())->Equals(*expected));
    REQUIRE((mag2.Accept(simplifyVisitor).value())->Equals(*expected));
}

TEST_CASE("Magnitude of Matrix", "[Magnitude][MatrixXXD]")
{
    std::vector<double> vals = {5,6,7,8};
    Oasis::Magnitude mag1{Oasis::Matrix{2, 2, vals}};
    auto expected = Oasis::Exponent{Oasis::Real{174}, Oasis::Real{0.5}}.Accept(simplifyVisitor).value();
    auto actual = mag1.Accept(simplifyVisitor).value();
    REQUIRE(actual->Equals(*expected));
}
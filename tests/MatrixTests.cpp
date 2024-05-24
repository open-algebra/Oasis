//
// Created by Andrew Nazareth on 5/24/24.
//

#include "catch2/catch_test_macros.hpp"
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "Oasis/Matrix.hpp"
#include "Oasis/Add.hpp"

#define EPSILON 10E-6

TEST_CASE("Create Empty Matrix", "[Matrix][Create][Empty]")
{
    Oasis::Matrix a = {3,4};

    REQUIRE(a.GetCols() == 4);
    REQUIRE(a.GetRows() == 3);
}

TEST_CASE("Empty Matrix Equality", "[Matrix][Empty]")
{
    Oasis::Matrix a = {3,4};
    Oasis::Matrix b = {3,4};

    REQUIRE(a.Equals(b));
}

TEST_CASE("Create Matrix From MatrixXXD", "[Matrix][Create][MatrixXXD]")
{
    Oasis::MatrixXXD a{2,2};
    a(0,0) = 1;
    a(0,1) = 2;
    a(1,0) = 3;
    a(1,1) = 4;

    Oasis::Matrix mat{a};

    Oasis::Matrix mat2{Oasis::MatrixXXD{{1,2},{3,4}}};

    Oasis::MatrixXXD b{2,2};
    b << 1, 2, 3, 4;

    Oasis::Matrix mat3{b};

    REQUIRE(mat.GetMatrix() == a);
    REQUIRE(mat2.Equals(mat));
    REQUIRE(mat3.Equals(mat));
}

TEST_CASE("Add Matrices", "[Matrix][Add]")
{
    Oasis::Matrix mat1{Oasis::MatrixXXD{{1,2},
                                        {3,4}}};
    Oasis::Matrix mat2{Oasis::MatrixXXD{{8,7},
                                        {6,5}}};

    auto result = Oasis::Add<Oasis::Expression>{mat1, mat2}.Simplify();
    auto spec = Oasis::Matrix::Specialize(*result);

    REQUIRE(spec != nullptr);
    REQUIRE(spec->Equals(Oasis::Matrix{Oasis::MatrixXXD{{9,9},{9,9}}}));
}
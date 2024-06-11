//
// Created by Andrew Nazareth on 5/24/24.
//

#include "catch2/catch_test_macros.hpp"
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "Oasis/Matrix.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Divide.hpp"

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

TEST_CASE("Create Matrix from Vector", "[Matrix][Create][Vector]")
{
    std::vector<double> matrix = {1,2,3,4};
    Oasis::Matrix even{2,2,matrix};
    Oasis::Matrix less{1,2,matrix};
    Oasis::Matrix more{2,3,matrix};
    Oasis::Matrix less2{2,1,matrix};

    INFO("Even:");
    INFO(even.GetMatrix());
    INFO("Less:");
    INFO(less.GetMatrix());
    INFO("More:");
    INFO(more.GetMatrix());
    INFO("Less2:");
    INFO(less2.GetMatrix());
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
    // std::cout<<spec->GetMatrix()<<std::endl;
}

TEST_CASE("Subtract Matrices", "[Matrix][Subtract]")
{
    Oasis::Matrix mat1{Oasis::MatrixXXD{{1,2},
                                        {3,4}}};
    Oasis::Matrix mat2{Oasis::MatrixXXD{{5,6},
                                        {7,8}}};

    auto result = Oasis::Subtract<Oasis::Expression, Oasis::Expression>{mat2, mat1}.Simplify();
    auto spec = Oasis::Matrix::Specialize(*result);

    REQUIRE(spec != nullptr);
    REQUIRE(spec->Equals(Oasis::Matrix{Oasis::MatrixXXD{{4,4},{4,4}}}));
}

TEST_CASE("Multiply Matrix and Real", "[Matrix][Real][Multiply]")
{
    Oasis::Real a {5.0};
    Oasis::Matrix mat1{Oasis::MatrixXXD{{1,2},
                                        {3,4}}};

    auto result1 = Oasis::Multiply{a, mat1}.Simplify();
    auto result2 = Oasis::Multiply{mat1, a}.Simplify();
    auto spec1 = Oasis::Matrix::Specialize(*result1);
    auto spec2 = Oasis::Matrix::Specialize(*result2);

    REQUIRE(spec1 != nullptr);
    REQUIRE(spec2 != nullptr);

    REQUIRE(spec1->Equals(Oasis::Matrix{Oasis::MatrixXXD{{5.0, 10.0},{15.0, 20.0}}}));
    REQUIRE(spec2->Equals(Oasis::Matrix{Oasis::MatrixXXD{{5.0, 10.0},{15.0, 20.0}}}));
}

TEST_CASE("Create Identity Matrix", "[Matrix][Identity]")
{
    Oasis::MatrixXXD mat(3,3);
    Oasis::Matrix Oasis::Matrix{mat}.Identity();
}

TEST_CASE("Multiply Matrices, same dimensions", "[Matrix][Multiply]")
{

}

TEST_CASE("Multiply Matrices, different dimensions", "[Matrix][Multiply]")
{

}

TEST_CASE("Addition of Matrix and Real", "[Matrix][Real][Add]")
{

}

TEST_CASE("Subtraction of Matrix and Real", "[Matrix][Real][Subtract]")
{

}
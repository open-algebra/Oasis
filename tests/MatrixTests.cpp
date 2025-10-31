//
// Created by Andrew Nazareth on 5/24/24.
//

#include "catch2/catch_test_macros.hpp"
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "Oasis/Matrix.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/RecursiveCast.hpp"
#include "Oasis/SimplifyVisitor.hpp"

#define EPSILON 10E-6

inline Oasis::SimplifyVisitor simplifyVisitor{};

TEST_CASE("Create Empty Matrix", "[Matrix][Create][Empty]")
{
    Oasis::Matrix a = {3,4};

    REQUIRE(a.GetCols() == 4);
    REQUIRE(a.GetRows() == 3);
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

    auto result = Oasis::Add<Oasis::Expression>{mat1, mat2}.Accept(simplifyVisitor).value();
    auto spec = Oasis::RecursiveCast<Oasis::Matrix>(*result);

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

    auto result = Oasis::Subtract<Oasis::Expression, Oasis::Expression>{mat2, mat1}.Accept(simplifyVisitor).value();
    auto spec = Oasis::RecursiveCast<Oasis::Matrix>(*result);

    REQUIRE(spec != nullptr);
    REQUIRE(spec->Equals(Oasis::Matrix{Oasis::MatrixXXD{{4,4},{4,4}}}));
}

TEST_CASE("Multiply Matrix and Real", "[Matrix][Real][Multiply]")
{
    Oasis::Real a {5.0};
    Oasis::Matrix mat1{Oasis::MatrixXXD{{1,2},
                                        {3,4}}};

    auto result1 = Oasis::Multiply{a, mat1}.Accept(simplifyVisitor).value();
    auto result2 = Oasis::Multiply{mat1, a}.Accept(simplifyVisitor).value();
    auto spec1 = Oasis::RecursiveCast<Oasis::Matrix>(*result1);
    auto spec2 = Oasis::RecursiveCast<Oasis::Matrix>(*result2);

    REQUIRE(spec1 != nullptr);
    REQUIRE(spec2 != nullptr);

    REQUIRE(spec1->Equals(Oasis::Matrix{Oasis::MatrixXXD{{5.0, 10.0},{15.0, 20.0}}}));
    REQUIRE(spec2->Equals(Oasis::Matrix{Oasis::MatrixXXD{{5.0, 10.0},{15.0, 20.0}}}));
}

TEST_CASE("Create Identity Matrix", "[Matrix][Identity]")
{
    Oasis::MatrixXXD mat(3,3);
    Oasis::Matrix i{mat};
    Oasis::Matrix identity = *(Oasis::RecursiveCast<Oasis::Matrix>(*i.Identity()));

    mat << 1,0,0,
           0,1,0,
           0,0,1;

    REQUIRE(identity.GetMatrix() == mat);
}

TEST_CASE("Multiply Matrices, same dimensions", "[Matrix][Multiply]")
{
    Oasis::Matrix mat1{Oasis::MatrixXXD{{5,6},{7,8}}};
    Oasis::Matrix mat2{Oasis::MatrixXXD{{1,2},{3,4}}};
    Oasis::Matrix expected{Oasis::MatrixXXD{{23,34},{31,46}}};
    auto result = Oasis::Multiply{mat1, mat2}.Accept(simplifyVisitor).value();
    REQUIRE(result->Equals(expected));
}

TEST_CASE("Multiply Matrices, different dimensions", "[Matrix][Multiply]")
{
    Oasis::Matrix mat1{Oasis::MatrixXXD{{1,2},{3,4}}};
    Oasis::Matrix mat2{Oasis::MatrixXXD{{1,2},{3,4},{5,6}}};
    Oasis::Matrix mat3{Oasis::MatrixXXD{{1,2,3},{4,5,6}}};

    Oasis::Multiply expected12{mat1, mat2};
    Oasis::Matrix expected21{Oasis::MatrixXXD{{7,10},{15,22},{23,34}}};
    Oasis::Matrix expected13{Oasis::MatrixXXD{{9,12,15},{19,26,33}}};
    Oasis::Multiply expected31{mat3, mat1};
    Oasis::Matrix expected23{Oasis::MatrixXXD{{9,12,15},{19,26,33},{29,40,51}}};
    Oasis::Matrix expected32{Oasis::MatrixXXD{{22,28},{49,64}}};

    auto res12 = Oasis::Multiply{mat1, mat2}.Accept(simplifyVisitor).value();
    auto res21 = Oasis::Multiply{mat2, mat1}.Accept(simplifyVisitor).value();
    auto res13 = Oasis::Multiply{mat1, mat3}.Accept(simplifyVisitor).value();
    auto res31 = Oasis::Multiply{mat3, mat1}.Accept(simplifyVisitor).value();
    auto res23 = Oasis::Multiply{mat2, mat3}.Accept(simplifyVisitor).value();
    auto res32 = Oasis::Multiply{mat3, mat2}.Accept(simplifyVisitor).value();

    REQUIRE(res12->Equals(expected12));
    REQUIRE(res21->Equals(expected21));
    REQUIRE(res13->Equals(expected13));
    REQUIRE(res31->Equals(expected31));
    REQUIRE(res23->Equals(expected23));
    REQUIRE(res32->Equals(expected32));
}
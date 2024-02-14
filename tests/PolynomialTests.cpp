//
// Created by Matthew McCall on 8/7/23.
//
#include "catch2/catch_test_macros.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Expression.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Variable.hpp"
#include "Oasis/Divide.hpp"
#include <tuple>
#include <vector>
#include <set>


TEST_CASE("7th degree polynomial with rational roots", "[factor][duplicateRoot]")
{
    Oasis::Add add {
        Oasis::Add {
            Oasis::Add {
                Oasis::Real {222126775},
                Oasis::Multiply {
                    Oasis::Real { -1989731815 },
                    Oasis::Variable { "x" } },
            },
            Oasis::Add {
                Oasis::Multiply {
                    Oasis::Real { 6674884402 },
                    Oasis::Exponent{
                        Oasis::Variable{ "x" },
                        Oasis::Real{2}}},
                Oasis::Multiply {
                    Oasis::Real { -9967889122 },
                    Oasis::Exponent{
                        Oasis::Variable{ "x" },
                        Oasis::Real{3}}},
            }},
        Oasis::Add {
            Oasis::Add {
                Oasis::Multiply {
                    Oasis::Real { 5069070055 },
                    Oasis::Exponent{
                        Oasis::Variable{ "x" },
                        Oasis::Real{4}}},
                Oasis::Multiply {
                    Oasis::Real { 2370752969 },
                    Oasis::Exponent{
                        Oasis::Variable{ "x" },
                        Oasis::Real{5}}},
            },
            Oasis::Add {
                Oasis::Multiply {
                    Oasis::Real { -2841027600 },
                    Oasis::Exponent{
                        Oasis::Variable{ "x" },
                        Oasis::Real{6}}},
                Oasis::Multiply {
                    Oasis::Real { 446760000 },
                    Oasis::Exponent{
                        Oasis::Variable{ "x" },
                        Oasis::Real{7}}},
            }}
    };
    auto zeros = add.FindZeros();
    REQUIRE(zeros.size() == 6);
    std::set<std::tuple<long,long>> goalSet = {std::tuple(1,3),std::tuple(67,73),std::tuple(13,17),std::tuple(-5,3),std::tuple(101,200),std::tuple(5,1)};
    for(auto& i: zeros){
        auto divideCase = Oasis::Divide<Oasis::Real>::Specialize(*i);
        REQUIRE(divideCase!=nullptr);
        std::tuple<long,long> asTuple = std::tuple(lround(divideCase->GetMostSigOp().GetValue()),lround(divideCase->GetLeastSigOp().GetValue()));
        REQUIRE(goalSet.contains(asTuple));
        goalSet.erase(asTuple);
    }
}

TEST_CASE("linear polynomial", "[factor]")
{
    Oasis::Add add {
        Oasis::Real(30),
        Oasis::Variable("x")
    };
    auto zeros = add.FindZeros();
    REQUIRE(zeros.size() == 1);
    if(zeros.size()==1){
        auto root = Oasis::Divide<Oasis::Real>::Specialize(*zeros[0]);
        REQUIRE(root->GetMostSigOp().GetValue()==-30);
        REQUIRE(root->GetLeastSigOp().GetValue()==1);
    }
}
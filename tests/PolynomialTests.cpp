//
// Created by Matthew McCall on 8/7/23.
//
#include "catch2/catch_test_macros.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Expression.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Variable.hpp"
#include <set>
#include <tuple>
#include <vector>

TEST_CASE("7th degree polynomial with rational roots", "[factor][duplicateRoot]")
{
    // 19891950 - 160023895 x + 475755646 x^2 - 621674764 x^3 + 253625736 x^4 + 167604471 x^5 - 159891300 x^6 + 24097500 x^7
    Oasis::Add add {
        Oasis::Add {
            Oasis::Add {
                Oasis::Real { 19'891'950 },
                Oasis::Multiply {
                    Oasis::Real { -160'023'895 },
                    Oasis::Variable { "x" } },
            },
            Oasis::Add {
                Oasis::Multiply {
                    Oasis::Real { 475'755'646 },
                    Oasis::Exponent {
                        Oasis::Variable { "x" },
                        Oasis::Real { 2 } } },
                Oasis::Multiply {
                    Oasis::Real { -621'674'764 },
                    Oasis::Exponent {
                        Oasis::Variable { "x" },
                        Oasis::Real { 3 } } },
            } },
        Oasis::Add {
            Oasis::Add {
                Oasis::Multiply {
                    Oasis::Real { 253'625'736 },
                    Oasis::Exponent {
                        Oasis::Variable { "x" },
                        Oasis::Real { 4 } } },
                Oasis::Multiply {
                    Oasis::Real { 167'604'471 },
                    Oasis::Exponent {
                        Oasis::Variable { "x" },
                        Oasis::Real { 5 } } },
            },
            Oasis::Add {
                Oasis::Multiply {
                    Oasis::Real { -159'891'300 },
                    Oasis::Exponent {
                        Oasis::Variable { "x" },
                        Oasis::Real { 6 } } },
                Oasis::Multiply {
                    Oasis::Real { 24'097'500 },
                    Oasis::Exponent {
                        Oasis::Variable { "x" },
                        Oasis::Real { 7 } } },
            } }
    };
    auto zeros = add.FindZeros();
    REQUIRE(zeros.size() == 6);
    std::set<std::tuple<long, long>> goalSet = { std::tuple(1, 3), std::tuple(6, 7), std::tuple(13, 17), std::tuple(-5, 3), std::tuple(101, 150), std::tuple(5, 1) };
    for (auto& i : zeros) {
        auto divideCase = Oasis::Divide<Oasis::Real>::Specialize(*i);
        REQUIRE(divideCase != nullptr);
        std::tuple<long, long> asTuple = std::tuple(lround(divideCase->GetMostSigOp().GetValue()), lround(divideCase->GetLeastSigOp().GetValue()));
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
    if (zeros.size() == 1) {
        auto root = Oasis::Divide<Oasis::Real>::Specialize(*zeros[0]);
        REQUIRE(root->GetMostSigOp().GetValue() == -30);
        REQUIRE(root->GetLeastSigOp().GetValue() == 1);
    }
}
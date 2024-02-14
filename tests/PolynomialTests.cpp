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
    Oasis::Add add {
        Oasis::Add {
            Oasis::Add {
                Oasis::Real { 24750 },
                Oasis::Multiply {
                    Oasis::Real { -200925 },
                    Oasis::Variable { "x" } },
            },
            Oasis::Add {
                Oasis::Multiply {
                    Oasis::Real { 573625 },
                    Oasis::Exponent {
                        Oasis::Variable { "x" },
                        Oasis::Real { 2 } } },
                Oasis::Multiply {
                    Oasis::Real { -631406 },
                    Oasis::Exponent {
                        Oasis::Variable { "x" },
                        Oasis::Real { 3 } } },
            } },
        Oasis::Add {
            Oasis::Add {
                Oasis::Multiply {
                    Oasis::Real { 79184 },
                    Oasis::Exponent {
                        Oasis::Variable { "x" },
                        Oasis::Real { 4 } } },
                Oasis::Multiply {
                    Oasis::Real { 247799 },
                    Oasis::Exponent {
                        Oasis::Variable { "x" },
                        Oasis::Real { 5 } } },
            },
            Oasis::Add {
                Oasis::Multiply {
                    Oasis::Real { -92631 },
                    Oasis::Exponent {
                        Oasis::Variable { "x" },
                        Oasis::Real { 6 } } },
                Oasis::Multiply {
                    Oasis::Real { 8820 },
                    Oasis::Exponent {
                        Oasis::Variable { "x" },
                        Oasis::Real { 7 } } },
            } }
    };
    auto zeros = add.FindZeros();
    REQUIRE(zeros.size() == 6);
    std::set<std::tuple<long, long>> goalSet = { std::tuple(1, 3), std::tuple(6, 7), std::tuple(3, 7), std::tuple(-5, 3), std::tuple(11, 20), std::tuple(5, 1) };
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
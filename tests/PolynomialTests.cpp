//
// Created by Matthew McCall on 8/7/23.
//
#include "catch2/catch_test_macros.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Expression.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"
#include <set>
#include <tuple>
#include <vector>

TEST_CASE("7th degree polynomial with rational roots", "[factor][duplicateRoot]")
{
    std::vector<std::unique_ptr<Oasis::Expression>> vec;
    long offset = -3;
    std::vector<long> vecL = { 24750, -200'925, 573'625, -631'406, 79184, 247'799, -92631, 8820 };
    for (size_t i = 0; i < vecL.size(); i++) {
        Oasis::Real num = Oasis::Real(vecL[i]);
        long exp = ((long)i) + offset;
        if (exp < -1) {
            vec.push_back(Oasis::Divide(num, Oasis::Exponent(Oasis::Variable("x"), Oasis::Real(-exp * 1.0))).Copy());
        } else if (exp == -1) {
            vec.push_back(Oasis::Divide(num, Oasis::Variable("x")).Copy());
        } else if (exp == 0) {
            vec.push_back(num.Copy());
        } else if (exp == 1) {
            vec.push_back(Oasis::Multiply(num, Oasis::Variable("x")).Copy());
        } else {
            vec.push_back(Oasis::Multiply(num, Oasis::Exponent(Oasis::Variable("x"), Oasis::Real(exp * 1.0))).Copy());
        }
    }
    auto add = Oasis::BuildFromVector<Oasis::Add>(vec);
    auto zeros = add->FindZeros();
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

TEST_CASE("imaginary linear polynomial")
{
    Oasis::Add add {
        Oasis::Imaginary(),
        Oasis::Variable("x")
    };
    auto zeros = add.FindZeros();
    REQUIRE(zeros.size() == 1);
    if (zeros.size() == 1) {
        std::cout << zeros[0]->ToString();
        auto root = Oasis::Multiply<Oasis::Real, Oasis::Imaginary>::Specialize(*zeros[0]);
        REQUIRE(root != nullptr);
        REQUIRE(root->GetMostSigOp().GetValue() == -1);
    }
}

TEST_CASE("irrational quadratic", "[quadraticFormula]")
{
    std::vector<std::unique_ptr<Oasis::Expression>> vec;
    long offset = -3;
    std::vector<long> vecL = { -1, 1, 1 };
    for (size_t i = 0; i < vecL.size(); i++) {
        Oasis::Real num = Oasis::Real(vecL[i]);
        long exp = ((long)i) + offset;
        if (exp < -1) {
            vec.push_back(Oasis::Divide(num, Oasis::Exponent(Oasis::Variable("x"), Oasis::Real(-exp))).Copy());
        } else if (exp == -1) {
            vec.push_back(Oasis::Divide(num, Oasis::Variable("x")).Copy());
        } else if (exp == 0) {
            vec.push_back(num.Copy());
        } else if (exp == 1) {
            vec.push_back(Oasis::Multiply(num, Oasis::Variable("x")).Copy());
        } else {
            vec.push_back(Oasis::Multiply(num, Oasis::Exponent(Oasis::Variable("x"), Oasis::Real(exp))).Copy());
        }
    }
    auto add = Oasis::BuildFromVector<Oasis::Add>(vec);
    auto zeros = add->FindZeros();
    REQUIRE(zeros.size() == 2);
    auto negOne = Oasis::Real(-1);
    auto two = Oasis::Real(2);
    auto root5 = Oasis::Exponent(Oasis::Real(5), Oasis::Divide(Oasis::Real(1), two));
    std::list<std::unique_ptr<Oasis::Expression>> goalSet = {};
    goalSet.push_back(Oasis::Divide(Oasis::Add(negOne, root5), two).Copy());
    goalSet.push_back(Oasis::Divide(Oasis::Subtract(negOne, root5), two).Copy());
    for (auto& i : zeros) {
        for (auto i2 = goalSet.begin(); i2 != goalSet.end(); i2++) {
            if ((*i2)->Equals(*i)) {
                goalSet.erase(i2);
                break;
            }
        }
    }
    REQUIRE(goalSet.size() == 0);
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
        REQUIRE(root != nullptr);
        REQUIRE(root->GetMostSigOp().GetValue() == -30);
        REQUIRE(root->GetLeastSigOp().GetValue() == 1);
    }
}
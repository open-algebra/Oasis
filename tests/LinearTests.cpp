//
// Created by Andrew Nazareth on 2/16/24.
//

#include "catch2/catch_test_macros.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Linear.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"

#define EPSILON 0.000001

TEST_CASE("Linear Creation", "[Linear]")
{
    Oasis::Add add {

        Oasis::Add {
            Oasis::Multiply {
                Oasis::Real { 4.0 },
                Oasis::Variable { "x" } },
            Oasis::Multiply {
                Oasis::Real { 7.0 },
                Oasis::Variable { "y" } } },
        Oasis::Real { 2.0 }
    };
    Oasis::Add add2 {
        Oasis::Add {
            Oasis::Variable { "y" },
            Oasis::Variable { "z" } },
        Oasis::Real { 3.0 }
    };
    Oasis::Add add3 {
        Oasis::Add {
            Oasis::Multiply {
                Oasis::Real { 3.0 },
                Oasis::Variable { "x" } },
            Oasis::Multiply {
                Oasis::Real { 2.0 },
                Oasis::Variable { "z" } } },
        Oasis::Real { -7.0 }
    };

    std::vector<std::unique_ptr<Oasis::Expression>> exprs;

    exprs.push_back(add.Generalize());
    exprs.push_back(add2.Generalize());
    exprs.push_back(add3.Generalize());

    auto result = Oasis::SolveLinearSystems(exprs);

    REQUIRE(result.find("x") != result.end());
    REQUIRE_THAT(result.find("x")->second, Catch::Matchers::WithinAbs(3.0, EPSILON));
    REQUIRE(result.find("y") != result.end());
    REQUIRE_THAT(result.find("y")->second, Catch::Matchers::WithinAbs(-2.0, EPSILON));
    REQUIRE(result.find("z") != result.end());
    REQUIRE_THAT(result.find("z")->second, Catch::Matchers::WithinAbs(-1.0, EPSILON));
}

TEST_CASE("Linear Solve with subtraction", "[Linear][Subtract]")
{
    Oasis::Add add1{ // 4x - 7y + -5

        Oasis::Subtract {
            Oasis::Multiply {
                Oasis::Real { 4.0 },
                Oasis::Variable { "x" } },
            Oasis::Multiply {
                Oasis::Real { 7.0 },
                Oasis::Variable { "y" } } },
        Oasis::Real { -5.0 }
    };
    Oasis::Add add2 { // y + z + -8
        Oasis::Add {
            Oasis::Variable { "y" },
            Oasis::Variable { "z" } },
        Oasis::Real { -8.0 }
    };
    Oasis::Subtract add3 { // 3x + 2z - 23
        Oasis::Add {
            Oasis::Multiply {
                Oasis::Real { 3.0 },
                Oasis::Variable { "x" } },
            Oasis::Multiply {
                Oasis::Real { 2.0 },
                Oasis::Variable { "z" } } },
        Oasis::Real { 23 }
    };

    std::vector<std::unique_ptr<Oasis::Expression>> exprs;

    exprs.emplace_back(add1.Copy());
    exprs.emplace_back(add2.Copy());
    exprs.emplace_back(add3.Copy());

    auto result = Oasis::SolveLinearSystems(exprs);

    REQUIRE(result.find("x") != result.end());
    REQUIRE_THAT(result.find("x")->second, Catch::Matchers::WithinAbs(3.0, EPSILON));
    REQUIRE(result.find("y") != result.end());
    REQUIRE_THAT(result.find("y")->second, Catch::Matchers::WithinAbs(1.0, EPSILON));
    REQUIRE(result.find("z") != result.end());
    REQUIRE_THAT(result.find("z")->second, Catch::Matchers::WithinAbs(7.0, EPSILON));
}

TEST_CASE("Linear Solve with subtraction 2", "[Linear][Subtract]")
{
    Oasis::Add add1{ // ((4x - (7y + 5)) + z)
            Oasis::Subtract{
                    Oasis::Multiply{
                            Oasis::Real{4.0},
                            Oasis::Variable{"x"}},
                    Oasis::Add{
                            Oasis::Multiply{
                                    Oasis::Real{7.0},
                                    Oasis::Variable{"y"}},
                            Oasis::Real{5.0}
                    }},
            Oasis::Variable{"z"}
    };
    Oasis::Add add2{
            Oasis::Subtract{ // (((y + x) - 8) + z)
                    Oasis::Add{
                            Oasis::Variable{"y"},
                            Oasis::Variable{"x"}},
                    Oasis::Real{8.0}},
            Oasis::Variable{"z"}
    };
    Oasis::Add add3{ // (((3x + 2z) - 23) + 3y)
            Oasis::Subtract{
                    Oasis::Add{
                            Oasis::Multiply{
                                    Oasis::Real{3.0},
                                    Oasis::Variable{"x"}},
                            Oasis::Multiply{
                                    Oasis::Real{2.0},
                                    Oasis::Variable{"z"}}},
                    Oasis::Real{23}},
            Oasis::Multiply{
                    Oasis::Real{3},
                    Oasis::Variable{"y"}
            }
    };

    std::vector<std::unique_ptr<Oasis::Expression>> exprs;
    exprs.emplace_back(add1.Copy());
    exprs.emplace_back(add2.Copy());
    exprs.emplace_back(add3.Copy());

    auto result = Oasis::SolveLinearSystems(exprs);

    REQUIRE(result.find("x") != result.end());
    REQUIRE(result.find("y") != result.end());
    REQUIRE(result.find("z") != result.end());
    REQUIRE_THAT(result.find("x")->second, Catch::Matchers::WithinAbs(4.81818181818, EPSILON));
    REQUIRE_THAT(result.find("y")->second, Catch::Matchers::WithinAbs(2.18181818181, EPSILON));
    REQUIRE_THAT(result.find("z")->second, Catch::Matchers::WithinAbs(1.0, EPSILON));
}
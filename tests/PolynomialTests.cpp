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
#include "Oasis/RecursiveCast.hpp"
#include "Oasis/InFixSerializer.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"
#include "Common.hpp"

#include <cmath>
#include <set>
#include <tuple>
#include <vector>
#include <iostream>

TEST_CASE("linear polynomial test 1: x + 30", "[factor]")
{
    Oasis::Add add {
        Oasis::Variable("x"),
        Oasis::Real(30)
    };
    auto result = add.FindZeros();
    REQUIRE(result.size() == 1);
    REQUIRE(result[0]->Is<Oasis::Real>());

    auto poly_var_res = dynamic_cast<Oasis::Real&>(*result[0]);
    REQUIRE(poly_var_res.GetValue() == -30);
}

TEST_CASE("linear polynomial test 2: 3x - 6", "[factor]")
{
    Oasis::Subtract minus {
        Oasis::Multiply{
            Oasis::Real(3),
            Oasis::Variable("x")
        },
        Oasis::Real(6)
    };
    OASIS_CAPTURE_WITH_SERIALIZER(minus);
    auto result = minus.FindZeros();
    REQUIRE(result.size() == 1);
    REQUIRE(result[0]->Is<Oasis::Real>());

    auto poly_var_res = dynamic_cast<Oasis::Real&>(*result[0]);
    REQUIRE(poly_var_res.GetValue() == 2);
}

TEST_CASE("linear polynomial test 3: 2x + 30", "[factor]")
{
    Oasis::Add add {
        Oasis::Real(30),
        Oasis::Multiply<Oasis::Expression>{
            Oasis::Real(2),
            Oasis::Variable("x")
        }
    };
    auto zeros = add.FindZeros();
    REQUIRE(zeros.size() == 1);
    REQUIRE(zeros[0]->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*zeros[0]);
    REQUIRE(simplifiedReal.GetValue() == -15);
}

TEST_CASE("Quadratic polynomial test 1: x² + 5x + 6", "[factor]")
{
    // x² + 5x + 6
    Oasis::Add<> add {
        Oasis::Real(6),
        Oasis::Multiply<Oasis::Expression>{
            Oasis::Real(5),
            Oasis::Variable("x")
        },
        Oasis::Exponent<Oasis::Variable, Oasis::Real>{
            Oasis::Variable("x"),
            Oasis::Real(2)
        }
    };
    OASIS_CAPTURE_WITH_SERIALIZER(add);

    auto zeros = add.FindZeros();

    REQUIRE(zeros.size() == 2);

    if (zeros.size() == 2) {
        // Check first root (-2)
        auto root1 = Oasis::RecursiveCast<Oasis::Divide<Oasis::Real>>(*zeros[0]);
        REQUIRE(root1 != nullptr);
        REQUIRE(root1->GetMostSigOp().GetValue() == -4);
        REQUIRE(root1->GetLeastSigOp().GetValue() == 2);

        // Check second root (-3)
        auto root2 = Oasis::RecursiveCast<Oasis::Divide<Oasis::Real>>(*zeros[1]);
        REQUIRE(root2 != nullptr);
        REQUIRE(root2->GetMostSigOp().GetValue() == -6);
        REQUIRE(root2->GetLeastSigOp().GetValue() == 2);
    }
}

TEST_CASE("Quadratic polynomial test 2: x² - 2x -3", "[factor]")
{
    // x² - 2x -3
    Oasis::Add<> add{
        Oasis::Exponent<Oasis::Variable, Oasis::Real>{
            Oasis::Variable("x"),
            Oasis::Real(2)
        },
        Oasis::Multiply {
            Oasis::Real(-2),
            Oasis::Variable("x")
        },
        Oasis::Real(-3)
    };
    OASIS_CAPTURE_WITH_SERIALIZER(add);

    auto zeros = add.FindZeros();

    REQUIRE(zeros.size() == 2);

    if (zeros.size() == 2) {
        // Check first root (3)
        auto root1 = Oasis::RecursiveCast<Oasis::Divide<Oasis::Real>>(*zeros[0]);
        REQUIRE(root1 != nullptr);
        REQUIRE(root1->GetMostSigOp().GetValue() == 6);
        REQUIRE(root1->GetLeastSigOp().GetValue() == 2);

        // Check second root (-1)
        auto root2 = Oasis::RecursiveCast<Oasis::Divide<Oasis::Real>>(*zeros[1]);
        REQUIRE(root2 != nullptr);
        REQUIRE(root2->GetMostSigOp().GetValue() == -2);
        REQUIRE(root2->GetLeastSigOp().GetValue() == 2);
    }
}

TEST_CASE("Quadratic polynomial test 3: x² - 9", "[factor]")
{
    Oasis::Subtract minus {
        Oasis::Exponent<Oasis::Variable, Oasis::Real> { // x²
            Oasis::Variable("x"),
            Oasis::Real(2) },
        Oasis::Real(9),
    };
    auto zeros = minus.FindZeros();
    OASIS_CAPTURE_WITH_SERIALIZER(minus);
    REQUIRE(zeros.size() == 2);
    if (zeros.size() == 2) {
        auto root1 = Oasis::RecursiveCast<Oasis::Divide<Oasis::Real>>(*zeros[0]);
        REQUIRE(root1 != nullptr);
        REQUIRE(root1->GetMostSigOp().GetValue() == 3);
        REQUIRE(root1->GetLeastSigOp().GetValue() == 1);
        auto root2 = Oasis::RecursiveCast<Oasis::Divide<Oasis::Real>>(*zeros[1]);
        REQUIRE(root2 != nullptr);
        REQUIRE(root2->GetMostSigOp().GetValue() == -3);
        REQUIRE(root2->GetLeastSigOp().GetValue() == 1);
    }
}

TEST_CASE("Quadratic polynomial test 4: x² - 16", "[factor]")
{
    Oasis::Subtract minus {
        Oasis::Exponent<Oasis::Variable, Oasis::Real> {
            Oasis::Variable("x"),
            Oasis::Real(2) },
        Oasis::Real(16),
    };
    auto zeros = minus.FindZeros();
    OASIS_CAPTURE_WITH_SERIALIZER(minus);
    REQUIRE(zeros.size() == 2);
    if (zeros.size() == 2) {
        auto root1 = Oasis::RecursiveCast<Oasis::Divide<Oasis::Real>>(*zeros[0]);
        REQUIRE(root1 != nullptr);
        REQUIRE(root1->GetMostSigOp().GetValue() == 4);
        REQUIRE(root1->GetLeastSigOp().GetValue() == 1);
        auto root2 = Oasis::RecursiveCast<Oasis::Divide<Oasis::Real>>(*zeros[1]);
        REQUIRE(root2 != nullptr);
        REQUIRE(root2->GetMostSigOp().GetValue() == -4);
        REQUIRE(root2->GetLeastSigOp().GetValue() == 1);
    }
}

TEST_CASE("Quadratic polynomial test 5: x² - 25", "[factor]")
{
    Oasis::Subtract minus {
        Oasis::Exponent<Oasis::Variable, Oasis::Real> { // x²
            Oasis::Variable("x"),
            Oasis::Real(2) },
        Oasis::Real(25),
    };
    auto zeros = minus.FindZeros();
    OASIS_CAPTURE_WITH_SERIALIZER(minus);
    REQUIRE(zeros.size() == 2);
    if (zeros.size() == 2) {
        auto root1 = Oasis::RecursiveCast<Oasis::Divide<Oasis::Real>>(*zeros[0]);
        REQUIRE(root1 != nullptr);
        REQUIRE(root1->GetMostSigOp().GetValue() == 5);
        REQUIRE(root1->GetLeastSigOp().GetValue() == 1);
        auto root2 = Oasis::RecursiveCast<Oasis::Divide<Oasis::Real>>(*zeros[1]);
        REQUIRE(root2 != nullptr);
        REQUIRE(root2->GetMostSigOp().GetValue() == -5);
        REQUIRE(root2->GetLeastSigOp().GetValue() == 1);
    }
}

TEST_CASE("Rational Quadratic polynomial test 1: 2x² + x - 1", "[factor]")
{
    // 2x² + x - 1
    Oasis::Add<> add{
        Oasis::Multiply{
            Oasis::Real(2),
            Oasis::Exponent<Oasis::Variable, Oasis::Real>{
                        Oasis::Variable("x"),
                        Oasis::Real(2)
            }
        },
        Oasis::Variable("x"),
        Oasis::Real(-1)
    };
    OASIS_CAPTURE_WITH_SERIALIZER(add);

    auto zeros = add.FindZeros();

    REQUIRE(zeros.size() == 2);

    if (zeros.size() == 2) {
        // Check first root (1/2)
        auto root1 = Oasis::RecursiveCast<Oasis::Divide<Oasis::Real>>(*zeros[0]);
        auto denominator1 = root1->GetLeastSigOp().GetValue();
        auto numerator1 = root1->GetMostSigOp().GetValue();
        REQUIRE(root1 != nullptr);
        REQUIRE(numerator1/denominator1 == 1.0/2.0);


        // Check second root (-1)
        auto root2 = Oasis::RecursiveCast<Oasis::Divide<Oasis::Real>>(*zeros[1]);
        auto denominator2 = root2->GetLeastSigOp().GetValue();
        auto numerator2 = root2->GetMostSigOp().GetValue();
        REQUIRE(root2 != nullptr);
        REQUIRE(numerator2/denominator2 == -1);

    }
}

TEST_CASE("Rational Quadratic polynomial test 2: 6x² - 5x + 1", "[factor]")
{
    // 6x² - 5x + 1
    Oasis::Add<> add{
        Oasis::Multiply{
            Oasis::Real(6),
            Oasis::Exponent<Oasis::Variable, Oasis::Real>{
                Oasis::Variable("x"),
                Oasis::Real(2)
            }
        },
        Oasis::Multiply{
                Oasis::Real(-5),
                Oasis::Variable("x")
        },
        Oasis::Real(1)
    };
    OASIS_CAPTURE_WITH_SERIALIZER(add);

    auto zeros = add.FindZeros();

    REQUIRE(zeros.size() == 2);

    if (zeros.size() == 2) {

        auto root1 = Oasis::RecursiveCast<Oasis::Divide<Oasis::Real>>(*zeros[0]);
        auto denominator = root1->GetLeastSigOp().GetValue();
        auto numerator = root1->GetMostSigOp().GetValue();
        REQUIRE(root1 != nullptr);
        REQUIRE(numerator/denominator == 1.0/2.0);

        auto root2 = Oasis::RecursiveCast<Oasis::Divide<Oasis::Real>>(*zeros[1]);
        auto denominator2 = root2->GetLeastSigOp().GetValue();
        auto numerator2 = root2->GetMostSigOp().GetValue();
        REQUIRE(root2 != nullptr);
        REQUIRE(numerator2/denominator2 == 1.0/3.0);
    }
}


TEST_CASE("Cubic polynomial test 1: 3x³ - 16x² + 23x - 6:", "[factor]")
{
    // 3x³ - 16x² + 23x - 6:
    Oasis::Add<> cubic{
        Oasis::Multiply{          // 3x³ term
            Oasis::Real(3),
            Oasis::Exponent<Oasis::Variable, Oasis::Real>{
                Oasis::Variable("x"),
                Oasis::Real(3)
            }
        },
        Oasis::Multiply{          // -16x² term
            Oasis::Real(-16),
            Oasis::Exponent<Oasis::Variable, Oasis::Real>{
                Oasis::Variable("x"),
                Oasis::Real(2)
            }
        },
        Oasis::Multiply{          // 23x term
            Oasis::Real(23),
            Oasis::Variable("x")
        },
        Oasis::Real(-6)          // -6 term
    };
    OASIS_CAPTURE_WITH_SERIALIZER(cubic);

    auto zeros = cubic.FindZeros();

    REQUIRE(zeros.size() == 3);

    if (zeros.size() == 3) {
        auto root1 = Oasis::RecursiveCast<Oasis::Divide<Oasis::Real>>(*zeros[0]);
        auto denominator = root1->GetLeastSigOp().GetValue();
        auto numerator = root1->GetMostSigOp().GetValue();
        REQUIRE(root1 != nullptr);
        REQUIRE(numerator/denominator == 1.0/2.0);

        auto root2 = Oasis::RecursiveCast<Oasis::Divide<Oasis::Real>>(*zeros[1]);
        auto denominator2 = root2->GetLeastSigOp().GetValue();
        auto numerator2 = root2->GetMostSigOp().GetValue();
        REQUIRE(root2 != nullptr);
        REQUIRE(numerator2/denominator2 == 1.0/3.0);

        auto root3 = Oasis::RecursiveCast<Oasis::Divide<Oasis::Real>>(*zeros[2]);
        auto denominator3 = root3->GetLeastSigOp().GetValue();
        auto numerator3 = root3->GetMostSigOp().GetValue();
        REQUIRE(root2 != nullptr);
        REQUIRE(numerator3/denominator3 == 3.0/1.0);
    }
}

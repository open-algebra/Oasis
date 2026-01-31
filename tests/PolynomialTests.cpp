//
// Created by Matthew McCall on 8/7/23.
//
#include "catch2/catch_test_macros.hpp"

#include "Common.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Expression.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/RecursiveCast.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"

#include <set>
#include <vector>

TEST_CASE("linear polynomial test 1: x + 30", "[factor]")
{
    Oasis::Add add {
        Oasis::Variable("x"),
        Oasis::Real(30)
    };
    auto result_wrapped = add.FindZeros();
    REQUIRE(result_wrapped.has_value());

    auto result = std::move(result_wrapped.value());

    REQUIRE(result.size() == 1);
    REQUIRE(result[0]->Is<Oasis::Real>());

    auto poly_var_res = dynamic_cast<Oasis::Real&>(*result[0]);
    REQUIRE(poly_var_res.GetValue() == -30);
}

TEST_CASE("Variable check test 1: x + y + 30", "[factor]")
{
    Oasis::Add<> add {
        Oasis::Variable("x"),
        Oasis::Variable("y"),
        Oasis::Real(30)
    };
    auto result = add.FindZeros();
    REQUIRE((!result.has_value()));
}

TEST_CASE("linear polynomial test 2: 3x - 6", "[factor]")
{
    Oasis::Subtract minus {
        Oasis::Multiply {
            Oasis::Real(3),
            Oasis::Variable("x") },
        Oasis::Real(6)
    };
    OASIS_CAPTURE_WITH_SERIALIZER(minus);
    auto result_wrapped = minus.FindZeros();
    REQUIRE(result_wrapped.has_value());

    auto result = std::move(result_wrapped.value());
    REQUIRE(result.size() == 1);
    REQUIRE(result[0]->Is<Oasis::Real>());

    auto poly_var_res = dynamic_cast<Oasis::Real&>(*result[0]);
    REQUIRE(poly_var_res.GetValue() == 2);
}

TEST_CASE("linear polynomial test 3: 2x + 30", "[factor]")
{
    Oasis::Add add {
        Oasis::Real(30),
        Oasis::Multiply<Oasis::Expression> {
            Oasis::Real(2),
            Oasis::Variable("x") }
    };
    auto result_wrapped = add.FindZeros();
    REQUIRE(result_wrapped.has_value());

    auto result = std::move(result_wrapped.value());
    REQUIRE(result.size() == 1);
    REQUIRE(result[0]->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*result[0]);
    REQUIRE(simplifiedReal.GetValue() == -15);
}

TEST_CASE("Quadratic polynomial test 1: x^2 + 5x + 6", "[factor]")
{
    // x^2 + 5x + 6
    Oasis::Add<> add {
        Oasis::Real(6),
        Oasis::Multiply<Oasis::Expression> {
            Oasis::Real(5),
            Oasis::Variable("x") },
        Oasis::Exponent<Oasis::Variable, Oasis::Real> {
            Oasis::Variable("x"),
            Oasis::Real(2) }
    };
    OASIS_CAPTURE_WITH_SERIALIZER(add);

    auto result_wrapped = add.FindZeros();
    REQUIRE(result_wrapped.has_value());

    auto zeros = std::move(result_wrapped.value());

    REQUIRE(zeros.size() == 2);
    REQUIRE(print_expr(*zeros[0]) == "(-2/1)");
    REQUIRE(print_expr(*zeros[1]) == "(-3/1)");

}

TEST_CASE("Quadratic polynomial test 2: x^2 - 2x -3", "[factor]")
{
    // x^2 - 2x -3
    Oasis::Add<> add {
        Oasis::Exponent<Oasis::Variable, Oasis::Real> {
            Oasis::Variable("x"),
            Oasis::Real(2) },
        Oasis::Multiply {
            Oasis::Real(-2),
            Oasis::Variable("x") },
        Oasis::Real(-3)
    };
    OASIS_CAPTURE_WITH_SERIALIZER(add);

    auto result_wrapped = add.FindZeros();
    REQUIRE(result_wrapped.has_value());

    auto zeros = std::move(result_wrapped.value());

    REQUIRE(zeros.size() == 2);
    REQUIRE(print_expr(*zeros[0]) == "(3/1)");
    REQUIRE(print_expr(*zeros[1]) == "(-1/1)");
}

TEST_CASE("Quadratic polynomial test 2: x^2 + 8x -14", "[factor]")
{
    // x^2 + 8x -14
    Oasis::Add<> add {
        Oasis::Exponent<Oasis::Variable, Oasis::Real> {
            Oasis::Variable("x"),
            Oasis::Real(2) },
        Oasis::Multiply {
            Oasis::Real(8),
            Oasis::Variable("x") },
        Oasis::Real(-14)
    };
    OASIS_CAPTURE_WITH_SERIALIZER(add);

    auto result_wrapped = add.FindZeros();
    REQUIRE(result_wrapped.has_value());

    auto zeros = std::move(result_wrapped.value());

    REQUIRE(zeros.size() == 2);
}

TEST_CASE("Quadratic polynomial test 3: x^2 - 9", "[factor]")
{
    Oasis::Subtract minus {
        Oasis::Exponent<Oasis::Variable, Oasis::Real> { // x^2
            Oasis::Variable("x"),
            Oasis::Real(2) },
        Oasis::Real(9),
    };
    auto result_wrapped = minus.FindZeros();
    REQUIRE(result_wrapped.has_value());

    auto zeros = std::move(result_wrapped.value());
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

TEST_CASE("Quadratic polynomial test 4: x^2 - 16", "[factor]")
{
    Oasis::Subtract minus {
        Oasis::Exponent<Oasis::Variable, Oasis::Real> {
            Oasis::Variable("x"),
            Oasis::Real(2) },
        Oasis::Real(16),
    };
    auto result_wrapped = minus.FindZeros();
    REQUIRE(result_wrapped.has_value());

    auto zeros = std::move(result_wrapped.value());
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

TEST_CASE("Quadratic polynomial test 5: x^2 - 25", "[factor]")
{
    Oasis::Subtract minus {
        Oasis::Exponent<Oasis::Variable, Oasis::Real> { // x^2
            Oasis::Variable("x"),
            Oasis::Real(2) },
        Oasis::Real(25),
    };
    auto result_wrapped = minus.FindZeros();
    REQUIRE(result_wrapped.has_value());

    auto zeros = std::move(result_wrapped.value());
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

TEST_CASE("Rational Quadratic polynomial test 1: 2x^2 + x - 1", "[factor]")
{
    // 2x^2 + x - 1
    Oasis::Add<> add {
        Oasis::Multiply {
            Oasis::Real(2),
            Oasis::Exponent<Oasis::Variable, Oasis::Real> {
                Oasis::Variable("x"),
                Oasis::Real(2) } },
        Oasis::Variable("x"),
        Oasis::Real(-1)
    };
    OASIS_CAPTURE_WITH_SERIALIZER(add);

    auto result_wrapped = add.FindZeros();
    REQUIRE(result_wrapped.has_value());

    auto zeros = std::move(result_wrapped.value());

    REQUIRE(zeros.size() == 2);

    if (zeros.size() == 2) {
        // Check first root (1/2)
        auto root1 = Oasis::RecursiveCast<Oasis::Divide<Oasis::Real>>(*zeros[0]);
        auto denominator1 = root1->GetLeastSigOp().GetValue();
        auto numerator1 = root1->GetMostSigOp().GetValue();
        REQUIRE(root1 != nullptr);
        REQUIRE(numerator1 / denominator1 == 1.0 / 2.0);

        // Check second root (-1)
        auto root2 = Oasis::RecursiveCast<Oasis::Divide<Oasis::Real>>(*zeros[1]);
        auto denominator2 = root2->GetLeastSigOp().GetValue();
        auto numerator2 = root2->GetMostSigOp().GetValue();
        REQUIRE(root2 != nullptr);
        REQUIRE(numerator2 / denominator2 == -1);
    }
}

TEST_CASE("Rational Quadratic polynomial test 2: 6x^2 - 5x + 1", "[factor]")
{
    // 6x^2 - 5x + 1
    Oasis::Add<> add {
        Oasis::Multiply {
            Oasis::Real(6),
            Oasis::Exponent<Oasis::Variable, Oasis::Real> {
                Oasis::Variable("x"),
                Oasis::Real(2) } },
        Oasis::Multiply {
            Oasis::Real(-5),
            Oasis::Variable("x") },
        Oasis::Real(1)
    };
    OASIS_CAPTURE_WITH_SERIALIZER(add);

    auto result_wrapped = add.FindZeros();
    REQUIRE(result_wrapped.has_value());

    auto zeros = std::move(result_wrapped.value());

    REQUIRE(zeros.size() == 2);

    if (zeros.size() == 2) {

        auto root1 = Oasis::RecursiveCast<Oasis::Divide<Oasis::Real>>(*zeros[0]);
        auto denominator = root1->GetLeastSigOp().GetValue();
        auto numerator = root1->GetMostSigOp().GetValue();
        REQUIRE(root1 != nullptr);
        REQUIRE(numerator / denominator == 1.0 / 2.0);

        auto root2 = Oasis::RecursiveCast<Oasis::Divide<Oasis::Real>>(*zeros[1]);
        auto denominator2 = root2->GetLeastSigOp().GetValue();
        auto numerator2 = root2->GetMostSigOp().GetValue();
        REQUIRE(root2 != nullptr);
        REQUIRE(numerator2 / denominator2 == 1.0 / 3.0);
    }
}

TEST_CASE("Irrational Quadratic test 3: x^2 - 15x + 4", "[factor]") // roots of 15/2 +- sqrt(209) / 2);
{
    Oasis::Add<> add {
        Oasis::Exponent<Oasis::Variable, Oasis::Real> {
            Oasis::Variable("x"),
            Oasis::Real(2) },
        Oasis::Multiply {
            Oasis::Real(-15),
            Oasis::Variable("x") },
        Oasis::Real(4)
    };

    OASIS_CAPTURE_WITH_SERIALIZER(add);
    auto result_wrapped = add.FindZeros();
    REQUIRE(result_wrapped.has_value());

    auto zeros = std::move(result_wrapped.value());
    REQUIRE(zeros.size() == 2);
    REQUIRE(print_expr(*zeros[0]) == "(((-1*-15)/(2*1))+((209^(1/2))/(2*1)))");
    REQUIRE(print_expr(*zeros[1]) == "(((-1*-15)/(2*1))-((209^(1/2))/(2*1)))");
}

TEST_CASE("Cubic polynomial test 1: 3x^3 - 16x^2 + 23x - 6:", "[factor]")
{
    // 3x^3 - 16x^2 + 23x - 6:
    Oasis::Add<> cubic {
        Oasis::Multiply { // 3x^3 term
            Oasis::Real(3),
            Oasis::Exponent<Oasis::Variable, Oasis::Real> {
                Oasis::Variable("x"),
                Oasis::Real(3) } },
        Oasis::Multiply { // -16x^2 term
            Oasis::Real(-16),
            Oasis::Exponent<Oasis::Variable, Oasis::Real> {
                Oasis::Variable("x"),
                Oasis::Real(2) } },
        Oasis::Multiply { // 23x term
            Oasis::Real(23),
            Oasis::Variable("x") },
        Oasis::Real(-6) // -6 term
    };
    OASIS_CAPTURE_WITH_SERIALIZER(cubic);

    auto result_wrapped = cubic.FindZeros();
    REQUIRE(result_wrapped.has_value());

    auto zeros = std::move(result_wrapped.value());

    REQUIRE(zeros.size() == 3);
    REQUIRE(print_expr(*zeros[0]) == "(1/3)");
    REQUIRE(print_expr(*zeros[1]) == "(2/1)");
    REQUIRE(print_expr(*zeros[2]) == "(3/1)");
}

TEST_CASE("Quartic test 1: x^4 - 1", "[factor]")
{
    Oasis::Add<> quartic {
        Oasis::Exponent<Oasis::Variable, Oasis::Real> {
            Oasis::Variable("x"),
            Oasis::Real(4) },
        Oasis::Real(-1)
    };

    OASIS_CAPTURE_WITH_SERIALIZER(quartic);
    auto result_wrapped = quartic.FindZeros();
    REQUIRE(result_wrapped.has_value());

    auto zeros = std::move(result_wrapped.value());

    REQUIRE(zeros.size() == 2);
}

TEST_CASE("Quartic test 2: x^4 - 5x^2 + 4", "[factor]") // with 4 roots
{
    Oasis::Add<> quartic {
        Oasis::Exponent<Oasis::Variable, Oasis::Real> {
            Oasis::Variable("x"),
            Oasis::Real(4) },
        Oasis::Multiply { // -5x^2 term
            Oasis::Real(-5),
            Oasis::Exponent<Oasis::Variable, Oasis::Real> {
                Oasis::Variable("x"),
                Oasis::Real(2) } },
        Oasis::Real(4)
    };

    OASIS_CAPTURE_WITH_SERIALIZER(quartic);
    auto result_wrapped = quartic.FindZeros();
    REQUIRE(result_wrapped.has_value());

    auto zeros = std::move(result_wrapped.value());

    REQUIRE(zeros.size() == 8);
    if (zeros.size() == 8) {
        auto root1 = Oasis::RecursiveCast<Oasis::Divide<Oasis::Real>>(*zeros[0]);
        auto denominator = root1->GetLeastSigOp().GetValue();
        auto numerator = root1->GetMostSigOp().GetValue();
        REQUIRE(root1 != nullptr);
        REQUIRE(numerator / denominator == 1.0 / 1.0);

        auto root2 = Oasis::RecursiveCast<Oasis::Divide<Oasis::Real>>(*zeros[1]);
        auto denominator2 = root2->GetLeastSigOp().GetValue();
        auto numerator2 = root2->GetMostSigOp().GetValue();
        REQUIRE(root2 != nullptr);
        REQUIRE(numerator2 / denominator2 == -1.0 / 1.0);

        auto root3 = Oasis::RecursiveCast<Oasis::Divide<Oasis::Real>>(*zeros[2]);
        auto denominator3 = root3->GetLeastSigOp().GetValue();
        auto numerator3 = root3->GetMostSigOp().GetValue();
        REQUIRE(root3 != nullptr);
        REQUIRE(numerator3 / denominator3 == 2.0 / 1.0);

        auto root4 = Oasis::RecursiveCast<Oasis::Divide<Oasis::Real>>(*zeros[3]);
        auto denominator4 = root4->GetLeastSigOp().GetValue();
        auto numerator4 = root4->GetMostSigOp().GetValue();
        REQUIRE(root4 != nullptr);
        REQUIRE(numerator4 / denominator4 == -2.0 / 1.0);
    }
}

TEST_CASE("Irrational Cubic polynomial test 1: 4x^3 - 11x^2 + 2x + 3:", "[factor]")
{
    // 4x^3 - 11x^2 + 2x + 3:
    Oasis::Add<> cubic {
        Oasis::Multiply {
            Oasis::Real(4),
            Oasis::Exponent<Oasis::Variable, Oasis::Real> {
                Oasis::Variable("x"),
                Oasis::Real(3) } },
        Oasis::Multiply {
            Oasis::Real(-11),
            Oasis::Exponent<Oasis::Variable, Oasis::Real> {
                Oasis::Variable("x"),
                Oasis::Real(2) } },
        Oasis::Multiply {
            Oasis::Real(2),
            Oasis::Variable("x") },
        Oasis::Real(3)
    };
    OASIS_CAPTURE_WITH_SERIALIZER(cubic);

    auto result_wrapped = cubic.FindZeros();
    REQUIRE(result_wrapped.has_value());

    auto zeros = std::move(result_wrapped.value());

    REQUIRE(zeros.size() == 3);
}
//
// Created by Matthew McCall on 4/12/24.
//

#include "../src/Parsers/SExpression/SExpression.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("S-Expression Parsing Works for Simple Trees", "[Sexp]")
{
    Oasis::t3::Sexp simpleAdd { "(+ 1 2)" };

    REQUIRE(simpleAdd.label == "+");

    const auto op = simpleAdd.operands.cbegin();
    REQUIRE(op->label == "1");

    const auto op2 = std::next(op);
    REQUIRE(op2->label == "2");

    const auto end = std::next(op2);
    REQUIRE(end == simpleAdd.operands.cend());
}

TEST_CASE("S-Expression Parsing Works", "[Sexp]")
{
    Oasis::t3::Sexp add { "(+ (- 1 (* 2 4)) 2)" };

    REQUIRE(add.label == "+");

    const auto addOp = add.operands.cbegin();

    const Oasis::t3::Sexp& minus = *addOp;
    REQUIRE(minus.label == "-");

    const auto minusOp = minus.operands.cbegin();
    REQUIRE(minusOp->label == "1");

    const auto minusOp2 = std::next(minusOp);
    const Oasis::t3::Sexp& multiply = *minusOp2;
    REQUIRE(multiply.label == "*");

    const auto multiplyOp = multiply.operands.cbegin();
    REQUIRE(multiplyOp->label == "2");

    const auto multiplyOp2 = std::next(multiplyOp);
    REQUIRE(multiplyOp2->label == "4");

    const auto multiplyEnd = std::next(multiplyOp2);
    REQUIRE(multiplyEnd == multiply.operands.end());

    const auto minusEnd = std::next(minusOp2);
    REQUIRE(minusEnd == minus.operands.cend());

    const auto addOp2 = std::next(addOp);
    REQUIRE(addOp2->label == "2");

    const auto addEnd = std::next(addOp2);
    REQUIRE(addEnd == add.operands.cend());
}
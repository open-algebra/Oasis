#include "../../tests/Common.hpp"

#include "Oasis/RecursiveCast.hpp"
#include "catch2/catch_test_macros.hpp"

#include "Oasis/FromPALM.hpp"
#include "Oasis/PALMSerializer.hpp"

#include "../src/PALMTypes.hpp"

#include "Oasis/Undefined.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Derivative.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/EulerNumber.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Magnitude.hpp"
#include "Oasis/Matrix.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Negate.hpp"
#include "Oasis/Pi.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"

// ========================
// PALM Serialization Tests
// ========================

// --------------
// Operator Tests
// --------------

// --- Undefined ---
TEST_CASE("PALM Serialization of Undefined", "[PALM][Serializer][Operator][Undefined]")
{
    const Oasis::Undefined undefined {};

    Oasis::PALMSerializer serializer {};

    auto result = undefined.Accept(serializer).value();
    std::string expected = "( undefined )";

    REQUIRE(expected == result);
}

// --- Real ---
// Regular Values
TEST_CASE("PALM Serialization of Real", "[PALM][Serializer][Operator][Real]")
{
    const Oasis::Real real { 42.0 };

    Oasis::PALMSerializer serializer {};

    auto result = real.Accept(serializer).value();
    std::string expected = "( real 42 )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Real with decimal", "[PALM][Serializer][Operator][Real]")
{
    const Oasis::Real real { 3.14 };

    Oasis::PALMSerializer serializer {};

    auto result = real.Accept(serializer).value();
    std::string expected = "( real 3.14 )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Real with negative value", "[PALM][Serializer][Operator][Real]")
{
    const Oasis::Real real { -2.71828 };

    Oasis::PALMSerializer serializer {};

    auto result = real.Accept(serializer).value();
    std::string expected = "( real -2.71828 )";

    REQUIRE(expected == result);
}

// Precision Tests
TEST_CASE("PALM Serialization of Real with default precision", "[PALM][Serializer][Operator][Real]")
{
    const Oasis::Real real { 2.718281828459045 };

    Oasis::PALMSerializer serializer {};

    auto result = real.Accept(serializer).value();
    std::string expected = "( real 2.71828 )"; // Default precision is 5 decimal places

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Real to 2 decimal places", "[PALM][Serializer][Operator][Real]")
{
    const Oasis::Real real { 2.718281828459045 };

    Oasis::PALMSerializer serializer { { .numPlaces = 2 } };

    auto result = real.Accept(serializer).value();
    std::string expected = "( real 2.72 )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Real to 0 decimal places", "[PALM][Serializer][Operator][Real]")
{
    const Oasis::Real real { 2.718281828459045 };

    Oasis::PALMSerializer serializer { { .numPlaces = 0 } };

    auto result = real.Accept(serializer).value();
    std::string expected = "( real 3 )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Real with negative value and 3 decimal places", "[PALM][Serializer][Operator][Real]")
{
    const Oasis::Real real { -2.71828 };

    Oasis::PALMSerializer serializer { { .numPlaces = 3 } };

    auto result = real.Accept(serializer).value();
    std::string expected = "( real -2.718 )";

    REQUIRE(expected == result);
}

// --- Imaginary ---
// Default Values
TEST_CASE("PALM Serialization of Imaginary", "[PALM][Serializer][Operator][Imaginary]")
{
    const Oasis::Imaginary imaginary {};

    Oasis::PALMSerializer serializer {};

    auto result = imaginary.Accept(serializer).value();
    std::string expected = "( i )";

    REQUIRE(expected == result);
}

// Different Symbols
TEST_CASE("PALM Serialization of Imaginary with 'i' symbol", "[PALM][Serializer][Operator][Imaginary]")
{
    const Oasis::Imaginary imaginary {};

    Oasis::PALMSerializer serializer { { .imaginarySymbol = Oasis::PALMSerializationOpts::ImgSymType::I } };

    auto result = imaginary.Accept(serializer).value();
    std::string expected = "( i )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Imaginary with 'j' symbol", "[PALM][Serializer][Operator][Imaginary]")
{
    const Oasis::Imaginary imaginary {};

    Oasis::PALMSerializer serializer { { .imaginarySymbol = Oasis::PALMSerializationOpts::ImgSymType::J } };

    auto result = imaginary.Accept(serializer).value();
    std::string expected = "( j )";

    REQUIRE(expected == result);
}

// --- Variable ---
// Accepted Identifiers
TEST_CASE("PALM Serialization of Variable with Single Character", "[PALM][Serializer][Operator][Variable]")
{
    const Oasis::Variable variable { "x" };

    Oasis::PALMSerializer serializer {};

    auto result = variable.Accept(serializer).value();
    std::string expected = "( var x )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Variable with Multiple Characters", "[PALM][Serializer][Operator][Variable]")
{
    const Oasis::Variable variable { "velocity" };

    Oasis::PALMSerializer serializer {};

    auto result = variable.Accept(serializer).value();
    std::string expected = "( var velocity )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Variable with Underscore", "[PALM][Serializer][Operator][Variable]")
{
    const Oasis::Variable variable { "x_variable" };

    Oasis::PALMSerializer serializer {};

    auto result = variable.Accept(serializer).value();
    std::string expected = "( var x_variable )";

    REQUIRE(expected == result);
}

// Rejected Identifiers
TEST_CASE("PALM Serialization of Variable with Invalid Identifier", "[PALM][Serializer][Operator][Variable]")
{
    const Oasis::Variable variable { "123invalid" };

    Oasis::PALMSerializer serializer {};

    auto error = variable.Accept(serializer).value().error();
    auto expectedError = Oasis::PALMSerializationError {
        .type = Oasis::PALMSerializationError::PALMSerializationErrorType::InvalidIdentifier,
        .expression = &variable,
        .message = "Invalid identifier: 123invalid"
    };

    REQUIRE(expectedError == error);
}

TEST_CASE("PALM Serialization of Variable with Special Characters", "[PALM][Serializer][Operator][Variable]")
{
    const Oasis::Variable variable { "var$!" };

    Oasis::PALMSerializer serializer {};

    auto error = variable.Accept(serializer).value().error();
    auto expectedError = Oasis::PALMSerializationError {
        .type = Oasis::PALMSerializationError::PALMSerializationErrorType::InvalidIdentifier,
        .expression = &variable,
        .message = "Invalid identifier: var$!"
    };

    REQUIRE(expectedError == error);
}

TEST_CASE("PALM Serialization of Variable with Space", "[PALM][Serializer][Operator][Variable]")
{
    const Oasis::Variable variable { "my var" };

    Oasis::PALMSerializer serializer {};

    auto error = variable.Accept(serializer).value().error();
    auto expectedError = Oasis::PALMSerializationError {
        .type = Oasis::PALMSerializationError::PALMSerializationErrorType::InvalidIdentifier,
        .expression = &variable,
        .message = "Invalid identifier: my var"
    };

    REQUIRE(expectedError == error);
}

TEST_CASE("PALM Serialization of Keyword as Variable", "[PALM][Serializer][Operator][Variable]")
{
    const Oasis::Variable variable { "real" };

    Oasis::PALMSerializer serializer {};

    auto error = variable.Accept(serializer).value().error();
    auto expectedError = Oasis::PALMSerializationError {
        .type = Oasis::PALMSerializationError::PALMSerializationErrorType::InvalidIdentifier,
        .expression = &variable,
        .message = "Invalid identifier: real"
    };

    REQUIRE(expectedError == error);
}

// --- Addition ---
// Basic Tests
TEST_CASE("PALM Serialization of Addition Operator Symbol", "[PALM][Serializer][Operator][Addition]")
{
    const Oasis::Add<> addition {
        Oasis::Real { 1.0 },
        Oasis::Real { 2.0 }
    };

    Oasis::PALMSerializer serializer {};

    auto result = addition.Accept(serializer).value();
    std::string expected = "( + ( real 1 ) ( real 2 ) )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Nested Addition", "[PALM][Serializer][Addition]")
{
    const Oasis::Add<> addition {
        Oasis::Real { 5.0 },
        Oasis::Add {
            Oasis::Real { 3.0 },
            Oasis::Real { 2.0 } }
    };

    Oasis::PALMSerializer serializer {};

    auto result = addition.Accept(serializer).value();
    std::string expected = "( + ( real 5 ) ( + ( real 3 ) ( real 2 ) ) )";

    REQUIRE(expected == result);
}

// Malformed Tests
TEST_CASE("PALM Serialization of Addition missing Least Significant Operand", "[PALM][Serializer][Addition]")
{
    Oasis::Add<Oasis::Real, Oasis::Expression> addition;
    addition.SetMostSigOp(Oasis::Real { 5.0 });

    Oasis::PALMSerializer serializer = Oasis::PALMSerializer();

    auto error = addition.Accept(serializer).value().error();
    auto expectedError = Oasis::PALMSerializationError {
        .type = Oasis::PALMSerializationError::PALMSerializationErrorType::MissingOperand,
        .expression = &addition,
        .message = "Expression missing least significant operand"
    };

    REQUIRE(expectedError == error);
}

TEST_CASE("PALM Serialization of Addition missing Most Significant Operand", "[PALM][Serializer][Addition]")
{ // Missing most significant operand
    Oasis::Add<Oasis::Expression, Oasis::Real> addition;
    addition.SetLeastSigOp(Oasis::Real { 3.0 });

    Oasis::PALMSerializer serializer = Oasis::PALMSerializer();

    auto result = addition.Accept(serializer);

    REQUIRE(!result.has_value());
    REQUIRE(result.error() == "Expression missing most significant operand");
}








TEST_CASE("PALM Serialization of Subtraction", "[PALM][Serializer][Subtraction]")
{
    const Oasis::Subtract<> subtraction {
        Oasis::Real { 5.0 },
        Oasis::Real { 3.0 }
    };

    Oasis::PALMSerializer serializer {};

    auto result = subtraction.Accept(serializer).value();
    std::string expected = "( - ( real 5 ) ( real 3 ) )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Nested Subtraction", "[PALM][Serializer][Subtraction]")
{
    const Oasis::Subtract<> subtraction {
        Oasis::Real { 5.0 },
        Oasis::Subtract {
            Oasis::Real { 3.0 },
            Oasis::Real { 2.0 } }
    };

    Oasis::PALMSerializer serializer {};

    auto result = subtraction.Accept(serializer).value();
    std::string expected = "( - ( real 5 ) ( - ( real 3 ) ( real 2 ) ) )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Malformed Subtraction", "[PALM][Serializer][Subtraction]")
{
    { // Missing least significant operand
        Oasis::Subtract<Oasis::Real, Oasis::Expression> subtraction;
        subtraction.SetMostSigOp(Oasis::Real { 5.0 });

        Oasis::PALMSerializer serializer = Oasis::PALMSerializer();

        auto result = subtraction.Accept(serializer);

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Expression missing least significant operand");
    }

    { // Missing most significant operand
        Oasis::Subtract<Oasis::Expression, Oasis::Real> subtraction;
        subtraction.SetLeastSigOp(Oasis::Real { 3.0 });

        Oasis::PALMSerializer serializer = Oasis::PALMSerializer();

        auto result = subtraction.Accept(serializer);

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Expression missing most significant operand");
    }
}

TEST_CASE("PALM Serialization of Multiplication", "[PALM][Serializer][Multiplication]")
{
    const Oasis::Multiply<> multiplication {
        Oasis::Real { 5.0 },
        Oasis::Real { 3.0 }
    };

    Oasis::PALMSerializer serializer {};

    auto result = multiplication.Accept(serializer).value();
    std::string expected = "( * ( real 5 ) ( real 3 ) )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Nested Multiplication", "[PALM][Serializer][Multiplication]")
{
    const Oasis::Multiply<> multiplication {
        Oasis::Real { 5.0 },
        Oasis::Multiply {
            Oasis::Real { 3.0 },
            Oasis::Real { 2.0 } }
    };

    Oasis::PALMSerializer serializer {};

    auto result = multiplication.Accept(serializer).value();
    std::string expected = "( * ( real 5 ) ( * ( real 3 ) ( real 2 ) ) )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Malformed Multiplication", "[PALM][Serializer][Multiplication]")
{
    { // Missing least significant operand
        Oasis::Multiply<Oasis::Real, Oasis::Expression> multiplication;
        multiplication.SetMostSigOp(Oasis::Real { 5.0 });

        Oasis::PALMSerializer serializer = Oasis::PALMSerializer();

        auto result = multiplication.Accept(serializer);

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Expression missing least significant operand");
    }

    { // Missing most significant operand
        Oasis::Multiply<Oasis::Expression, Oasis::Real> multiplication;
        multiplication.SetLeastSigOp(Oasis::Real { 3.0 });

        Oasis::PALMSerializer serializer = Oasis::PALMSerializer();

        auto result = multiplication.Accept(serializer);

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Expression missing most significant operand");
    }
}

TEST_CASE("PALM Serialization of Division", "[PALM][Serializer][Division]")
{
    const Oasis::Divide<> division {
        Oasis::Real { 6.0 },
        Oasis::Real { 3.0 }
    };

    Oasis::PALMSerializer serializer {};

    auto result = division.Accept(serializer);
    std::string expected = "( / ( real 6 ) ( real 3 ) )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Nested Division", "[PALM][Serializer][Division]")
{
    const Oasis::Divide<> division {
        Oasis::Real { 6.0 },
        Oasis::Divide {
            Oasis::Real { 3.0 },
            Oasis::Real { 2.0 } }
    };

    Oasis::PALMSerializer serializer {};

    auto result = division.Accept(serializer);
    std::string expected = "( / ( real 6 ) ( / ( real 3 ) ( real 2 ) ) )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Malformed Division", "[PALM][Serializer][Division]")
{
    { // Missing least significant operand
        Oasis::Divide<Oasis::Real, Oasis::Expression> division;
        division.SetMostSigOp(Oasis::Real { 6.0 });

        Oasis::PALMSerializer serializer = Oasis::PALMSerializer();

        auto result = division.Accept(serializer);

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Expression missing least significant operand");
    }

    { // Missing most significant operand
        Oasis::Divide<Oasis::Expression, Oasis::Real> division;
        division.SetLeastSigOp(Oasis::Real { 3.0 });

        Oasis::PALMSerializer serializer = Oasis::PALMSerializer();

        auto result = division.Accept(serializer);

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Expression missing most significant operand");
    }
}

TEST_CASE("PALM Serialization of Exponentiation", "[PALM][Serializer][Exponentiation]")
{
    const Oasis::Exponent<> exponent {
        Oasis::Real { 2.0 },
        Oasis::Real { 3.0 }
    };

    Oasis::PALMSerializer serializer {};

    auto result = exponent.Accept(serializer);
    std::string expected = "( ^ ( real 2 ) ( real 3 ) )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Nested Exponentiation", "[PALM][Serializer][Exponentiation]")
{
    const Oasis::Exponent<> exponent {
        Oasis::Real { 2.0 },
        Oasis::Exponent {
            Oasis::Real { 3.0 },
            Oasis::Real { 2.0 } }
    };

    Oasis::PALMSerializer serializer {};

    auto result = exponent.Accept(serializer);
    std::string expected = "( ^ ( real 2 ) ( ^ ( real 3 ) ( real 2 ) ) )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Malformed Exponentiation", "[PALM][Serializer][Exponentiation]")
{
    { // Missing least significant operand
        Oasis::Exponent<Oasis::Real, Oasis::Expression> exponent;
        exponent.SetMostSigOp(Oasis::Real { 2.0 });

        Oasis::PALMSerializer serializer = Oasis::PALMSerializer();

        auto result = exponent.Accept(serializer);

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Expression missing least significant operand");
    }

    { // Missing most significant operand
        Oasis::Exponent<Oasis::Expression, Oasis::Real> exponent;
        exponent.SetLeastSigOp(Oasis::Real { 3.0 });

        Oasis::PALMSerializer serializer = Oasis::PALMSerializer();

        auto result = exponent.Accept(serializer);

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Expression missing most significant operand");
    }
}

TEST_CASE("PALM Serialization of Logarithm", "[PALM][Serializer][Logarithm]")
{
    const Oasis::Log<> log {
        Oasis::Real { 10.0 },
        Oasis::Real { 1000.0 }
    };

    Oasis::PALMSerializer serializer {};

    auto result = log.Accept(serializer);
    std::string expected = "( log ( real 10 ) ( real 1000 ) )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Nested Logarithm", "[PALM][Serializer][Logarithm]")
{
    const Oasis::Log<> log {
        Oasis::Real { 10.0 },
        Oasis::Log {
            Oasis::Real { 10.0 },
            Oasis::Real { 1000.0 } }
    };

    Oasis::PALMSerializer serializer {};

    auto result = log.Accept(serializer);
    std::string expected = "( log ( real 10 ) ( log ( real 10 ) ( real 1000 ) ) )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Malformed Logarithm", "[PALM][Serializer][Logarithm]")
{
    { // Missing least significant operand
        Oasis::Log<Oasis::Real, Oasis::Expression> log;
        log.SetMostSigOp(Oasis::Real { 10.0 });

        Oasis::PALMSerializer serializer = Oasis::PALMSerializer();

        auto result = log.Accept(serializer);

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Expression missing least significant operand");
    }

    { // Missing most significant operand
        Oasis::Log<Oasis::Expression, Oasis::Real> log;
        log.SetLeastSigOp(Oasis::Real { 1000.0 });

        Oasis::PALMSerializer serializer = Oasis::PALMSerializer();

        auto result = log.Accept(serializer);

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Expression missing most significant operand");
    }
}

TEST_CASE("PALM Serialization of Negation", "[PALM][Serializer][Negation]")
{
    const Oasis::Negate<> negate {
        Oasis::Real { 5.0 }
    };

    Oasis::PALMSerializer serializer {};

    auto result = negate.Accept(serializer).value();
    std::string expected = "( neg ( real 5 ) )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Nested Negation", "[PALM][Serializer][Negation]")
{
    const Oasis::Negate<> negate {
        Oasis::Negate {
            Oasis::Real { 5.0 } }
    };

    Oasis::PALMSerializer serializer {};

    auto result = negate.Accept(serializer).value();
    std::string expected = "( neg ( neg ( real 5 ) ) )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Malformed Negation", "[PALM][Serializer][Negation]")
{
    { // Missing operand
        Oasis::Negate<Oasis::Expression> negate;

        Oasis::PALMSerializer serializer = Oasis::PALMSerializer();

        auto result = negate.Accept(serializer);

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Expression missing most significant operand");
    }
}

TEST_CASE("PALM Serialization of Derivative", "[PALM][Serializer][Derivative]")
{
    const Oasis::Derivative<> derivative {
        Oasis::Variable { "x" },
        Oasis::Real { 5.0 }
    };

    Oasis::PALMSerializer serializer {};

    auto result = derivative.Accept(serializer);
    std::string expected = "( d ( var x ) ( real 5 ) )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Nested Derivative", "[PALM][Serializer][Derivative]")
{
    const Oasis::Derivative<> derivative {
        Oasis::Variable { "x" },
        Oasis::Derivative {
            Oasis::Variable { "y" },
            Oasis::Real { 5.0 } }
    };

    Oasis::PALMSerializer serializer {};

    auto result = derivative.Accept(serializer);
    std::string expected = "( d ( var x ) ( d ( var y ) ( real 5 ) ) )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Malformed Derivative", "[PALM][Serializer][Derivative]")
{
    { // Missing least significant operand
        Oasis::Derivative<Oasis::Variable, Oasis::Expression> derivative;
        derivative.SetMostSigOp(Oasis::Variable { "x" });

        Oasis::PALMSerializer serializer = Oasis::PALMSerializer();

        auto result = derivative.Accept(serializer);

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Expression missing least significant operand");
    }

    { // Missing most significant operand
        Oasis::Derivative<Oasis::Expression, Oasis::Real> derivative;
        derivative.SetLeastSigOp(Oasis::Real { 5.0 });

        Oasis::PALMSerializer serializer = Oasis::PALMSerializer();

        auto result = derivative.Accept(serializer);

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Expression missing most significant operand");
    }
}

TEST_CASE("PALM Serialization of Integral", "[PALM][Serializer][Integral]")
{
    const Oasis::Integral<> integral {
        Oasis::Variable { "x" },
        Oasis::Real { 5.0 }
    };

    Oasis::PALMSerializer serializer {};

    auto result = integral.Accept(serializer);
    std::string expected = "( int ( var x ) ( real 5 ) )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Nested Integral", "[PALM][Serializer][Integral]")
{
    const Oasis::Integral<> integral {
        Oasis::Variable { "x" },
        Oasis::Integral {
            Oasis::Variable { "y" },
            Oasis::Real { 5.0 } }
    };

    Oasis::PALMSerializer serializer {};

    auto result = integral.Accept(serializer);
    std::string expected = "( int ( var x ) ( int ( var y ) ( real 5 ) ) )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Malformed Integral", "[PALM][Serializer][Integral]")
{
    { // Missing least significant operand
        Oasis::Integral<Oasis::Variable, Oasis::Expression> integral;
        integral.SetMostSigOp(Oasis::Variable { "x" });

        Oasis::PALMSerializer serializer = Oasis::PALMSerializer();

        auto result = integral.Accept(serializer);

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Expression missing least significant operand");
    }

    { // Missing most significant operand
        Oasis::Integral<Oasis::Expression, Oasis::Real> integral;
        integral.SetLeastSigOp(Oasis::Real { 5.0 });

        Oasis::PALMSerializer serializer = Oasis::PALMSerializer();

        auto result = integral.Accept(serializer);

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Expression missing most significant operand");
    }
}

// TODO: Add tests for Matrix

TEST_CASE("PALM Serialization of Euler Number", "[PALM][Serializer][EulerNumber]")
{
    const Oasis::EulerNumber euler {};

    Oasis::PALMSerializer serializer {};

    auto result = euler.Accept(serializer).value();
    std::string expected = "( e )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Pi", "[PALM][Serializer][Pi]")
{
    const Oasis::Pi pi {};

    Oasis::PALMSerializer serializer {};

    auto result = pi.Accept(serializer).value();
    std::string expected = "( pi )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Magnitude", "[PALM][Serializer][Magnitude]")
{
    const Oasis::Magnitude<Oasis::Real> magnitude {
        Oasis::Real { -5.0 }
    };

    Oasis::PALMSerializer serializer {};

    auto result = magnitude.Accept(serializer).value();
    std::string expected = "( magnitude ( real -5 ) )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Nested Magnitude", "[PALM][Serializer][Magnitude]")
{
    const Oasis::Magnitude<Oasis::Magnitude<Oasis::Real>> magnitude {
        Oasis::Magnitude {
            Oasis::Real { -5.0 } }
    };

    Oasis::PALMSerializer serializer {};

    auto result = magnitude.Accept(serializer).value();
    std::string expected = "( magnitude ( magnitude ( real -5 ) ) )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Malformed Magnitude", "[PALM][Serializer][Magnitude]")
{
    { // Missing operand
        Oasis::Magnitude<Oasis::Expression> magnitude;

        Oasis::PALMSerializer serializer = Oasis::PALMSerializer();

        auto result = magnitude.Accept(serializer);

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Expression missing most significant operand");
    }
}

TEST_CASE("PALM Serialization of Complex Expressions", "[PALM][Serializer][ComplexExpression]")
{
    Oasis::PALMSerializer serializer {};

    { // Test 1
        const Oasis::Add<> expression {
            Oasis::Real { 5.0 },
            Oasis::Multiply {
                Oasis::Variable { "x" },
                Oasis::Exponent {
                    Oasis::Real { 2.0 },
                    Oasis::Real { 3.0 } } }
        };

        auto result = expression.Accept(serializer).value();
        std::string expected = "( + ( real 5 ) ( * ( var x ) ( ^ ( real 2 ) ( real 3 ) ) ) )";

        REQUIRE(expected == result);
    }

    { // Test 2
        const Oasis::Subtract<> expression {
            Oasis::Divide {
                Oasis::Real { 10.0 },
                Oasis::Real { 2.0 } },
            Oasis::Log {
                Oasis::Real { 10.0 },
                Oasis::Real { 100.0 } }
        };

        auto result = expression.Accept(serializer).value();
        std::string expected = "( - ( / ( real 10 ) ( real 2 ) ) ( log ( real 10 ) ( real 100 ) ) )";

        REQUIRE(expected == result);
    }
}

TEST_CASE("PALM Serialization with Different Expression Padding", "[PALM][Serializer][Padding]")
{
    { // No Padding
        Oasis::PALMSerializer serializer({ .expressionPadding = "" });

        const Oasis::Add<> expression {
            Oasis::Real { 5.0 },
            Oasis::Multiply {
                Oasis::Variable { "x" },
                Oasis::Exponent {
                    Oasis::Real { 2.0 },
                    Oasis::Real { 3.0 } } }
        };

        auto result = expression.Accept(serializer).value();
        std::string expected = "(+ (real 5) (* (var x) (^ (real 2) (real 3))))";

        REQUIRE(expected == result);
    }

    { // Tab Padding
        Oasis::PALMSerializer serializer({ .expressionPadding = "\t" });

        const Oasis::Add<> expression {
            Oasis::Real { 5.0 },
            Oasis::Multiply {
                Oasis::Variable { "x" },
                Oasis::Exponent {
                    Oasis::Real { 2.0 },
                    Oasis::Real { 3.0 } } }
        };

        auto result = expression.Accept(serializer).value();
        std::string expected = "(\t+ (\treal 5\t) (\t* (\tvar x\t) (\t^ (\treal 2\t) (\treal 3\t)\t)\t)\t)";

        REQUIRE(expected == result);
    }

    { // %bruh% Padding (because I can)
        Oasis::PALMSerializer serializer({ .expressionPadding = "%bruh%" });

        const Oasis::Add<> expression {
            Oasis::Real { 5.0 },
            Oasis::Multiply {
                Oasis::Variable { "x" },
                Oasis::Exponent {
                    Oasis::Real { 2.0 },
                    Oasis::Real { 3.0 } } }
        };

        auto result = expression.Accept(serializer).value();
        std::string expected = "(%bruh%+ (%bruh%real 5%bruh%) (%bruh%* (%bruh%var x%bruh%) (%bruh%^ (%bruh%real 2%bruh%) (%bruh%real 3%bruh%)%bruh%)%bruh%)%bruh%)";

        REQUIRE(expected == result);
    }
}

TEST_CASE("PALM Serialization with Different Token Separators", "[PALM][Serializer][TokenSeparator]")
{
    { // Double Space Separator
        Oasis::PALMSerializer serializer({ .tokenSeparator = "  " });

        const Oasis::Add<> expression {
            Oasis::Real { 5.0 },
            Oasis::Multiply {
                Oasis::Variable { "x" },
                Oasis::Exponent {
                    Oasis::Real { 2.0 },
                    Oasis::Real { 3.0 } } }
        };

        auto result = expression.Accept(serializer).value();
        std::string expected = "( +  ( real  5 )  ( *  ( var  x )  ( ^  ( real  2 )  ( real  3 ) ) ) )";

        REQUIRE(expected == result);
    }

    { // % Separator
        Oasis::PALMSerializer serializer({ .tokenSeparator = "%" });

        const Oasis::Add<> expression {
            Oasis::Real { 5.0 },
            Oasis::Multiply {
                Oasis::Variable { "x" },
                Oasis::Exponent {
                    Oasis::Real { 2.0 },
                    Oasis::Real { 3.0 } } }
        };

        auto result = expression.Accept(serializer).value();
        std::string expected = "( +%( real%5 )%( *%( var%x )%( ^%( real%2 )%( real%3 ) ) ) )";

        REQUIRE(expected == result);
    }
}

/*
 * Parsing
 */
TEST_CASE("PALM Parsing of Real with various formats", "[PALM][Parser][Real]")
{
    { // Test with integer
        const auto expr = Oasis::FromPALM("( real 42 )");

        REQUIRE(expr.has_value());
        REQUIRE((*expr)->Is<Oasis::Real>());

        const auto real = Oasis::RecursiveCast<Oasis::Real>(**expr);
        REQUIRE(real->GetValue() == 42.0);
    }

    { // Test with float
        const auto expr = Oasis::FromPALM("( real 3.14 )");

        REQUIRE(expr.has_value());
        REQUIRE((*expr)->Is<Oasis::Real>());

        const auto real = Oasis::RecursiveCast<Oasis::Real>(**expr);
        REQUIRE(real->GetValue() == 3.14);
    }

    { // Test with negative float
        const auto expr = Oasis::FromPALM("( real -0.001 )");

        REQUIRE(expr.has_value());
        REQUIRE((*expr)->Is<Oasis::Real>());

        const auto real = Oasis::RecursiveCast<Oasis::Real>(**expr);
        REQUIRE(real->GetValue() == -0.001);
    }

    { // Test with scientific notation
        const auto expr = Oasis::FromPALM("( real 1.5e3 )");

        REQUIRE(expr.has_value());
        REQUIRE((*expr)->Is<Oasis::Real>());

        const auto real = Oasis::RecursiveCast<Oasis::Real>(**expr);
        REQUIRE(real->GetValue() == 1500.0);
    }
}

// TEST_CASE("PALM Parsing of Malformed Real", "[PALM][Parser][Real]")
// {
//     { // Missing value
//         const auto expr = Oasis::FromPALM("( real )");
//
//         auto error = expr.error();
//         auto expectedError = Oasis::PALMParseError {
//             .token = Oasis::PALMToken {
//                 .type = Oasis::PALMTokenType::Punctuator,
//                 .lexeme = ")",
//                 .charOffset = 7,
//                 .tokenIndex = 2,
//                 .line = 1,
//                 .column = 8,
//             },
//             .type = Oasis::PALMParseErrorType::MissingOperands,
//             .message = "Expected a real number after 'real' token."
//         };
//
//         REQUIRE(!expr);
//         REQUIRE(error == expectedError);
//     }
//
//     { // Invalid number format
//         const auto expr = Oasis::FromPALM("( real three.point.onefour )");
//
//         auto error = expr.error();
//         auto expectedError = Oasis::PALMParseError {
//             .token = Oasis::PALMToken {
//                 .type = Oasis::PALMTokenType::Unknown,
//                 .lexeme = "three.point.onefour",
//                 .charOffset = 12,
//                 .tokenIndex = 3,
//                 .line = 1,
//                 .column = 13,
//             },
//             .type = Oasis::PALMParseErrorType::InvalidNumberFormat,
//             .message = "Invalid number format for real number: 'three.point.onefour'."
//         };
//
//         REQUIRE(!expr);
//         REQUIRE(error == expectedError);
//     }
//
//     { // Number out of bounds
//         const auto expr = Oasis::FromPALM("( real 1e5000 )");
//
//         auto error = expr.error();
//         auto expectedError = Oasis::PALMParseError {
//             .token = Oasis::PALMToken {
//                 .type = Oasis::PALMTokenType::Unknown,
//                 .lexeme = "1e5000",
//                 .charOffset = 12,
//                 .tokenIndex = 3,
//                 .line = 1,
//                 .column = 13,
//             },
//             .type = Oasis::PALMParseErrorType::NumberOutOfBounds,
//             .message = "Real number out of bounds: '1e5000'."
//         };
//
//         REQUIRE(!expr);
//         REQUIRE(error == expectedError);
//     }
//
//     { // Extra tokens
//         const auto expr = Oasis::FromPALM("( real 3.14 extra )");
//
//         auto error = expr.error();
//         auto expectedError = Oasis::PALMParseError {
//             .token = Oasis::PALMToken {
//                 .type = Oasis::PALMTokenType::Identifier,
//                 .lexeme = "extra",
//                 .charOffset = 17,
//                 .tokenIndex = 4,
//                 .line = 1,
//                 .column = 18,
//             },
//             .type = Oasis::PALMParseErrorType::ExtraOperands,
//             .message = "Unexpected token 'extra' when parsing operator 'real'."
//         };
//
//         REQUIRE(!expr);
//         REQUIRE(error == expectedError);
//     }
// }
//
// TEST_CASE("PALM Parsing of Real with different spacing and padding", "[PALM][Parser][Real]")
// {
//     { // Extra spaces
//         const auto expr = Oasis::FromPALM("(   real    7.89   )");
//
//         REQUIRE(expr.has_value());
//         REQUIRE((*expr)->Is<Oasis::Real>());
//
//         const auto real = Oasis::RecursiveCast<Oasis::Real>(**expr);
//         REQUIRE(real->GetValue() == 7.89);
//     }
//
//     { // Tab padding
//         const auto expr = Oasis::FromPALM("(\treal\t0.5772\t)");
//
//         REQUIRE(expr.has_value());
//         REQUIRE((*expr)->Is<Oasis::Real>());
//
//         const auto real = Oasis::RecursiveCast<Oasis::Real>(**expr);
//         REQUIRE(real->GetValue() == 0.5772);
//     }
//
//     { // Newline padding
//         const auto expr = Oasis::FromPALM("(\nreal\n1.618\n)");
//
//         REQUIRE(expr.has_value());
//         REQUIRE((*expr)->Is<Oasis::Real>());
//
//         const auto real = Oasis::RecursiveCast<Oasis::Real>(**expr);
//         REQUIRE(real->GetValue() == 1.618);
//     }
// }
//
// TEST_CASE("PALM Parsing of Imaginary with different characters", "[PALM][Parser][Imaginary]")
// {
//     { // Test with 'i'
//         const auto expr = Oasis::FromPALM("( i )");
//
//         REQUIRE(expr.has_value());
//         REQUIRE((*expr)->Is<Oasis::Imaginary>());
//     }
//
//     { // Test with 'j'
//         const auto expr = Oasis::FromPALM("( j )");
//
//         REQUIRE(expr.has_value());
//         REQUIRE((*expr)->Is<Oasis::Imaginary>());
//     }
// }
//
// TEST_CASE("PALM Parsing of Malformed Imaginary", "[PALM][Parser][Imaginary]")
// {
//     { // Extra tokens
//         const auto expr = Oasis::FromPALM("( i extra )");
//
//         auto error = expr.error();
//         auto expectedError = Oasis::PALMParseError {
//             .token = Oasis::PALMToken {
//                 .type = Oasis::PALMTokenType::Identifier,
//                 .lexeme = "extra",
//                 .charOffset = 5,
//                 .tokenIndex = 2,
//                 .line = 1,
//                 .column = 6,
//             },
//             .type = Oasis::PALMParseErrorType::ExtraOperands,
//             .message = "Unexpected token 'extra' when parsing operator 'i'."
//         };
//
//         REQUIRE(!expr);
//         REQUIRE(error == expectedError);
//     }
// }
//
// TEST_CASE("PALM Parsing of Variable with various name formats", "[PALM][Parser][Variable]")
// {
//     { // Single letter
//         const auto expr = Oasis::FromPALM("( var x )");
//
//         REQUIRE(expr.has_value());
//         REQUIRE((*expr)->Is<Oasis::Variable>());
//
//         const auto var = Oasis::RecursiveCast<Oasis::Variable>(**expr);
//         REQUIRE(var->GetName() == "x");
//     }
//
//     { // Multi-letter
//         const auto expr = Oasis::FromPALM("( var myVariable )");
//
//         REQUIRE(expr.has_value());
//         REQUIRE((*expr)->Is<Oasis::Variable>());
//
//         const auto var = Oasis::RecursiveCast<Oasis::Variable>(**expr);
//         REQUIRE(var->GetName() == "myVariable");
//     }
//
//     { // With numbers
//         const auto expr = Oasis::FromPALM("( var var123 )");
//
//         REQUIRE(expr.has_value());
//         REQUIRE((*expr)->Is<Oasis::Variable>());
//
//         const auto var = Oasis::RecursiveCast<Oasis::Variable>(**expr);
//         REQUIRE(var->GetName() == "var123");
//     }
//
//     { // With underscore
//         const auto expr = Oasis::FromPALM("( var var_name )");
//
//         REQUIRE(expr.has_value());
//         REQUIRE((*expr)->Is<Oasis::Variable>());
//
//         const auto var = Oasis::RecursiveCast<Oasis::Variable>(**expr);
//         REQUIRE(var->GetName() == "var_name");
//     }
// }
//
// TEST_CASE("PALM Parsing of Malformed Variable", "[PALM][Parser][Variable]")
// {
//     { // Missing name
//         const auto expr = Oasis::FromPALM("( var )");
//
//         auto error = expr.error();
//         auto expectedError = Oasis::PALMParseError {
//             .token = Oasis::PALMToken {
//                 .type = Oasis::PALMTokenType::Punctuator,
//                 .lexeme = ")",
//                 .charOffset = 6,
//                 .tokenIndex = 2,
//                 .line = 1,
//                 .column = 7,
//             },
//             .type = Oasis::PALMParseErrorType::MissingOperands,
//             .message = "Expected a variable name after 'var' token."
//         };
//
//         REQUIRE(!expr);
//         REQUIRE(error == expectedError);
//     }
//
//     { // Extra tokens
//         const auto expr = Oasis::FromPALM("( var x extra )");
//
//         auto error = expr.error();
//         auto expectedError = Oasis::PALMParseError {
//             .token = Oasis::PALMToken {
//                 .type = Oasis::PALMTokenType::Identifier,
//                 .lexeme = "extra",
//                 .charOffset = 9,
//                 .tokenIndex = 3,
//                 .line = 1,
//                 .column = 10,
//             },
//             .type = Oasis::PALMParseErrorType::ExtraOperands,
//             .message = "Unexpected token 'extra' when parsing operator 'var'."
//         };
//
//         REQUIRE(!expr);
//         REQUIRE(error == expectedError);
//     }
//
//     { // Number
//         const auto expr = Oasis::FromPALM("( var 123 )");
//
//         auto error = expr.error();
//         auto expectedError = Oasis::PALMParseError {
//             .token = Oasis::PALMToken {
//                 .type = Oasis::PALMTokenType::Number,
//                 .lexeme = "123",
//                 .charOffset = 6,
//                 .tokenIndex = 2,
//                 .line = 1,
//                 .column = 7,
//             },
//             .type = Oasis::PALMParseErrorType::InvalidVariableName,
//             .message = "Invalid variable name: '123'."
//         };
//
//         REQUIRE(!expr);
//         REQUIRE(error == expectedError);
//     }
//
//     { // Expression
//         const auto expr = Oasis::FromPALM("( var ( real 5 ) )");
//
//         auto error = expr.error();
//         auto expectedError = Oasis::PALMParseError {
//             .token = Oasis::PALMToken {
//                 .type = Oasis::PALMTokenType::Punctuator,
//                 .lexeme = "(",
//                 .charOffset = 6,
//                 .tokenIndex = 2,
//                 .line = 1,
//                 .column = 7,
//             },
//             .type = Oasis::PALMParseErrorType::InvalidVariableName,
//             .message = "Invalid variable name: '('."
//         };
//
//         REQUIRE(!expr);
//         REQUIRE(error == expectedError);
//     }
// }
//
// TEST_CASE("PALM Parsing of Addition", "[PALM][Parser][Addition]")
// {
//     { // Simple Addition
//         const Oasis::Add<> expected {
//             Oasis::Real { 5.0 },
//             Oasis::Real { 3.0 },
//         };
//
//         OASIS_CAPTURE_WITH_SERIALIZER(expected);
//
//         const auto expr = Oasis::FromPALM("( + ( real 5 ) ( real 3 ) )");
//         REQUIRE(expr.has_value());
//         REQUIRE((*expr)->Equals(expected));
//     }
//
//     { // Nested Addition
//         const Oasis::Add<> expected {
//             Oasis::Real { 5.0 },
//             Oasis::Add {
//                 Oasis::Real { 3.0 },
//                 Oasis::Real { 2.0 } }
//         };
//
//         OASIS_CAPTURE_WITH_SERIALIZER(expected);
//
//         const auto expr = Oasis::FromPALM("( + ( real 5 ) ( + ( real 3 ) ( real 2 ) ) )");
//         REQUIRE(expr.has_value());
//         REQUIRE((*expr)->Equals(expected));
//     }
// }
//
// TEST_CASE("PALM Parsing of Malformed Addition", "[PALM][Parser][Addition]")
// {
//     { // Missing least significant operand
//         const auto expr = Oasis::FromPALM("( + ( real 5 ) )");
//
//         auto error = expr.error();
//         auto expectedError = Oasis::PALMParseError {
//             .token = Oasis::PALMToken {
//                 .type = Oasis::PALMTokenType::Punctuator,
//                 .lexeme = ")",
//                 .charOffset = 15,
//                 .tokenIndex = 4,
//                 .line = 1,
//                 .column = 16,
//             },
//             .type = Oasis::PALMParseErrorType::MissingOperands,
//             .message = "Incomplete '+' expression; expected at least two operands."
//         };
//
//         REQUIRE(!expr);
//         REQUIRE(error == expectedError);
//     }
//
//     { // Missing most significant operand
//         const auto expr = Oasis::FromPALM("( + )");
//
//         auto error = expr.error();
//         auto expectedError = Oasis::PALMParseError {
//             .token = Oasis::PALMToken {
//                 .type = Oasis::PALMTokenType::Punctuator,
//                 .lexeme = ")",
//                 .charOffset = 4,
//                 .tokenIndex = 2,
//                 .line = 1,
//                 .column = 5,
//             },
//             .type = Oasis::PALMParseErrorType::MissingOperands,
//             .message = "Incomplete '+' expression; expected at least two operands."
//         };
//
//         REQUIRE(!expr);
//         REQUIRE(error == expectedError);
//     }
//
//     { // Too many operands
//         const auto expr = Oasis::FromPALM("( + ( real 5 ) ( real 3 ) ( real 2 ) )");
//
//         auto error = expr.error();
//         auto expectedError = Oasis::PALMParseError {
//             .token = Oasis::PALMToken {
//                 .type = Oasis::PALMTokenType::Identifier,
//                 .lexeme = "(",
//                 .charOffset = 30,
//                 .tokenIndex = 6,
//                 .line = 1,
//                 .column = 31,
//             },
//             .type = Oasis::PALMParseErrorType::ExtraOperands,
//             .message = "Unexpected token '(' when parsing operator '+'."
//         };
//
//         REQUIRE(!expr);
//         REQUIRE(error == expectedError);
//     }
//
//     { // Raw number as operand
//         const auto expr = Oasis::FromPALM("( + 5 ( real 3 ) )");
//
//         auto error = expr.error();
//         auto expectedError = Oasis::PALMParseError {
//             .token = Oasis::PALMToken {
//                 .type = Oasis::PALMTokenType::Number,
//                 .lexeme = "5",
//                 .charOffset = 4,
//                 .tokenIndex = 2,
//                 .line = 1,
//                 .column = 5,
//             },
//             .type = Oasis::PALMParseErrorType::InvalidOperand,
//             .message = "Invalid operand '5' for '+' operator; expected an expression."
//         };
//
//         REQUIRE(!expr);
//         REQUIRE(error == expectedError);
//     }
//
//     { // Raw identifier as operand
//         const auto expr = Oasis::FromPALM("( + ( real 5 ) x )");
//
//         auto error = expr.error();
//         auto expectedError = Oasis::PALMParseError {
//             .token = Oasis::PALMToken {
//                 .type = Oasis::PALMTokenType::Identifier,
//                 .lexeme = "x",
//                 .charOffset = 18,
//                 .tokenIndex = 5,
//                 .line = 1,
//                 .column = 19,
//             },
//             .type = Oasis::PALMParseErrorType::InvalidOperand,
//             .message = "Invalid operand 'x' for '+' operator; expected an expression."
//         };
//
//         REQUIRE(!expr);
//         REQUIRE(error == expectedError);
//     }
// }
//
// TEST_CASE("PALM Parsing of Malformed Expression", "[PALM][Parser][Expression]")
// {
//     { // Missing opening parenthesis
//         const auto expr = Oasis::FromPALM("+ ( real 5 ) ( real 3 ) )");
//
//         auto error = expr.error();
//         auto expectedError = Oasis::PALMParseError {
//             .token = Oasis::PALMToken {
//                 .type = Oasis::PALMTokenType::Punctuator,
//                 .lexeme = "+",
//                 .charOffset = 0,
//                 .tokenIndex = 0,
//                 .line = 1,
//                 .column = 1,
//             },
//             .type = Oasis::PALMParseErrorType::UnexpectedToken,
//             .message = "Expected '(' at the beginning of an expression."
//         };
//
//         REQUIRE(!expr);
//         REQUIRE(error == expectedError);
//     }
//
//     { // Extra closing parenthesis
//         const auto expr = Oasis::FromPALM("( + ( real 5 ) ( real 3 ) ))");
//
//         auto error = expr.error();
//         auto expectedError = Oasis::PALMParseError {
//             .token = Oasis::PALMToken {
//                 .type = Oasis::PALMTokenType::Punctuator,
//                 .lexeme = ")",
//                 .charOffset = 30,
//                 .tokenIndex = 7,
//                 .line = 1,
//                 .column = 31,
//             },
//             .type = Oasis::PALMParseErrorType::UnexpectedToken,
//             .message = "Unexpected token ')' after end of expression."
//         };
//
//         REQUIRE(!expr);
//         REQUIRE(error == expectedError);
//     }
//
//     { // Missing expression
//         const auto expr = Oasis::FromPALM("");
//
//         auto error = expr.error();
//         auto expectedError = Oasis::PALMParseError {
//             .token = Oasis::PALMToken {
//                 .type = Oasis::PALMTokenType::EndOfInput,
//                 .lexeme = "",
//                 .charOffset = 0,
//                 .tokenIndex = 0,
//                 .line = 1,
//                 .column = 1,
//             },
//             .type = Oasis::PALMParseErrorType::UnexpectedEndOfInput,
//             .message = "Expected an expression but found end of input."
//         };
//
//         REQUIRE(!expr);
//         REQUIRE(error == expectedError);
//     }
//
//     { // False start of expression
//         const auto expr = Oasis::FromPALM("'");
//
//         auto error = expr.error();
//         auto expectedError = Oasis::PALMParseError {
//             .token = Oasis::PALMToken {
//                 .type = Oasis::PALMTokenType::Unknown,
//                 .lexeme = "'",
//                 .charOffset = 0,
//                 .tokenIndex = 0,
//                 .line = 1,
//                 .column = 1,
//             },
//             .type = Oasis::PALMParseErrorType::UnexpectedToken,
//             .message = "Expected '(' at the beginning of an expression."
//         };
//
//         REQUIRE(!expr);
//         REQUIRE(error == expectedError);
//     }
//
//     { // Empty expression
//         const auto expr = Oasis::FromPALM("()");
//
//         auto error = expr.error();
//         auto expectedError = Oasis::PALMParseError {
//             .token = Oasis::PALMToken {
//                 .type = Oasis::PALMTokenType::Punctuator,
//                 .lexeme = ")",
//                 .charOffset = 1,
//                 .tokenIndex = 1,
//                 .line = 1,
//                 .column = 2,
//             },
//             .type = Oasis::PALMParseErrorType::MissingOperator,
//             .message = "Expected an operator after '('."
//         };
//
//         REQUIRE(!expr);
//         REQUIRE(error == expectedError);
//     }
//
//     { // Missing operator
//         const auto expr = Oasis::FromPALM("( ( real 5 ) ( real 3 ) )");
//
//         auto error = expr.error();
//         auto expectedError = Oasis::PALMParseError {
//             .token = Oasis::PALMToken {
//                 .type = Oasis::PALMTokenType::Punctuator,
//                 .lexeme = "(",
//                 .charOffset = 2,
//                 .tokenIndex = 1,
//                 .line = 1,
//                 .column = 3,
//             },
//             .type = Oasis::PALMParseErrorType::MissingOperator,
//             .message = "Expected an operator after '('."
//         };
//
//         REQUIRE(!expr);
//         REQUIRE(error == expectedError);
//     }
//
//     { // Missing closing parenthesis
//         const auto expr = Oasis::FromPALM("( + ( real 5 ) ( real 3 ) ");
//
//         auto error = expr.error();
//         auto expectedError = Oasis::PALMParseError {
//             .token = Oasis::PALMToken {
//                 .type = Oasis::PALMTokenType::EndOfInput,
//                 .lexeme = "",
//                 .charOffset = 28,
//                 .tokenIndex = 6,
//                 .line = 1,
//                 .column = 28,
//             },
//             .type = Oasis::PALMParseErrorType::UnexpectedEndOfInput,
//             .message = "Expected ')' at the end of the expression."
//         };
//
//         REQUIRE(!expr);
//         REQUIRE(error == expectedError);
//     }
// }
//
// /* Beware, entering land of to be migrated */
// // TODO: Remove
// TEST_CASE("TEMP RUN PALM")
// {
//     const std::string palmString = "(+(real 5) (* (var x ) \n ( ^ ( real 2 ) ( real 3 ) ) ) )";
//
//     const std::string palmString2 = "( real three.point.onefour )";
//     const auto expr = Oasis::FromPALM(palmString2);
// }
//
// /*
//  * Subtraction Operation
//  */
// TEST_CASE("Parse Simple Subtraction", "[FromPALM][Parsing]")
// {
//     const Oasis::Subtract expected {
//         Oasis::Real { 5.0 },
//         Oasis::Real { 3.0 }
//     };
//
//     OASIS_CAPTURE_WITH_SERIALIZER(expected);
//
//     const auto expr = Oasis::FromPALMOld("( - ( real 5 ) ( real 3 ) )");
//     REQUIRE(expr.has_value());
//     REQUIRE((*expr)->Equals(expected));
// }
//
// TEST_CASE("Parse Nested Subtraction", "[FromPALM][Parsing]")
// {
//     const Oasis::Subtract expected {
//         Oasis::Real { 5.0 },
//         Oasis::Subtract {
//             Oasis::Real { 3.0 },
//             Oasis::Real { 2.0 } }
//     };
//
//     OASIS_CAPTURE_WITH_SERIALIZER(expected);
//
//     const auto expr = Oasis::FromPALMOld("( - ( real 5 ) ( - ( real 3 ) ( real 2 ) ) )");
//     REQUIRE(expr.has_value());
//     REQUIRE((*expr)->Equals(expected));
// }
//
// TEST_CASE("Parse Subtraction Multiple Elements", "[FromPALM][Parsing]")
// {
//     const Oasis::Subtract<> expected {
//         Oasis::Real { 5.0 },
//         Oasis::Subtract {
//             Oasis::Real { 3.0 },
//             Oasis::Subtract {
//                 Oasis::Real { 2.0 },
//                 Oasis::Real { 1.0 } } }
//     };
//
//     OASIS_CAPTURE_WITH_SERIALIZER(expected);
//
//     const auto expr = Oasis::FromPALMOld("( - ( real 5 ) ( real 3 ) ( real 2 ) ( real 1 ) )");
//     REQUIRE(expr.has_value());
//     REQUIRE((*expr)->Equals(expected));
// }
//
// TEST_CASE("Parse Malformed Subtraction", "[FromPALM][Parsing]")
// {
//     const auto expr = Oasis::FromPALMOld("( - ( real 5 ) )");
//     REQUIRE(!expr);
//     REQUIRE(expr.error() == Oasis::ParseErrorOld::IncompleteExpression);
// }
//
// TEST_CASE("Parse Subtraction Invalid Subexpression", "[FromPALM][Parsing]")
// {
//     const auto expr = Oasis::FromPALMOld("( - ( real 5 ) ( + ( real 3 ) ) )");
//     REQUIRE(!expr);
//     REQUIRE(expr.error() == Oasis::ParseErrorOld::IncompleteExpression);
// }
//
// /*
//  * Mixed Addition and Subtraction
//  */
// TEST_CASE("Parse Mixed Addition and Subtraction", "[FromPALM][Parsing]")
// {
//     const Oasis::Add expected {
//         Oasis::Real { 5.0 },
//         Oasis::Subtract {
//             Oasis::Real { 3.0 },
//             Oasis::Real { 2.0 } }
//     };
//
//     OASIS_CAPTURE_WITH_SERIALIZER(expected);
//
//     const auto expr = Oasis::FromPALMOld("( + ( real 5 ) ( - ( real 3 ) ( real 2 ) ) )");
//     REQUIRE(expr.has_value());
//     REQUIRE((*expr)->Equals(expected));
// }
//
// TEST_CASE("Parse Mixed Addition and Subtraction 2", "[FromPALM][Parsing]")
// {
//     const Oasis::Subtract expected {
//         Oasis::Real { 5.0 },
//         Oasis::Add {
//             Oasis::Real { 3.0 },
//             Oasis::Real { 2.0 } }
//     };
//
//     OASIS_CAPTURE_WITH_SERIALIZER(expected);
//
//     const auto expr = Oasis::FromPALMOld("( - ( real 5 ) ( + ( real 3 ) ( real 2 ) ) )");
//     REQUIRE(expr.has_value());
//     REQUIRE((*expr)->Equals(expected));
// }
//
// TEST_CASE("Parse Mixed Addition and Subtraction Multiple Elements", "[FromPALM][Parsing]")
// {
//     const Oasis::Add<> expected {
//         Oasis::Real { 5.0 },
//         Oasis::Subtract {
//             Oasis::Real { 3.0 },
//             Oasis::Add {
//                 Oasis::Real { 2.0 },
//                 Oasis::Real { 1.0 } } }
//     };
//
//     OASIS_CAPTURE_WITH_SERIALIZER(expected);
//
//     const auto expr = Oasis::FromPALMOld("( + ( real 5 ) ( - ( real 3 ) ( + ( real 2 ) ( real 1 ) ) ) )");
//     REQUIRE(expr.has_value());
//     REQUIRE((*expr)->Equals(expected));
// }

/*
 * Multiplication Operation
 */

TEST_CASE("Parse Simple Multiplication", "[FromPALM][Parsing]")
{
    const Oasis::Multiply expected {
        Oasis::Real { 5.0 },
        Oasis::Real { 3.0 }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALMOld("( * ( real 5 ) ( real 3 ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Nested Multiplication", "[FromPALM][Parsing]")
{
    const Oasis::Multiply expected {
        Oasis::Real { 5.0 },
        Oasis::Multiply {
            Oasis::Real { 3.0 },
            Oasis::Real { 2.0 } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALMOld("( * ( real 5 ) ( * ( real 3 ) ( real 2 ) ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Multiplication Multiple Elements", "[FromPALM][Parsing]")
{
    const Oasis::Multiply<> expected {
        Oasis::Real { 5.0 },
        Oasis::Real { 3.0 },
        Oasis::Real { 2.0 },
        Oasis::Real { 1.0 }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALMOld("( * ( real 5 ) ( real 3 ) ( real 2 ) ( real 1 ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Mixed Addition, Subtraction, and Multiplication", "[FromPALM][Parsing]")
{
    const Oasis::Add expected {
        Oasis::Real { 5.0 },
        Oasis::Subtract {
            Oasis::Real { 3.0 },
            Oasis::Multiply {
                Oasis::Real { 2.0 },
                Oasis::Real { 4.0 } } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALMOld("( + ( real 5 ) ( - ( real 3 ) ( * ( real 2 ) ( real 4 ) ) ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

/*
 * Division Operation
 */
TEST_CASE("Parse Simple Division", "[FromPALM][Parsing]")
{
    const Oasis::Divide expected {
        Oasis::Real { 6.0 },
        Oasis::Real { 3.0 }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALMOld("( / ( real 6 ) ( real 3 ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Nested Division", "[FromPALM][Parsing]")
{
    const Oasis::Divide expected {
        Oasis::Real { 6.0 },
        Oasis::Divide {
            Oasis::Real { 3.0 },
            Oasis::Real { 2.0 } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALMOld("( / ( real 6 ) ( / ( real 3 ) ( real 2 ) ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Division Multiple Elements", "[FromPALM][Parsing]")
{
    const Oasis::Divide<> expected {
        Oasis::Real { 6.0 },
        Oasis::Divide {
            Oasis::Real { 3.0 },
            Oasis::Divide {
                Oasis::Real { 2.0 },
                Oasis::Real { 1.0 } } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALMOld("( / ( real 6 ) ( real 3 ) ( real 2 ) ( real 1 ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Mixed Addition, Subtraction, Multiplication, and Division", "[FromPALM][Parsing]")
{
    const Oasis::Add expected {
        Oasis::Real { 5.0 },
        Oasis::Subtract {
            Oasis::Real { 3.0 },
            Oasis::Multiply {
                Oasis::Real { 2.0 },
                Oasis::Divide {
                    Oasis::Real { 4.0 },
                    Oasis::Real { 2.0 } } } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALMOld("( + ( real 5 ) ( - ( real 3 ) ( * ( real 2 ) ( / ( real 4 ) ( real 2 ) ) ) ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

/*
 * Exponent Operation
 */
TEST_CASE("Parse Simple Exponent", "[FromPALM][Parsing]")
{
    const Oasis::Exponent expected {
        Oasis::Real { 2.0 },
        Oasis::Real { 3.0 }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALMOld("( ^ ( real 2 ) ( real 3 ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Nested Exponent", "[FromPALM][Parsing]")
{
    const Oasis::Exponent expected {
        Oasis::Real { 2.0 },
        Oasis::Exponent {
            Oasis::Real { 3.0 },
            Oasis::Real { 2.0 } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALMOld("( ^ ( real 2 ) ( ^ ( real 3 ) ( real 2 ) ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Exponent Multiple Elements", "[FromPALM][Parsing]")
{
    const Oasis::Exponent<> expected {
        Oasis::Real { 2.0 },
        Oasis::Exponent {
            Oasis::Real { 3.0 },
            Oasis::Exponent {
                Oasis::Real { 2.0 },
                Oasis::Real { 1.0 } } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALMOld("( ^ ( real 2 ) ( real 3 ) ( real 2 ) ( real 1 ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Mixed Addition, Subtraction, Multiplication, Division, and Exponent", "[FromPALM][Parsing]")
{
    const Oasis::Add expected {
        Oasis::Real { 5.0 },
        Oasis::Subtract {
            Oasis::Real { 3.0 },
            Oasis::Multiply {
                Oasis::Real { 2.0 },
                Oasis::Divide {
                    Oasis::Real { 4.0 },
                    Oasis::Exponent {
                        Oasis::Real { 2.0 },
                        Oasis::Real { 3.0 } } } } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALMOld("( + ( real 5 ) ( - ( real 3 ) ( * ( real 2 ) ( / ( real 4 ) ( ^ ( real 2 ) ( real 3 ) ) ) ) ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

/*
 * Logarithm Operation
 */
TEST_CASE("Parse Simple Logarithm", "[FromPALM][Parsing]")
{
    const Oasis::Log expected {
        Oasis::Real { 2.0 },
        Oasis::Real { 8.0 }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALMOld("( log ( real 2 ) ( real 8 ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Nested Logarithm", "[FromPALM][Parsing]")
{
    const Oasis::Log expected {
        Oasis::Real { 2.0 },
        Oasis::Log {
            Oasis::Real { 3.0 },
            Oasis::Real { 9.0 } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALMOld("( log ( real 2 ) ( log ( real 3 ) ( real 9 ) ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Logarithm Multiple Elements", "[FromPALM][Parsing]")
{
    const Oasis::Log<> expected {
        Oasis::Real { 2.0 },
        Oasis::Log {
            Oasis::Real { 3.0 },
            Oasis::Log {
                Oasis::Real { 4.0 },
                Oasis::Real { 16.0 } } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALMOld("( log ( real 2 ) ( real 3 ) ( real 4 ) ( real 16 ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Mixed Many", "[FromPALM][Parsing]")
{
    const Oasis::Add expected {
        Oasis::Real { 5.0 },
        Oasis::Subtract {
            Oasis::Real { 3.0 },
            Oasis::Multiply {
                Oasis::Real { 2.0 },
                Oasis::Divide {
                    Oasis::Real { 4.0 },
                    Oasis::Exponent {
                        Oasis::Real { 2.0 },
                        Oasis::Log {
                            Oasis::Real { 2.0 },
                            Oasis::Real { 8.0 } } } } } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALMOld("( + ( real 5 ) ( - ( real 3 ) ( * ( real 2 ) ( / ( real 4 ) ( ^ ( real 2 ) ( log ( real 2 ) ( real 8 ) ) ) ) ) ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

/*
 * Integral Operation
 */
TEST_CASE("Parse Simple Integral", "[FromPALM][Parsing]")
{
    const Oasis::Integral expected {
        Oasis::Variable { "x" },
        Oasis::Real { 0.0 },
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALMOld("( int ( var x ) ( real 0 ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Nested Integral", "[FromPALM][Parsing]")
{
    const Oasis::Integral expected {
        Oasis::Variable { "x" },
        Oasis::Integral {
            Oasis::Variable { "y" },
            Oasis::Real { 0.0 } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALMOld("( int ( var x ) ( int ( var y ) ( real 0 ) ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Integral Multiple Elements", "[FromPALM][Parsing]")
{
    const Oasis::Integral<> expected {
        Oasis::Variable { "x" },
        Oasis::Integral {
            Oasis::Variable { "y" },
            Oasis::Integral {
                Oasis::Variable { "z" },
                Oasis::Real { 0.0 } } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALMOld("( int ( var x ) ( var y ) ( int ( var z ) ( real 0 ) ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Mixed Integral", "[FromPALM][Parsing]")
{
    const Oasis::Add expected {
        Oasis::Real { 5.0 },
        Oasis::Subtract {
            Oasis::Real { 3.0 },
            Oasis::Multiply {
                Oasis::Real { 2.0 },
                Oasis::Divide {
                    Oasis::Real { 4.0 },
                    Oasis::Exponent {
                        Oasis::Real { 2.0 },
                        Oasis::Log {
                            Oasis::Real { 2.0 },
                            Oasis::Integral {
                                Oasis::Variable { "x" },
                                Oasis::Real { 8.0 } } } } } } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALMOld("( + ( real 5 ) ( - ( real 3 ) ( * ( real 2 ) ( / ( real 4 ) ( ^ ( real 2 ) ( log ( real 2 ) ( int ( var x ) ( real 8 ) ) ) ) ) ) ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

/*
 * Negate Operation
 */
TEST_CASE("Parse Simple Negate", "[FromPALM][Parsing]")
{
    const Oasis::Negate expected {
        Oasis::Real { 5.0 }
    };

    const auto expr = Oasis::FromPALMOld("( neg ( real 5 ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Invalid Negate", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALMOld("( neg )");
    REQUIRE(!expr);
    REQUIRE(expr.error() == Oasis::ParseErrorOld::UnexpectedToken);
}

/*
 * Derivative Operation
 */
TEST_CASE("Parse Simple Derivative", "[FromPALM][Parsing]")
{
    const Oasis::Derivative expected {
        Oasis::Variable { "x" },
        Oasis::Real { 0.0 },
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALMOld("( d ( var x ) ( real 0 ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Nested Derivative", "[FromPALM][Parsing]")
{
    const Oasis::Derivative expected {
        Oasis::Variable { "x" },
        Oasis::Derivative {
            Oasis::Variable { "y" },
            Oasis::Real { 0.0 } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALMOld("( d ( var x ) ( d ( var y ) ( real 0 ) ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Derivative Multiple Elements", "[FromPALM][Parsing]")
{
    const Oasis::Derivative<> expected {
        Oasis::Variable { "x" },
        Oasis::Derivative {
            Oasis::Variable { "y" },
            Oasis::Derivative {
                Oasis::Variable { "z" },
                Oasis::Real { 0.0 } } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALMOld("( d ( var x ) ( var y ) ( d ( var z ) ( real 0 ) ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Mixed Derivative", "[FromPALM][Parsing]")
{
    const Oasis::Add expected {
        Oasis::Real { 5.0 },
        Oasis::Subtract {
            Oasis::Real { 3.0 },
            Oasis::Multiply {
                Oasis::Real { 2.0 },
                Oasis::Divide {
                    Oasis::Real { 4.0 },
                    Oasis::Exponent {
                        Oasis::Real { 2.0 },
                        Oasis::Log {
                            Oasis::Real { 2.0 },
                            Oasis::Derivative {
                                Oasis::Variable { "x" },
                                Oasis::Real { 8.0 } } } } } } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALMOld("( + ( real 5 ) ( - ( real 3 ) ( * ( real 2 ) ( / ( real 4 ) ( ^ ( real 2 ) ( log ( real 2 ) ( d ( var x ) ( real 8 ) ) ) ) ) ) ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

/*
 * Parse Constants Pi and Euler's Number
 */
TEST_CASE("Parse Constant Pi (pi)", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALMOld("( pi )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Is<Oasis::Pi>());
}

TEST_CASE("Parse Constant Euler's Number", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALMOld("( e )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Is<Oasis::EulerNumber>());
}

/*
 * Test Invalid Expressions
 */
TEST_CASE("Parse Invalid Expression", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALMOld("( unknown_op ( real 5 ) ( real 3 ) )");
    REQUIRE(!expr);
    REQUIRE(expr.error() == Oasis::ParseErrorOld::UnknownOperator);
}

TEST_CASE("Parse Empty Expression", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALMOld("");
    REQUIRE(!expr);
    REQUIRE(expr.error() == Oasis::ParseErrorOld::UnexpectedEndOfInput);
}
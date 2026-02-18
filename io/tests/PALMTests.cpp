#include "../../tests/Common.hpp"

#include "Oasis/RecursiveCast.hpp"
#include "catch2/catch_test_macros.hpp"
#include <catch2/catch_template_test_macros.hpp>

#include "Oasis/FromPALM.hpp"
#include "Oasis/PALMSerializer.hpp"

#include "../src/PALMTypes.hpp"

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
#include "Oasis/Undefined.hpp"
#include "Oasis/Variable.hpp"
#include "catch2/generators/catch_generators.hpp"

// =====================
// Test Helper Functions
// =====================

// ---------------------
// Serialization Helpers
// ---------------------
void requireSuccessfulSerialization(
    const Oasis::Expression& expr,
    const std::string& expected,
    const Oasis::PALMSerializationOpts& opts = {})
{
    Oasis::PALMSerializer serializer { opts };

    const auto visit = expr.Accept(serializer);

    const auto& result = visit.value();
    if (!result.has_value()) {
        CAPTURE(expected);
        CAPTURE(opts.numPlaces);
        FAIL("PALM serialization failed: " + result.error().message);
    }

    const auto& actual = result.value();
    CAPTURE(actual, expected, opts.numPlaces);
    REQUIRE(actual == expected);
}

void requireFailedSerialization(
    const Oasis::Expression& expr,
    const Oasis::PALMSerializationError& expectedError,
    const Oasis::PALMSerializationOpts& opts = {})
{
    Oasis::PALMSerializer serializer { opts };

    const auto visit = expr.Accept(serializer);
    REQUIRE(visit.has_value());

    const auto& result = visit.value();
    REQUIRE_FALSE(result.has_value());

    const auto& error = result.error();
    CAPTURE(error.message, expectedError.message);
    REQUIRE(error == expectedError);
}

void requireInvalidIdentifierSerializationError(
    const Oasis::Variable& expr,
    const std::string& expected,
    const Oasis::PALMSerializationOpts& opts = {})
{
    const auto expectedError = Oasis::PALMSerializationError {
        .type = Oasis::PALMSerializationError::PALMSerializationErrorType::InvalidIdentifier,
        .expression = &expr,
        .message = "Variable name '" + expected + "' is not a valid identifier"
    };

    requireFailedSerialization(expr, expectedError, opts);
}

void requireMissingOperandSerializationError(
    const Oasis::Expression& expr,
    const Oasis::PALMSerializationOpts& opts = {})
{
    const auto expectedError = Oasis::PALMSerializationError {
        .type = Oasis::PALMSerializationError::PALMSerializationErrorType::MissingOperand,
        .expression = &expr,
        .message = "Expression missing operand"
    };

    requireFailedSerialization(expr, expectedError, opts);
}

void requireMissingLeastSignificantOperandSerializationError(
    const Oasis::Expression& expr,
    const Oasis::PALMSerializationOpts& opts = {})
{
    const auto expectedError = Oasis::PALMSerializationError {
        .type = Oasis::PALMSerializationError::PALMSerializationErrorType::MissingOperand,
        .expression = &expr,
        .message = "Expression missing least significant operand"
    };

    requireFailedSerialization(expr, expectedError, opts);
}

void requireMissingMostSignificantOperandSerializationError(
    const Oasis::Expression& expr,
    const Oasis::PALMSerializationOpts& opts = {})
{
    const auto expectedError = Oasis::PALMSerializationError {
        .type = Oasis::PALMSerializationError::PALMSerializationErrorType::MissingOperand,
        .expression = &expr,
        .message = "Expression missing most significant operand"
    };

    requireFailedSerialization(expr, expectedError, opts);
}

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
    const std::string expected = "( undefined )";

    requireSuccessfulSerialization(undefined, expected);
}

// --- Real ---
// Regular Values
TEST_CASE("PALM Serialization of Real", "[PALM][Serializer][Operator][Real]")
{
    const Oasis::Real real { 42.0 };
    const std::string expected = "( real 42 )";

    requireSuccessfulSerialization(real, expected);
}

TEST_CASE("PALM Serialization of Real with decimal", "[PALM][Serializer][Operator][Real]")
{
    const Oasis::Real real { 3.14 };
    const std::string expected = "( real 3.14 )";

    requireSuccessfulSerialization(real, expected);
}

TEST_CASE("PALM Serialization of Real with negative value", "[PALM][Serializer][Operator][Real]")
{
    const Oasis::Real real { -2.71828 };
    const std::string expected = "( real -2.71828 )";

    requireSuccessfulSerialization(real, expected);
}

// Precision Tests
TEST_CASE("PALM Serialization of Real with default precision", "[PALM][Serializer][Operator][Real]")
{
    const Oasis::Real real { 2.718281828459045 };
    const std::string expected = "( real 2.71828 )"; // Default precision is 5 decimal places

    requireSuccessfulSerialization(real, expected);
}

TEST_CASE("PALM Serialization of Real to 2 decimal places", "[PALM][Serializer][Operator][Real]")
{
    const Oasis::Real real { 2.718281828459045 };
    const std::string expected = "( real 2.72 )";
    const Oasis::PALMSerializationOpts opts = { .numPlaces = 2 };

    requireSuccessfulSerialization(real, expected, opts);
}

TEST_CASE("PALM Serialization of Real to 0 decimal places", "[PALM][Serializer][Operator][Real]")
{
    const Oasis::Real real { 2.718281828459045 };
    const std::string expected = "( real 3 )";
    const Oasis::PALMSerializationOpts opts = { .numPlaces = 0 };

    requireSuccessfulSerialization(real, expected, opts);
}

TEST_CASE("PALM Serialization of Real with negative value and 3 decimal places", "[PALM][Serializer][Operator][Real]")
{
    const Oasis::Real real { -2.71828 };
    const std::string expected = "( real -2.718 )";
    const Oasis::PALMSerializationOpts opts = { .numPlaces = 3 };

    requireSuccessfulSerialization(real, expected, opts);
}

TEST_CASE("PALM Serialization of Real with NaN", "[PALM][Serializer][Operator][Real]")
{
    const Oasis::Real real { std::numeric_limits<double>::quiet_NaN() };
    const std::string expected = "( real NaN )";

    requireSuccessfulSerialization(real, expected);
}

TEST_CASE("PALM Serialization of Real with Positive Infinity", "[PALM][Serializer][Operator][Real]")
{
    const Oasis::Real real { std::numeric_limits<double>::infinity() };
    const std::string expected = "( real Infinity )";

    requireSuccessfulSerialization(real, expected);
}

TEST_CASE("PALM Serialization of Real with Negative Infinity", "[PALM][Serializer][Operator][Real]")
{
    const Oasis::Real real { -std::numeric_limits<double>::infinity() };
    const std::string expected = "( real -Infinity )";

    requireSuccessfulSerialization(real, expected);
}

// --- Imaginary ---
// Default Values
TEST_CASE("PALM Serialization of Imaginary", "[PALM][Serializer][Operator][Imaginary]")
{
    const Oasis::Imaginary imaginary {};
    const std::string expected = "( i )";

    requireSuccessfulSerialization(imaginary, expected);
}

// Different Symbols
TEST_CASE("PALM Serialization of Imaginary with 'i' symbol", "[PALM][Serializer][Operator][Imaginary]")
{
    const Oasis::Imaginary imaginary {};
    const std::string expected = "( i )";
    const Oasis::PALMSerializationOpts opts = { .imaginarySymbol = Oasis::PALMSerializationOpts::ImgSymType::I };

    requireSuccessfulSerialization(imaginary, expected, opts);
}

TEST_CASE("PALM Serialization of Imaginary with 'j' symbol", "[PALM][Serializer][Operator][Imaginary]")
{
    const Oasis::Imaginary imaginary {};
    const std::string expected = "( j )";
    const Oasis::PALMSerializationOpts opts = { .imaginarySymbol = Oasis::PALMSerializationOpts::ImgSymType::J };

    requireSuccessfulSerialization(imaginary, expected, opts);
}

// --- Variable ---
// Accepted Identifiers
TEST_CASE("PALM Serialization of Variable with Single Character", "[PALM][Serializer][Operator][Variable]")
{
    const Oasis::Variable variable { "x" };
    const std::string expected = "( var x )";

    requireSuccessfulSerialization(variable, expected);
}

TEST_CASE("PALM Serialization of Variable with Multiple Characters", "[PALM][Serializer][Operator][Variable]")
{
    const Oasis::Variable variable { "velocity" };
    const std::string expected = "( var velocity )";

    requireSuccessfulSerialization(variable, expected);
}

TEST_CASE("PALM Serialization of Variable with Underscore", "[PALM][Serializer][Operator][Variable]")
{
    const Oasis::Variable variable { "x_variable" };
    const std::string expected = "( var x_variable )";

    requireSuccessfulSerialization(variable, expected);
}

// Rejected Identifiers
TEST_CASE("PALM Serialization of Variable with Empty Identifier", "[PALM][Serializer][Operator][Variable]")
{
    const Oasis::Variable variable { "" };

    requireInvalidIdentifierSerializationError(variable, "");
}

TEST_CASE("PALM Serialization of Variable with Invalid Identifier", "[PALM][Serializer][Operator][Variable]")
{
    const Oasis::Variable variable { "123invalid" };

    requireInvalidIdentifierSerializationError(variable, "123invalid");
}

TEST_CASE("PALM Serialization of Variable with Special Characters", "[PALM][Serializer][Operator][Variable]")
{
    const Oasis::Variable variable { "var$!" };

    requireInvalidIdentifierSerializationError(variable, "var$!");
}

TEST_CASE("PALM Serialization of Variable with Space", "[PALM][Serializer][Operator][Variable]")
{
    const Oasis::Variable variable { "my var" };

    requireInvalidIdentifierSerializationError(variable, "my var");
}

TEST_CASE("PALM Serialization of Keyword as Variable", "[PALM][Serializer][Operator][Variable]")
{
    const Oasis::Variable variable { "real" };

    requireInvalidIdentifierSerializationError(variable, "real");
}

// --- Addition ---
// --- Basic Tests ---
TEST_CASE("PALM Serialization of Addition Operator Symbol", "[PALM][Serializer][Operator][Addition]")
{
    const Oasis::Add<> addition {
        Oasis::Real { 1.0 },
        Oasis::Real { 2.0 }
    };
    const std::string expected = "( + ( real 1 ) ( real 2 ) )";

    requireSuccessfulSerialization(addition, expected);
}

TEST_CASE("PALM Serialization of Nested Addition", "[PALM][Serializer][Addition]")
{
    const Oasis::Add<> addition {
        Oasis::Real { 5.0 },
        Oasis::Add {
            Oasis::Real { 3.0 },
            Oasis::Real { 2.0 } }
    };
    const std::string expected = "( + ( real 5 ) ( + ( real 3 ) ( real 2 ) ) )";

    requireSuccessfulSerialization(addition, expected);
}

// --- Malformed Tests ---
TEST_CASE("PALM Serialization of Addition Missing Least Significant Operand", "[PALM][Serializer][Addition]")
{
    Oasis::Add<Oasis::Real, Oasis::Expression> addition;
    addition.SetMostSigOp(Oasis::Real { 5.0 });

    requireMissingLeastSignificantOperandSerializationError(addition);
}

TEST_CASE("PALM Serialization of Addition Missing Most Significant Operand", "[PALM][Serializer][Addition]")
{
    Oasis::Add<Oasis::Expression, Oasis::Real> addition;
    addition.SetLeastSigOp(Oasis::Real { 3.0 });

    requireMissingMostSignificantOperandSerializationError(addition);
}

// --- Subtraction ---
// --- Basic Tests ---
TEST_CASE("PALM Serialization of Subtraction", "[PALM][Serializer][Subtraction]")
{
    const Oasis::Subtract<> subtraction {
        Oasis::Real { 5.0 },
        Oasis::Real { 3.0 }
    };
    const std::string expected = "( - ( real 5 ) ( real 3 ) )";

    requireSuccessfulSerialization(subtraction, expected);
}

TEST_CASE("PALM Serialization of Nested Subtraction", "[PALM][Serializer][Subtraction]")
{
    const Oasis::Subtract<> subtraction {
        Oasis::Real { 5.0 },
        Oasis::Subtract {
            Oasis::Real { 3.0 },
            Oasis::Real { 2.0 } }
    };
    const std::string expected = "( - ( real 5 ) ( - ( real 3 ) ( real 2 ) ) )";

    requireSuccessfulSerialization(subtraction, expected);
}

// --- Malformed Tests ---
TEST_CASE("PALM Serialization of Subtraction Missing Least Significant Operand", "[PALM][Serializer][Subtraction]")
{
    Oasis::Subtract<Oasis::Real, Oasis::Expression> subtraction;
    subtraction.SetMostSigOp(Oasis::Real { 5.0 });

    requireMissingLeastSignificantOperandSerializationError(subtraction);
}

TEST_CASE("PALM Serialization of Subtraction Missing Most Significant Operand", "[PALM][Serializer][Subtraction]")
{
    Oasis::Subtract<Oasis::Expression, Oasis::Real> subtraction;
    subtraction.SetLeastSigOp(Oasis::Real { 3.0 });

    requireMissingMostSignificantOperandSerializationError(subtraction);
}

// --- Multiplication ---
// --- Basic Tests ---
TEST_CASE("PALM Serialization of Multiplication", "[PALM][Serializer][Multiplication]")
{
    const Oasis::Multiply<> multiplication {
        Oasis::Real { 5.0 },
        Oasis::Real { 3.0 }
    };
    const std::string expected = "( * ( real 5 ) ( real 3 ) )";

    requireSuccessfulSerialization(multiplication, expected);
}

TEST_CASE("PALM Serialization of Nested Multiplication", "[PALM][Serializer][Multiplication]")
{
    const Oasis::Multiply<> multiplication {
        Oasis::Real { 5.0 },
        Oasis::Multiply {
            Oasis::Real { 3.0 },
            Oasis::Real { 2.0 } }
    };
    const std::string expected = "( * ( real 5 ) ( * ( real 3 ) ( real 2 ) ) )";

    requireSuccessfulSerialization(multiplication, expected);
}

// --- Malformed Tests ---
TEST_CASE("PALM Serialization of Multiplication Missing Least Significant Operand", "[PALM][Serializer][Multiplication]")
{
    Oasis::Multiply<Oasis::Real, Oasis::Expression> multiplication;
    multiplication.SetMostSigOp(Oasis::Real { 5.0 });

    requireMissingLeastSignificantOperandSerializationError(multiplication);
}

TEST_CASE("PALM Serialization of Multiplication Missing Most Significant Operand", "[PALM][Serializer][Multiplication]")
{
    Oasis::Multiply<Oasis::Expression, Oasis::Real> multiplication;
    multiplication.SetLeastSigOp(Oasis::Real { 3.0 });

    requireMissingMostSignificantOperandSerializationError(multiplication);
}

// --- Division ---
// --- Basic Tests ---
TEST_CASE("PALM Serialization of Division", "[PALM][Serializer][Division]")
{
    const Oasis::Divide<> division {
        Oasis::Real { 6.0 },
        Oasis::Real { 3.0 }
    };
    const std::string expected = "( / ( real 6 ) ( real 3 ) )";

    requireSuccessfulSerialization(division, expected);
}

TEST_CASE("PALM Serialization of Nested Division", "[PALM][Serializer][Division]")
{
    const Oasis::Divide<> division {
        Oasis::Real { 6.0 },
        Oasis::Divide {
            Oasis::Real { 3.0 },
            Oasis::Real { 2.0 } }
    };
    const std::string expected = "( / ( real 6 ) ( / ( real 3 ) ( real 2 ) ) )";

    requireSuccessfulSerialization(division, expected);
}

// --- Malformed Tests ---
TEST_CASE("PALM Serialization of Division Missing Least Significant Operand", "[PALM][Serializer][Division]")
{
    Oasis::Divide<Oasis::Real, Oasis::Expression> division;
    division.SetMostSigOp(Oasis::Real { 6.0 });

    requireMissingLeastSignificantOperandSerializationError(division);
}

TEST_CASE("PALM Serialization of Division Missing Most Significant Operand", "[PALM][Serializer][Division]")
{
    Oasis::Divide<Oasis::Expression, Oasis::Real> division;
    division.SetLeastSigOp(Oasis::Real { 3.0 });

    requireMissingMostSignificantOperandSerializationError(division);
}

// --- Exponentiation ---
// --- Basic Tests ---
TEST_CASE("PALM Serialization of Exponentiation", "[PALM][Serializer][Exponentiation]")
{
    const Oasis::Exponent<> exponent {
        Oasis::Real { 2.0 },
        Oasis::Real { 3.0 }
    };
    const std::string expected = "( ^ ( real 2 ) ( real 3 ) )";

    requireSuccessfulSerialization(exponent, expected);
}

TEST_CASE("PALM Serialization of Nested Exponentiation", "[PALM][Serializer][Exponentiation]")
{
    const Oasis::Exponent<> exponent {
        Oasis::Real { 2.0 },
        Oasis::Exponent {
            Oasis::Real { 3.0 },
            Oasis::Real { 2.0 } }
    };
    const std::string expected = "( ^ ( real 2 ) ( ^ ( real 3 ) ( real 2 ) ) )";

    requireSuccessfulSerialization(exponent, expected);
}

// --- Malformed Tests ---
TEST_CASE("PALM Serialization of Exponentiation Missing Least Significant Operand", "[PALM][Serializer][Exponentiation]")
{
    Oasis::Exponent<Oasis::Real, Oasis::Expression> exponent;
    exponent.SetMostSigOp(Oasis::Real { 2.0 });

    requireMissingLeastSignificantOperandSerializationError(exponent);
}

TEST_CASE("PALM Serialization of Exponentiation Missing Most Significant Operand", "[PALM][Serializer][Exponentiation]")
{
    Oasis::Exponent<Oasis::Expression, Oasis::Real> exponent;
    exponent.SetLeastSigOp(Oasis::Real { 3.0 });

    requireMissingMostSignificantOperandSerializationError(exponent);
}

// --- Logarithm ---
// --- Basic Tests ---
TEST_CASE("PALM Serialization of Logarithm", "[PALM][Serializer][Logarithm]")
{
    const Oasis::Log<> log {
        Oasis::Real { 10.0 },
        Oasis::Real { 1000.0 }
    };
    const std::string expected = "( log ( real 10 ) ( real 1000 ) )";

    requireSuccessfulSerialization(log, expected);
}

TEST_CASE("PALM Serialization of Nested Logarithm", "[PALM][Serializer][Logarithm]")
{
    const Oasis::Log<> log {
        Oasis::Real { 10.0 },
        Oasis::Log {
            Oasis::Real { 10.0 },
            Oasis::Real { 1000.0 } }
    };
    const std::string expected = "( log ( real 10 ) ( log ( real 10 ) ( real 1000 ) ) )";

    requireSuccessfulSerialization(log, expected);
}

// --- Malformed Tests ---
TEST_CASE("PALM Serialization of Logarithm Missing Least Significant Operand", "[PALM][Serializer][Logarithm]")
{
    Oasis::Log<Oasis::Real, Oasis::Expression> log;
    log.SetMostSigOp(Oasis::Real { 10.0 });

    requireMissingLeastSignificantOperandSerializationError(log);
}

TEST_CASE("PALM Serialization of Logarithm Missing Most Significant Operand", "[PALM][Serializer][Logarithm]")
{
    Oasis::Log<Oasis::Expression, Oasis::Real> log;
    log.SetLeastSigOp(Oasis::Real { 1000.0 });

    requireMissingMostSignificantOperandSerializationError(log);
}

// --- Negation ---
// --- Basic Tests ---
TEST_CASE("PALM Serialization of Negation", "[PALM][Serializer][Negation]")
{
    const Oasis::Negate<> negate {
        Oasis::Real { 5.0 }
    };
    const std::string expected = "( neg ( real 5 ) )";

    requireSuccessfulSerialization(negate, expected);
}

TEST_CASE("PALM Serialization of Nested Negation", "[PALM][Serializer][Negation]")
{
    const Oasis::Negate<> negate {
        Oasis::Negate {
            Oasis::Real { 5.0 } }
    };
    const std::string expected = "( neg ( neg ( real 5 ) ) )";

    requireSuccessfulSerialization(negate, expected);
}

// --- Malformed Tests ---
TEST_CASE("PALM Serialization of Negation Missing Operand", "[PALM][Serializer][Negation]")
{
    const Oasis::Negate<> negate;

    requireMissingOperandSerializationError(negate);
}

// --- Derivative ---
// --- Basic Tests ---
TEST_CASE("PALM Serialization of Derivative", "[PALM][Serializer][Derivative]")
{
    const Oasis::Derivative<> derivative {
        Oasis::Variable { "x" },
        Oasis::Real { 5.0 }
    };
    const std::string expected = "( d ( var x ) ( real 5 ) )";

    requireSuccessfulSerialization(derivative, expected);
}

TEST_CASE("PALM Serialization of Nested Derivative", "[PALM][Serializer][Derivative]")
{
    const Oasis::Derivative<> derivative {
        Oasis::Variable { "x" },
        Oasis::Derivative {
            Oasis::Variable { "y" },
            Oasis::Real { 5.0 } }
    };
    const std::string expected = "( d ( var x ) ( d ( var y ) ( real 5 ) ) )";

    requireSuccessfulSerialization(derivative, expected);
}

// --- Malformed Tests ---
TEST_CASE("PALM Serialization of Derivative Missing Least Significant Operand", "[PALM][Serializer][Derivative]")
{
    Oasis::Derivative<Oasis::Real, Oasis::Expression> derivative;
    derivative.SetMostSigOp(Oasis::Real { 5.0 });

    requireMissingLeastSignificantOperandSerializationError(derivative);
}

TEST_CASE("PALM Serialization of Derivative Missing Most Significant Operand", "[PALM][Serializer][Derivative]")
{
    Oasis::Derivative<Oasis::Expression, Oasis::Real> derivative;
    derivative.SetLeastSigOp(Oasis::Real { 5.0 });

    requireMissingMostSignificantOperandSerializationError(derivative);
}

// --- Integral ---
// --- Basic Tests ---
TEST_CASE("PALM Serialization of Integral", "[PALM][Serializer][Integral]")
{
    const Oasis::Integral<> integral {
        Oasis::Variable { "x" },
        Oasis::Real { 5.0 }
    };
    const std::string expected = "( int ( var x ) ( real 5 ) )";

    requireSuccessfulSerialization(integral, expected);
}

TEST_CASE("PALM Serialization of Nested Integral", "[PALM][Serializer][Integral]")
{
    const Oasis::Integral<> integral {
        Oasis::Variable { "x" },
        Oasis::Integral {
            Oasis::Variable { "y" },
            Oasis::Real { 5.0 } }
    };
    const std::string expected = "( int ( var x ) ( int ( var y ) ( real 5 ) ) )";

    requireSuccessfulSerialization(integral, expected);
}

// --- Malformed Tests ---
TEST_CASE("PALM Serialization of Integral Missing Least Significant Operand", "[PALM][Serializer][Integral]")
{
    Oasis::Integral<Oasis::Variable, Oasis::Expression> integral;
    integral.SetMostSigOp(Oasis::Variable { "x" });

    requireMissingLeastSignificantOperandSerializationError(integral);
}

TEST_CASE("PALM Serialization of Integral Missing Most Significant Operand", "[PALM][Serializer][Integral]")
{
    Oasis::Integral<Oasis::Expression, Oasis::Real> integral;
    integral.SetLeastSigOp(Oasis::Real { 5.0 });

    requireMissingMostSignificantOperandSerializationError(integral);
}

// TODO: Implement Matrix

// --- Euler Number ---
TEST_CASE("PALM Serialization of Euler Number", "[PALM][Serializer][EulerNumber]")
{
    const Oasis::EulerNumber euler {};
    const std::string expected = "( e )";

    requireSuccessfulSerialization(euler, expected);
}

// --- Pi ---
TEST_CASE("PALM Serialization of Pi", "[PALM][Serializer][Pi]")
{
    const Oasis::Pi pi {};
    const std::string expected = "( pi )";

    requireSuccessfulSerialization(pi, expected);
}

// --- Magnitude ---
// --- Basic Tests ---
TEST_CASE("PALM Serialization of Magnitude", "[PALM][Serializer][Magnitude]")
{
    const Oasis::Magnitude magnitude {
        Oasis::Real { -5.0 }
    };
    const std::string expected = "( magnitude ( real -5 ) )";

    requireSuccessfulSerialization(magnitude, expected);
}

TEST_CASE("PALM Serialization of Nested Magnitude", "[PALM][Serializer][Magnitude]")
{
    const Oasis::Magnitude<Oasis::Magnitude<Oasis::Real>> magnitude {
        Oasis::Magnitude {
            Oasis::Real { -5.0 } }
    };
    const std::string expected = "( magnitude ( magnitude ( real -5 ) ) )";

    requireSuccessfulSerialization(magnitude, expected);
}

// --- Malformed Tests ---
TEST_CASE("PALM Serialization of Magnitude Missing Operand", "[PALM][Serializer][Magnitude]")
{
    const Oasis::Magnitude<Oasis::Expression> magnitude;

    requireMissingOperandSerializationError(magnitude);
}

// -------------------------
// Compound Expression Tests
// -------------------------

// Basic Compound Expression Tests
TEST_CASE("PALM Serialization of Compound Expression Add Multiply Exponent", "[PALM][Serializer][CompoundExpression]")
{
    auto serializer = Oasis::PALMSerializer();

    const Oasis::Add<> expression {
        Oasis::Real { 5.0 },
        Oasis::Multiply {
            Oasis::Variable { "x" },
            Oasis::Exponent {
                Oasis::Real { 2.0 },
                Oasis::Real { 3.0 } } }
    };
    const std::string expected = "( + ( real 5 ) ( * ( var x ) ( ^ ( real 2 ) ( real 3 ) ) ) )";

    requireSuccessfulSerialization(expression, expected);
}

TEST_CASE("PALM Serialization of Compound Expression Subtract Divide Log", "[PALM][Serializer][CompoundExpression]")
{
    const Oasis::Subtract<> expression {
        Oasis::Divide {
            Oasis::Real { 10.0 },
            Oasis::Real { 2.0 } },
        Oasis::Log {
            Oasis::Real { 10.0 },
            Oasis::Real { 100.0 } }
    };
    const std::string expected = "( - ( / ( real 10 ) ( real 2 ) ) ( log ( real 10 ) ( real 100 ) ) )";

    requireSuccessfulSerialization(expression, expected);
}

// ---------------------------
// Padding and Separator Tests
// ---------------------------

// Expression Padding Tests
TEST_CASE("PALM Serialization with No Expression Padding", "[PALM][Serializer][NoPadding]")
{
    const Oasis::Add<> expression {
        Oasis::Real { 5.0 },
        Oasis::Multiply {
            Oasis::Variable { "x" },
            Oasis::Exponent {
                Oasis::Real { 2.0 },
                Oasis::Real { 3.0 } } }
    };
    const std::string expected = "(+ (real 5) (* (var x) (^ (real 2) (real 3))))";
    const Oasis::PALMSerializationOpts opts = { .expressionPadding = "" };

    requireSuccessfulSerialization(expression, expected, opts);
}

TEST_CASE("PALM Serialization with Space Expression Padding", "[PALM][Serializer][SpacePadding]")
{
    const Oasis::Add<> expression {
        Oasis::Real { 5.0 },
        Oasis::Multiply {
            Oasis::Variable { "x" },
            Oasis::Exponent {
                Oasis::Real { 2.0 },
                Oasis::Real { 3.0 } } }
    };
    const std::string expected = "( + ( real 5 ) ( * ( var x ) ( ^ ( real 2 ) ( real 3 ) ) ) )";
    const Oasis::PALMSerializationOpts opts = { .expressionPadding = " " };

    requireSuccessfulSerialization(expression, expected, opts);
}

TEST_CASE("PALM Serialization with Hyphen Expression Padding", "[PALM][Serializer][HyphenPadding]")
{
    const Oasis::Add<> expression {
        Oasis::Real { 5.0 },
        Oasis::Multiply {
            Oasis::Variable { "x" },
            Oasis::Exponent {
                Oasis::Real { 2.0 },
                Oasis::Real { 3.0 } } }
    };
    const std::string expected = "(-+ (-real 5-) (-* (-var x-) (-^ (-real 2-) (-real 3-)-)-)-)";
    const Oasis::PALMSerializationOpts opts = { .expressionPadding = "-" };

    requireSuccessfulSerialization(expression, expected, opts);
}

TEST_CASE("PALM Serialization with Tab Padding", "[PALM][Serializer][TabPadding]")
{
    Oasis::PALMSerializer serializer({ .expressionPadding = "\t" });

    const Oasis::Add<> expression {
        Oasis::Real { 5.0 },
        Oasis::Multiply {
            Oasis::Variable { "x" },
            Oasis::Exponent {
                Oasis::Real { 2.0 },
                Oasis::Real { 3.0 } } }
    };
    const std::string expected = "(\t+ (\treal 5\t) (\t* (\tvar x\t) (\t^ (\treal 2\t) (\treal 3\t)\t)\t)\t)";
    const Oasis::PALMSerializationOpts opts = { .expressionPadding = "\t" };

    requireSuccessfulSerialization(expression, expected, opts);
}

TEST_CASE("PALM Serialization with %bruh% Padding", "[PALM][Serializer][BruhPadding]")
{
    const Oasis::Add<> expression {
        Oasis::Real { 5.0 },
        Oasis::Multiply {
            Oasis::Variable { "x" },
            Oasis::Exponent {
                Oasis::Real { 2.0 },
                Oasis::Real { 3.0 } } }
    };
    const std::string expected = "(%bruh%+ (%bruh%real 5%bruh%) (%bruh%* (%bruh%var x%bruh%) (%bruh%^ (%bruh%real 2%bruh%) (%bruh%real 3%bruh%)%bruh%)%bruh%)%bruh%)";
    const Oasis::PALMSerializationOpts opts = { .expressionPadding = "%bruh%" };

    requireSuccessfulSerialization(expression, expected, opts);
}

// Token Separator Tests
TEST_CASE("PALM Serialization with Double Space Separator", "[PALM][Serializer][DoubleSpaceSeparator]")
{
    const Oasis::Add<> expression {
        Oasis::Real { 5.0 },
        Oasis::Multiply {
            Oasis::Variable { "x" },
            Oasis::Exponent {
                Oasis::Real { 2.0 },
                Oasis::Real { 3.0 } } }
    };
    const std::string expected = "( +  ( real  5 )  ( *  ( var  x )  ( ^  ( real  2 )  ( real  3 ) ) ) )";
    const Oasis::PALMSerializationOpts opts = { .tokenSeparator = "  " };

    requireSuccessfulSerialization(expression, expected, opts);
}

TEST_CASE("PALM Serialization with Percent Separator", "[PALM][Serializer][PercentSeparator]")
{
    const Oasis::Add<> expression {
        Oasis::Real { 5.0 },
        Oasis::Multiply {
            Oasis::Variable { "x" },
            Oasis::Exponent {
                Oasis::Real { 2.0 },
                Oasis::Real { 3.0 } } }
    };
    const std::string expected = "( +%( real%5 )%( *%( var%x )%( ^%( real%2 )%( real%3 ) ) ) )";
    const Oasis::PALMSerializationOpts opts = { .tokenSeparator = "%" };

    requireSuccessfulSerialization(expression, expected, opts);
}

// ==================
// PALM Parsing Tests
// ==================

// ----------------
// Helper Functions
// ----------------
// Success
void requireSuccessfulParsing(
    const std::string& palm,
    const Oasis::Expression& expectedExpr)
{
    const auto expr = Oasis::FromPALM(palm);

    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expectedExpr));
}

// Failure
void requireFailedParsing(
    const std::string& palm,
    const Oasis::PALMParseError& expectedError)
{
    const auto expr = Oasis::FromPALM(palm);

    REQUIRE_FALSE(expr.has_value());

    const auto& error = expr.error();
    CAPTURE(error.message, expectedError.message);
    REQUIRE(error == expectedError);
}

// --- Specific Error Types ---
void requireExtraOperandParsingError(
    const std::string& palm,
    const Oasis::PALMToken& errorToken)
{
    const auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::ExtraOperands,
        .token = errorToken,
        .message = "Unexpected token '" + errorToken.lexeme + "' found after parsing expression"
    };

    requireFailedParsing(palm, expectedError);
}

void requireMissingOperandParsingError(
    const std::string& palm,
    const Oasis::PALMToken& errorToken)
{
    const auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::MissingOperand,
        .token = errorToken,
        .message = "Expected operand after token '" + errorToken.lexeme + "'"
    };

    requireFailedParsing(palm, expectedError);
}

void requireInvalidNumberFormatParsingError(
    const std::string& palm,
    const Oasis::PALMToken& errorToken)
{
    const auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidNumberFormat,
        .token = errorToken,
        .message = "Invalid number format: '" + errorToken.lexeme + "'"
    };

    requireFailedParsing(palm, expectedError);
}

void requireNumberOutOfBoundsParsingError(
    const std::string& palm,
    const Oasis::PALMToken& errorToken)
{
    const auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::NumberOutOfBounds,
        .token = errorToken,
        .message = "Number out of bounds: '" + errorToken.lexeme + "'"
    };

    requireFailedParsing(palm, expectedError);
}

void requireInvalidVariableNameParsingError(
    const std::string& palm,
    const Oasis::PALMToken& errorToken)
{
    const auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidVariableName,
        .token = errorToken,
        .message = "Invalid identifier: '" + errorToken.lexeme + "'"
    };

    requireFailedParsing(palm, expectedError);
}

void requireInvalidOperandParsingError(
    const std::string& palm,
    const Oasis::PALMToken& errorToken)
{
    const auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidOperand,
        .token = errorToken,
        .message = "Invalid operand: '" + errorToken.lexeme + "'. Expected an expression."
    };

    requireFailedParsing(palm, expectedError);
}

void requireMissingOpeningParenthesisParsingError(
    const std::string& palm,
    const Oasis::PALMToken& errorToken)
{
    const auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::MissingOpeningParenthesis,
        .token = errorToken,
        .message = "Expected '(' before expression starting with token '" + errorToken.lexeme + "'"
    };

    requireFailedParsing(palm, expectedError);
}

void requireMissingClosingParenthesisParsingError(
    const std::string& palm,
    const Oasis::PALMToken& errorToken)
{
    const auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::MissingClosingParenthesis,
        .token = errorToken,
        .message = "Expected ')' to close expression starting with token '" + errorToken.lexeme + "'"
    };

    requireFailedParsing(palm, expectedError);
}

void requireInvalidOperatorParsingError(
    const std::string& palm,
    const Oasis::PALMToken& errorToken)
{
    const auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidOperator,
        .token = errorToken,
        .message = "Invalid operator: '" + errorToken.lexeme + "'"
    };

    requireFailedParsing(palm, expectedError);
}

void requireUnexpectedEndOfInputParsingError(
    const std::string& palm,
    const Oasis::PALMToken& errorToken)
{
    const auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::UnexpectedEndOfInput,
        .token = errorToken,
        .message = "Unexpected end of input while parsing expression"
    };

    requireFailedParsing(palm, expectedError);
}

void requireMissingOperatorParsingError(
    const std::string& palm,
    const Oasis::PALMToken& errorToken)
{
    const auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::MissingOperator,
        .token = errorToken,
        .message = "Expected operator after token '" + errorToken.lexeme + "'"
    };

    requireFailedParsing(palm, expectedError);
}

void requireUnexpectedTokenParsingError(
    const std::string& palm,
    const Oasis::PALMToken& errorToken)
{
    const auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::UnexpectedToken,
        .token = errorToken,
        .message = "Unexpected token: '" + errorToken.lexeme + "'"
    };

    requireFailedParsing(palm, expectedError);
}

// --- Operator Mapping ---
// Trait mapping operator class → symbol string
template <typename T>
struct OperatorSymbol;

// Binary Operators
template <>
struct OperatorSymbol<Oasis::Add<>> {
    static constexpr auto value = "+";
};
template <>
struct OperatorSymbol<Oasis::Subtract<>> {
    static constexpr auto value = "-";
};
template <>
struct OperatorSymbol<Oasis::Multiply<>> {
    static constexpr auto value = "*";
};
template <>
struct OperatorSymbol<Oasis::Divide<>> {
    static constexpr auto value = "/";
};
template <>
struct OperatorSymbol<Oasis::Exponent<>> {
    static constexpr auto value = "^";
};
template <>
struct OperatorSymbol<Oasis::Log<>> {
    static constexpr auto value = "log";
};
template <>
struct OperatorSymbol<Oasis::Integral<>> {
    static constexpr auto value = "int";
};
template <>
struct OperatorSymbol<Oasis::Derivative<>> {
    static constexpr auto value = "d";
};

// Unary Operators
template <>
struct OperatorSymbol<Oasis::Negate<>> {
    static constexpr auto value = "neg";
};
template <>
struct OperatorSymbol<Oasis::Magnitude<>> {
    static constexpr auto value = "magnitude";
};

// Leaf Operators
template <>
struct OperatorSymbol<Oasis::Undefined> {
    static constexpr auto value = "undefined";
};
template <>
struct OperatorSymbol<Oasis::Imaginary> {
    static constexpr auto value = "i";
};
template <>
struct OperatorSymbol<Oasis::Pi> {
    static constexpr auto value = "pi";
};
template <>
struct OperatorSymbol<Oasis::EulerNumber> {
    static constexpr auto value = "e";
};

// ------------------
// Binary Expressions
// ------------------

// --- Basic Tests ---
TEMPLATE_TEST_CASE(
    "PALM Parsing of Binary Expressions",
    "[PALM][Parser][Operator][Template]",
    Oasis::Add<>,
    Oasis::Subtract<>,
    Oasis::Multiply<>,
    Oasis::Divide<>,
    Oasis::Exponent<>,
    Oasis::Log<>,
    Oasis::Integral<>,
    Oasis::Derivative<>)
{
    const TestType expected {
        Oasis::Real { 3.0 },
        Oasis::Real { 5.0 }
    };

    const std::string op = OperatorSymbol<TestType>::value;
    const std::string palm = "( " + op + " ( real 3 ) ( real 5 ) )";
    requireSuccessfulParsing(palm, expected);
}

TEMPLATE_TEST_CASE(
    "PALM Parsing of Nested Binary Expressions",
    "[PALM][Parser][Operator][Template][Nested]",
    Oasis::Add<>,
    Oasis::Subtract<>,
    Oasis::Multiply<>,
    Oasis::Divide<>,
    Oasis::Exponent<>,
    Oasis::Log<>,
    Oasis::Integral<>,
    Oasis::Derivative<>)
{
    const TestType expected {
        Oasis::Real { 5.0 },
        TestType {
            Oasis::Real { 3.0 },
            Oasis::Real { 2.0 } }
    };

    const std::string op = OperatorSymbol<TestType>::value;
    const std::string palm = "( " + op + " ( real 5 ) ( " + op + " ( real 3 ) ( real 2 ) ) )";
    requireSuccessfulParsing(palm, expected);
}

// --- Malformed Tests ---
TEMPLATE_TEST_CASE(
    "PALM Parsing of Binary Expressions Missing Least Significant Operand",
    "[PALM][Parser][Operator][Template][Malformed]",
    Oasis::Add<>,
    Oasis::Subtract<>,
    Oasis::Multiply<>,
    Oasis::Divide<>,
    Oasis::Exponent<>,
    Oasis::Log<>,
    Oasis::Integral<>,
    Oasis::Derivative<>)
{
    const std::string op = OperatorSymbol<TestType>::value;
    const std::string palm = "( " + op + " ( real 5 ) )";
    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::Punctuator,
        .lexeme = ")",
        .charOffset = palm.size() - 1,
        .tokenIndex = 7,
        .line = 1,
        .column = palm.size(),
    };

    requireMissingOperandParsingError(palm, errorToken);
}

TEMPLATE_TEST_CASE(
    "PALM Parsing of Binary Expressions Missing Most Significant Operand",
    "[PALM][Parser][Operator][Template][Malformed]",
    Oasis::Add<>,
    Oasis::Subtract<>,
    Oasis::Multiply<>,
    Oasis::Divide<>,
    Oasis::Exponent<>,
    Oasis::Log<>,
    Oasis::Integral<>,
    Oasis::Derivative<>)
{
    const std::string op = OperatorSymbol<TestType>::value;
    const std::string palm = "( " + op + " )";
    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::Punctuator,
        .lexeme = ")",
        .charOffset = palm.size() - 1,
        .tokenIndex = 3,
        .line = 1,
        .column = palm.size(),
    };

    requireMissingOperandParsingError(palm, errorToken);
}

TEMPLATE_TEST_CASE(
    "PALM Parsing of Binary Expressions with Extra Tokens",
    "[PALM][Parser][Operator][Template][Malformed]",
    Oasis::Add<>,
    Oasis::Subtract<>,
    Oasis::Multiply<>,
    Oasis::Divide<>,
    Oasis::Exponent<>,
    Oasis::Log<>,
    Oasis::Integral<>,
    Oasis::Derivative<>)
{
    const std::string op = OperatorSymbol<TestType>::value;
    const std::string palm = "( " + op + " ( real 3 ) ( real 5 ) extra_token )";
    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::Identifier,
        .lexeme = "extra_token",
        .charOffset = palm.size() - 13,
        .tokenIndex = 8,
        .line = 1,
        .column = palm.size() - 12,
    };

    requireExtraOperandParsingError(palm, errorToken);
}

TEMPLATE_TEST_CASE(
    "PALM Parsing of Binary Expressions with Identifier Operand",
    "[PALM][Parser][Operator][Template][Malformed]",
    Oasis::Add<>,
    Oasis::Subtract<>,
    Oasis::Multiply<>,
    Oasis::Divide<>,
    Oasis::Exponent<>,
    Oasis::Log<>,
    Oasis::Integral<>,
    Oasis::Derivative<>)
{
    const std::string op = OperatorSymbol<TestType>::value;
    const std::string palm = "( " + op + " ( real 3 ) x )";

    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::Identifier,
        .lexeme = "x",
        .charOffset = palm.size() - 2,
        .tokenIndex = 5,
        .line = 1,
        .column = palm.size() - 1,
    };

    return requireInvalidOperandParsingError(palm, errorToken);
}

TEMPLATE_TEST_CASE(
    "PALM Parsing of Binary Expressions with Number Operand",
    "[PALM][Parser][Operator][Template][Malformed]",
    Oasis::Add<>,
    Oasis::Subtract<>,
    Oasis::Multiply<>,
    Oasis::Divide<>,
    Oasis::Exponent<>,
    Oasis::Log<>,
    Oasis::Integral<>,
    Oasis::Derivative<>)
{
    const std::string op = OperatorSymbol<TestType>::value;
    const std::string palm = "( " + op + " ( real 3 ) 42 )";

    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::Number,
        .lexeme = "42",
        .charOffset = palm.size() - 3,
        .tokenIndex = 5,
        .line = 1,
        .column = palm.size() - 2,
    };

    return requireInvalidOperandParsingError(palm, errorToken);
}

TEMPLATE_TEST_CASE(
    "PALM Parsing of Binary Expressions Missing Opening Parentheses",
    "[PALM][Parser][Operator][Template][Malformed]",
    Oasis::Add<>,
    Oasis::Subtract<>,
    Oasis::Multiply<>,
    Oasis::Divide<>,
    Oasis::Exponent<>,
    Oasis::Log<>,
    Oasis::Integral<>,
    Oasis::Derivative<>)
{
    const std::string op = OperatorSymbol<TestType>::value;
    const std::string palm = " " + op + " ( real 3 ) ( real 5 ) )";

    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::Identifier,
        .lexeme = op,
        .charOffset = 1,
        .tokenIndex = 0,
        .line = 1,
        .column = 2,
    };

    requireMissingOpeningParenthesisParsingError(palm, errorToken);
}

TEMPLATE_TEST_CASE(
    "PALM Parsing of Binary Expressions Missing Closing Parentheses",
    "[PALM][Parser][Operator][Template][Malformed]",
    Oasis::Add<>,
    Oasis::Subtract<>,
    Oasis::Multiply<>,
    Oasis::Divide<>,
    Oasis::Exponent<>,
    Oasis::Log<>,
    Oasis::Integral<>,
    Oasis::Derivative<>)
{
    const std::string op = OperatorSymbol<TestType>::value;
    const std::string palm = "( " + op + " ( real 3 ) ( real 5 ) ";

    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::EndOfInput,
        .lexeme = "",
        .charOffset = palm.size(),
        .tokenIndex = 7,
        .line = 1,
        .column = palm.size() + 1,
    };

    requireMissingClosingParenthesisParsingError(palm, errorToken);
}

// -----------------
// Unary Expressions
// -----------------

// --- Basic Tests ---
TEMPLATE_TEST_CASE(
    "PALM Parsing of Unary Expressions",
    "[PALM][Parser][Operator][Template][Unary]",
    Oasis::Negate<>,
    Oasis::Magnitude<>)
{
    const TestType expected {
        Oasis::Real { -7.0 }
    };

    const std::string op = OperatorSymbol<TestType>::value;
    const std::string palm = "( " + op + " ( real -7 ) )";

    requireSuccessfulParsing(palm, expected);
}

TEMPLATE_TEST_CASE(
    "PALM Parsing of Nested Unary Expressions",
    "[PALM][Parser][Operator][Template][Unary][Nested]",
    Oasis::Negate<>,
    Oasis::Magnitude<>)
{
    const TestType expected {
        TestType {
            Oasis::Real { -7.0 } }
    };

    const std::string op = OperatorSymbol<TestType>::value;
    const std::string palm = "( " + op + " ( " + op + " ( real -7 ) ) )";

    requireSuccessfulParsing(palm, expected);
}

// --- Malformed Tests ---
TEMPLATE_TEST_CASE(
    "PALM Parsing of Unary Expressions Missing Operand",
    "[PALM][Parser][Operator][Template][Unary][Malformed]",
    Oasis::Negate<>,
    Oasis::Magnitude<>)
{
    const std::string op = OperatorSymbol<TestType>::value;
    const std::string palm = "( " + op + " )";

    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::Punctuator,
        .lexeme = ")",
        .charOffset = palm.size() - 1,
        .tokenIndex = 3,
        .line = 1,
        .column = palm.size(),
    };

    requireMissingOperandParsingError(palm, errorToken);
}

TEMPLATE_TEST_CASE(
    "PALM Parsing of Unary Expressions with Extra Tokens",
    "[PALM][Parser][Operator][Template][Unary][Malformed]",
    Oasis::Negate<>,
    Oasis::Magnitude<>)
{
    const std::string op = OperatorSymbol<TestType>::value;
    const std::string palm = "( " + op + " ( real -7 ) extra_token )";

    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::Identifier,
        .lexeme = "extra_token",
        .charOffset = palm.size() - 13,
        .tokenIndex = 5,
        .line = 1,
        .column = palm.size() - 12,
    };

    requireExtraOperandParsingError(palm, errorToken);
}

TEMPLATE_TEST_CASE(
    "PALM Parsing of Unary Expressions with Number Operand",
    "[PALM][Parser][Operator][Template][Unary][Malformed]",
    Oasis::Negate<>,
    Oasis::Magnitude<>)
{
    const std::string op = OperatorSymbol<TestType>::value;
    const std::string palm = "( " + op + " 42 )";

    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::Number,
        .lexeme = "42",
        .charOffset = palm.size() - 3,
        .tokenIndex = 4,
        .line = 1,
        .column = palm.size() - 2,
    };

    return requireInvalidOperandParsingError(palm, errorToken);
}

TEMPLATE_TEST_CASE(
    "PALM Parsing of Unary Expressions with Identifier Operand",
    "[PALM][Parser][Operator][Template][Unary][Malformed]",
    Oasis::Negate<>,
    Oasis::Magnitude<>)
{
    const std::string op = OperatorSymbol<TestType>::value;
    const std::string palm = "( " + op + " x )";

    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::Identifier,
        .lexeme = "x",
        .charOffset = palm.size() - 2,
        .tokenIndex = 4,
        .line = 1,
        .column = palm.size() - 1,
    };

    return requireInvalidOperandParsingError(palm, errorToken);
}

TEMPLATE_TEST_CASE(
    "PALM Parsing of Unary Expressions Missing Opening Parentheses",
    "[PALM][Parser][Operator][Template][Unary][Malformed]",
    Oasis::Negate<>,
    Oasis::Magnitude<>)
{
    const std::string op = OperatorSymbol<TestType>::value;
    const std::string palm = " " + op + " ( real -7 ) )";

    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::Identifier,
        .lexeme = op,
        .charOffset = 1,
        .tokenIndex = 0,
        .line = 1,
        .column = 2,
    };

    requireMissingOpeningParenthesisParsingError(palm, errorToken);
}

TEMPLATE_TEST_CASE(
    "PALM Parsing of Unary Expressions Missing Closing Parentheses",
    "[PALM][Parser][Operator][Template][Unary][Malformed]",
    Oasis::Negate<>,
    Oasis::Magnitude<>)
{
    const std::string op = OperatorSymbol<TestType>::value;
    const std::string palm = "( " + op + " ( real -7 ) ";

    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::EndOfInput,
        .lexeme = "",
        .charOffset = palm.size(),
        .tokenIndex = 5,
        .line = 1,
        .column = palm.size() + 1,
    };

    requireMissingClosingParenthesisParsingError(palm, errorToken);
}

// ----------------
// Leaf Expressions
// ----------------
// --- Basic Tests ---
TEMPLATE_TEST_CASE(
    "PALM Parsing of Leaf Expressions",
    "[PALM][Parser][Operator][Template][Leaf]",
    Oasis::Undefined,
    Oasis::Imaginary,
    Oasis::EulerNumber,
    Oasis::Pi)
{
    const TestType expected {};

    const std::string op = OperatorSymbol<TestType>::value;
    const std::string palm = "( " + op + " )";

    requireSuccessfulParsing(palm, expected);
}

// --- Malformed Tests ---
TEMPLATE_TEST_CASE(
    "PALM Parsing of Leaf Expressions with Extra Tokens",
    "[PALM][Parser][Operator][Template][Leaf][Malformed]",
    Oasis::Undefined,
    Oasis::Imaginary,
    Oasis::EulerNumber,
    Oasis::Pi)
{
    const std::string op = OperatorSymbol<TestType>::value;
    const std::string palm = "( " + op + " extra_token )";
    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::Identifier,
        .lexeme = "extra_token",
        .charOffset = palm.size() - 13,
        .tokenIndex = 2,
        .line = 1,
        .column = palm.size() - 12,
    };

    requireExtraOperandParsingError(palm, errorToken);
}

TEMPLATE_TEST_CASE(
    "PALM Parsing of Leaf Expressions with Missing Parentheses",
    "[PALM][Parser][Operator][Template][Leaf][Malformed]",
    Oasis::Undefined,
    Oasis::Imaginary,
    Oasis::EulerNumber,
    Oasis::Pi)
{
    const std::string op = OperatorSymbol<TestType>::value;
    const std::string palm = " " + op + " )";
    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::Identifier,
        .lexeme = op,
        .charOffset = 1,
        .tokenIndex = 0,
        .line = 1,
        .column = 2,
    };

    requireMissingOpeningParenthesisParsingError(palm, errorToken);
}

TEMPLATE_TEST_CASE(
    "PALM Parsing of Leaf Expressions with Missing Closing Parentheses",
    "[PALM][Parser][Operator][Template][Leaf][Malformed]",
    Oasis::Undefined,
    Oasis::Imaginary,
    Oasis::EulerNumber,
    Oasis::Pi)
{
    const std::string op = OperatorSymbol<TestType>::value;
    const std::string palm = "( " + op + " ";

    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::EndOfInput,
        .lexeme = "",
        .charOffset = palm.size(),
        .tokenIndex = 3,
        .line = 1,
        .column = palm.size() + 1,
    };

    requireMissingClosingParenthesisParsingError(palm, errorToken);
}

// --------------
// Real Tests
// --------------

// --- Basic Tests ---
TEST_CASE("PALM Parsing of Real Numbers", "[PALM][Parser][Operator][Real]")
{
    auto [palm, expected] = GENERATE(
        std::make_pair("( real 42 )", Oasis::Real { 42.0 }),
        std::make_pair("( real -7 )", Oasis::Real { -7.0 }),
        std::make_pair("( real 3.14159 )", Oasis::Real { 3.14159 }),
        std::make_pair("( real -2.71828 )", Oasis::Real { -2.71828 }),
        std::make_pair("( real 0.0 )", Oasis::Real { 0.0 }),
        std::make_pair("( real 1.23e4 )", Oasis::Real { 12300.0 }),
        std::make_pair("( real NaN )", Oasis::Real { std::numeric_limits<double>::quiet_NaN() }),
        std::make_pair("( real Infinity )", Oasis::Real { std::numeric_limits<double>::infinity() }),
        std::make_pair("( real -Infinity )", Oasis::Real { -std::numeric_limits<double>::infinity() }));

    requireSuccessfulParsing(palm, expected);
}

// --- Malformed Tests ---
TEST_CASE("PALM Parsing of Real with Invalid Number", "[PALM][Parser][Operator][Real]")
{
    auto [palm, errorLexeme, errorType] = GENERATE(
        std::make_tuple("( real )", ")", Oasis::PALMTokenType::Punctuator),
        std::make_tuple("( real ( real 5 ) )", "(", Oasis::PALMTokenType::Punctuator),
        std::make_tuple("( real x )", "x", Oasis::PALMTokenType::Identifier),
        std::make_tuple("( real not_a_number )", "not_a_number", Oasis::PALMTokenType::Identifier),
        std::make_tuple("( real real )", "real", Oasis::PALMTokenType::Operator));


    const Oasis::PALMToken errorToken = {
        .type = errorType,
        .lexeme = errorLexeme,
        .charOffset = 7,
        .tokenIndex = 2,
        .line = 1,
        .column = 8,
    };

    requireMissingOperandParsingError(palm, errorToken);
}

TEST_CASE("PALM Parsing of Real with Out of Bounds Number", "[PALM][Parser][Operator][Real]")
{
    const std::string palm = "( real 1e4000 )";
    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::Number,
        .lexeme = "1e4000",
        .charOffset = 12,
        .tokenIndex = 3,
        .line = 1,
        .column = 13,
    };

    requireNumberOutOfBoundsParsingError(palm, errorToken);
}

TEST_CASE("PALM Parsing of Real with Extra Tokens", "[PALM][Parser][Operator][Real]")
{
    const std::string palm = "( real 3.14 extra_token )";
    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::Identifier,
        .lexeme = "extra_token",
        .charOffset = 17,
        .tokenIndex = 4,
        .line = 1,
        .column = 18
    };

    requireExtraOperandParsingError(palm, errorToken);
}

TEST_CASE("PALM Parsing of Real with Missing Opening Parentheses", "[PALM][Parser][Operator][Real]")
{
    const std::string palm = " real 3.14 )";
    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::Identifier,
        .lexeme = "real",
        .charOffset = 1,
        .tokenIndex = 0,
        .line = 1,
        .column = 2,
    };

    requireMissingOpeningParenthesisParsingError(palm, errorToken);
}

TEST_CASE("PALM Parsing of Real with Missing Closing Parentheses", "[PALM][Parser][Operator][Real]")
{
    const std::string palm = "( real 3.14 ";
    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::EndOfInput,
        .lexeme = "",
        .charOffset = palm.size(),
        .tokenIndex = 4,
        .line = 1,
        .column = palm.size() + 1,
    };

    requireMissingClosingParenthesisParsingError(palm, errorToken);
}

// ---------------
// Imaginary Tests
// ---------------
// Testing both 'i' and 'j' representations
TEST_CASE("PALM Parsing of Imaginary Number", "[PALM][Parser][Operator][Imaginary]")
{
    auto [palm, expected] = GENERATE(
        std::make_pair("( i )", Oasis::Imaginary {}),
        std::make_pair("( j )", Oasis::Imaginary {}));

    requireSuccessfulParsing(palm, expected);
}

// --------------
// Variable Tests
// --------------
// --- Basic Tests ---
TEST_CASE("PALM Parsing of Variable", "[PALM][Parser][Operator][Variable]")
{
    auto [palm, expected] = GENERATE(
        std::make_pair("( var myVar )", Oasis::Variable { "myVar" }),
        std::make_pair("( var x )", Oasis::Variable { "x" }),
        std::make_pair("( var var123 )", Oasis::Variable { "var123" }),
        std::make_pair("( var var_name )", Oasis::Variable { "var_name" })

    );

    requireSuccessfulParsing(palm, expected);
}

// --- Malformed Tests ---
TEST_CASE("PALM Parsing of Variable with Invalid Identifier", "[PALM][Parser][Operator][Variable]")
{
    auto [palm, errorLexeme, errorType] = GENERATE(
        std::make_tuple("( var var-name )", "var-name", Oasis::PALMTokenType::Unknown),
        std::make_tuple("( var var$name )", "var$name", Oasis::PALMTokenType::Unknown),
        std::make_tuple("( var real )", "real", Oasis::PALMTokenType::Operator),
        std::make_tuple("( var log )", "log", Oasis::PALMTokenType::Operator),
        std::make_tuple("( var 123 )", "123", Oasis::PALMTokenType::Number),
        std::make_tuple("( var ( real 5 ) )", "(", Oasis::PALMTokenType::Punctuator),
        std::make_tuple("( var )", ")", Oasis::PALMTokenType::Punctuator));

    const Oasis::PALMToken errorToken = {
        .type = errorType,
        .lexeme = errorLexeme,
        .charOffset = 6,
        .tokenIndex = 2,
        .line = 1,
        .column = 7,
    };

    requireInvalidVariableNameParsingError(palm, errorToken);
}

TEST_CASE("PALM Parsing of Variable with Extra Tokens", "[PALM][Parser][Operator][Variable]")
{
    const std::string palm = "( var x extra_token )";
    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::Identifier,
        .lexeme = "extra_token",
        .charOffset = 9,
        .tokenIndex = 3,
        .line = 1,
        .column = 10,
    };

    requireExtraOperandParsingError(palm, errorToken);
}

TEST_CASE("PALM Parsing of Variable with Missing Opening Parentheses", "[PALM][Parser][Operator][Variable]")
{
    const std::string palm = " var x )";
    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::Identifier,
        .lexeme = "var",
        .charOffset = 1,
        .tokenIndex = 0,
        .line = 1,
        .column = 2,
    };

    requireMissingOpeningParenthesisParsingError(palm, errorToken);
}

TEST_CASE("PALM Parsing of Variable with Missing Closing Parentheses", "[PALM][Parser][Operator][Variable]")
{
    const std::string palm = "( var x ";
    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::EndOfInput,
        .lexeme = "",
        .charOffset = palm.size(),
        .tokenIndex = 3,
        .line = 1,
        .column = palm.size() + 1,
    };

    requireMissingClosingParenthesisParsingError(palm, errorToken);
}

// ----------------
// Expression Tests
// ----------------
// --- Malformed Tests ---
TEST_CASE("PALM Parsing of Real with different spacing and padding", "[PALM][Parser][Real]")
{
    auto [ palm, expected ] = GENERATE(
        std::make_pair("(   real    7.89   )", Oasis::Real{7.89}),
        std::make_pair("(\treal\t0.5772\t)", Oasis::Real{0.5772}),
        std::make_pair("(\nreal\n1.618\n)", Oasis::Real{1.618})
    );

    requireSuccessfulParsing(palm, expected);
}

TEST_CASE("Parse Expression with Unknown Operator", "[PALM][Parsing]")
{
    const std::string palm = "( unknown_op ( real 5 ) ( real 3 ) )";
    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::Identifier,
        .lexeme = "unknown_op",
        .charOffset = 2,
        .tokenIndex = 0,
        .line = 1,
        .column = 3,
    };

    requireInvalidOperandParsingError(palm, errorToken);
}

TEST_CASE("Parse Empty Expression", "[PALM][Parsing]")
{
    const std::string palm;
    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::EndOfInput,
        .lexeme = "",
        .charOffset = 0,
        .tokenIndex = 0,
        .line = 1,
        .column = 1,
    };

    requireUnexpectedEndOfInputParsingError(palm, errorToken);
}

TEST_CASE("Parse Expression with Only Parentheses", "[PALM][Parsing]")
{
    const std::string palm = "( )";
    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::Punctuator,
        .lexeme = ")",
        .charOffset = 2,
        .tokenIndex = 1,
        .line = 1,
        .column = 3,
    };

    requireMissingOperatorParsingError(palm, errorToken);
}

TEST_CASE("Parse Expression with Missing Operator", "[PALM][Parsing]")
{
    const std::string palm = "( ( real 5 ) ( real 3 ) )";
    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::Punctuator,
        .lexeme = "(",
        .charOffset = 2,
        .tokenIndex = 1,
        .line = 1,
        .column = 3,
    };

    requireMissingOperatorParsingError(palm, errorToken);
}

TEST_CASE("Parse Expression with Extra Tokens After Closing Parenthesis", "[PALM][Parsing]")
{
    const std::string palm = "( + ( real 5 ) ( real 3 ) ) extra_token";
    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::Identifier,
        .lexeme = "extra_token",
        .charOffset = 33,
        .tokenIndex = 8,
        .line = 1,
        .column = 33,
    };

    requireUnexpectedTokenParsingError(palm, errorToken);
}

TEST_CASE("Parse Expression Invalid Start of Expression", "[PALM][Parsing]")
{
    const std::string palm = "'";
    const Oasis::PALMToken errorToken = {
        .type = Oasis::PALMTokenType::Operator,
        .lexeme = "'",
        .charOffset = 0,
        .tokenIndex = 0,
        .line = 1,
        .column = 1,
    };

    requireMissingOpeningParenthesisParsingError(palm, errorToken);
}

// ------------------------
// Complex Expression Tests
// ------------------------
TEST_CASE("PALM Parsing of Complex Expressions", "[PALM][Parser][Operator]") {
    using Factory = std::function<std::unique_ptr<Oasis::Expression>()>;

    auto [palm, makeExpected] = GENERATE(
        std::make_pair(
            std::string{"( + ( real 5 ) ( * ( real 3 ) ( - ( real 2 ) ( real 1 ) ) ) )"},
            Factory{[] {
                return std::unique_ptr<Oasis::Expression>(
                    new Oasis::Add<
                        Oasis::Real,
                        Oasis::Multiply<Oasis::Real, Oasis::Subtract<Oasis::Real>>
                    >(
                        Oasis::Real{5.0},
                        Oasis::Multiply{
                            Oasis::Real{3.0},
                            Oasis::Subtract{Oasis::Real{2.0}, Oasis::Real{1.0}}
                        }
                    )
                );
            }}
        ),
        std::make_pair(
            std::string{"( / ( ^ ( real 2 ) ( real 3 ) ) ( log ( real 10 ) ( real 1000 ) ) )"},
            Factory{[] {
                return std::unique_ptr<Oasis::Expression>(
                    new Oasis::Divide<
                        Oasis::Exponent<Oasis::Real, Oasis::Real>,
                        Oasis::Log<Oasis::Real>
                    >(
                        Oasis::Exponent{Oasis::Real{2.0}, Oasis::Real{3.0}},
                        Oasis::Log{Oasis::Real{10.0}, Oasis::Real{1000.0}}
                    )
                );
            }}
        ),
        std::make_pair(
            std::string{"( + ( real 5 ) ( - ( real 3 ) ( * ( real 2 ) ( / ( real 4 ) ( real 2 ) ) ) ) )"},
            Factory{[] {
                return std::unique_ptr<Oasis::Expression>(
                    new Oasis::Add<
                        Oasis::Real,
                        Oasis::Subtract<
                            Oasis::Real,
                            Oasis::Multiply<Oasis::Real, Oasis::Divide<Oasis::Real>
                            >
                        >
                    >(
                        Oasis::Real{5.0},
                        Oasis::Subtract{
                            Oasis::Real{3.0},
                            Oasis::Multiply{
                                Oasis::Real{2.0},
                                Oasis::Divide{Oasis::Real{4.0}, Oasis::Real{2.0}}
                            }
                        }
                    )
                );
            }}
        )
    );

    const auto expected = makeExpected();
    requireSuccessfulParsing(palm, *expected);
}
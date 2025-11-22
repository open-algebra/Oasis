#include "../../tests/Common.hpp"

#include "Oasis/RecursiveCast.hpp"
#include "catch2/catch_test_macros.hpp"

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

// =====================
// Test Helper Functions
// =====================

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
    Oasis::PALMSerializer serializer{opts};

    const auto visit = expr.Accept(serializer);
    REQUIRE(visit.has_value());

    const auto& result = visit.value();
    REQUIRE_FALSE(result.has_value());

    const auto& error = result.error();
    CAPTURE(error.message, expectedError.message);
    REQUIRE(error == expectedError);
}

void requireInvalidIdentifierError(
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

void requireMissingOperandError(
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

void requireMissingLeastSignificantOperandError(
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

void requireMissingMostSignificantOperandError(
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

    requireInvalidIdentifierError(variable, "");
}

TEST_CASE("PALM Serialization of Variable with Invalid Identifier", "[PALM][Serializer][Operator][Variable]")
{
    const Oasis::Variable variable { "123invalid" };

    requireInvalidIdentifierError(variable, "123invalid");
}

TEST_CASE("PALM Serialization of Variable with Special Characters", "[PALM][Serializer][Operator][Variable]")
{
    const Oasis::Variable variable { "var$!" };

    requireInvalidIdentifierError(variable, "var$!");
}

TEST_CASE("PALM Serialization of Variable with Space", "[PALM][Serializer][Operator][Variable]")
{
    const Oasis::Variable variable { "my var" };

    requireInvalidIdentifierError(variable, "my var");
}

TEST_CASE("PALM Serialization of Keyword as Variable", "[PALM][Serializer][Operator][Variable]")
{
    const Oasis::Variable variable { "real" };

    requireInvalidIdentifierError(variable, "real");
}

// --- Addition ---
// Basic Tests
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

// Malformed Tests
TEST_CASE("PALM Serialization of Addition Missing Least Significant Operand", "[PALM][Serializer][Addition]")
{
    Oasis::Add<Oasis::Real, Oasis::Expression> addition;
    addition.SetMostSigOp(Oasis::Real { 5.0 });

    requireMissingLeastSignificantOperandError(addition);
}

TEST_CASE("PALM Serialization of Addition Missing Most Significant Operand", "[PALM][Serializer][Addition]")
{
    Oasis::Add<Oasis::Expression, Oasis::Real> addition;
    addition.SetLeastSigOp(Oasis::Real { 3.0 });

    requireMissingMostSignificantOperandError(addition);
}

// --- Subtraction ---
// Basic Tests
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

// Malformed Tests
TEST_CASE("PALM Serialization of Subtraction Missing Least Significant Operand", "[PALM][Serializer][Subtraction]")
{
    Oasis::Subtract<Oasis::Real, Oasis::Expression> subtraction;
    subtraction.SetMostSigOp(Oasis::Real { 5.0 });

    requireMissingLeastSignificantOperandError(subtraction);
}

TEST_CASE("PALM Serialization of Subtraction Missing Most Significant Operand", "[PALM][Serializer][Subtraction]")
{
    Oasis::Subtract<Oasis::Expression, Oasis::Real> subtraction;
    subtraction.SetLeastSigOp(Oasis::Real { 3.0 });

    requireMissingMostSignificantOperandError(subtraction);
}

// --- Multiplication ---
// Basic Tests
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

// Malformed Tests
TEST_CASE("PALM Serialization of Multiplication Missing Least Significant Operand", "[PALM][Serializer][Multiplication]")
{
    Oasis::Multiply<Oasis::Real, Oasis::Expression> multiplication;
    multiplication.SetMostSigOp(Oasis::Real { 5.0 });

    requireMissingLeastSignificantOperandError(multiplication);
}

TEST_CASE("PALM Serialization of Multiplication Missing Most Significant Operand", "[PALM][Serializer][Multiplication]")
{
    Oasis::Multiply<Oasis::Expression, Oasis::Real> multiplication;
    multiplication.SetLeastSigOp(Oasis::Real { 3.0 });

    requireMissingMostSignificantOperandError(multiplication);
}

// --- Division ---
// Basic Tests
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

// Malformed Tests
TEST_CASE("PALM Serialization of Division Missing Least Significant Operand", "[PALM][Serializer][Division]")
{
    Oasis::Divide<Oasis::Real, Oasis::Expression> division;
    division.SetMostSigOp(Oasis::Real { 6.0 });

    requireMissingLeastSignificantOperandError(division);
}

TEST_CASE("PALM Serialization of Division Missing Most Significant Operand", "[PALM][Serializer][Division]")
{
    Oasis::Divide<Oasis::Expression, Oasis::Real> division;
    division.SetLeastSigOp(Oasis::Real { 3.0 });

    requireMissingMostSignificantOperandError(division);
}

// --- Exponentiation ---
// Basic Tests
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

// Malformed Tests
TEST_CASE("PALM Serialization of Exponentiation Missing Least Significant Operand", "[PALM][Serializer][Exponentiation]")
{
    Oasis::Exponent<Oasis::Real, Oasis::Expression> exponent;
    exponent.SetMostSigOp(Oasis::Real { 2.0 });

    requireMissingLeastSignificantOperandError(exponent);
}

TEST_CASE("PALM Serialization of Exponentiation Missing Most Significant Operand", "[PALM][Serializer][Exponentiation]")
{
    Oasis::Exponent<Oasis::Expression, Oasis::Real> exponent;
    exponent.SetLeastSigOp(Oasis::Real { 3.0 });

    requireMissingMostSignificantOperandError(exponent);
}

// --- Logarithm ---
// Basic Tests
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

// Malformed Tests
TEST_CASE("PALM Serialization of Logarithm Missing Least Significant Operand", "[PALM][Serializer][Logarithm]")
{
    Oasis::Log<Oasis::Real, Oasis::Expression> log;
    log.SetMostSigOp(Oasis::Real { 10.0 });

    requireMissingLeastSignificantOperandError(log);
}

TEST_CASE("PALM Serialization of Logarithm Missing Most Significant Operand", "[PALM][Serializer][Logarithm]")
{
    Oasis::Log<Oasis::Expression, Oasis::Real> log;
    log.SetLeastSigOp(Oasis::Real { 1000.0 });

    requireMissingMostSignificantOperandError(log);
}

// --- Negation ---
// Basic Tests
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

// Malformed Tests
TEST_CASE("PALM Serialization of Negation Missing Operand", "[PALM][Serializer][Negation]")
{
    const Oasis::Negate<> negate;

    requireMissingOperandError(negate);
}

// --- Derivative ---
// Basic Tests
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

// Malformed Tests
TEST_CASE("PALM Serialization of Derivative Missing Least Significant Operand", "[PALM][Serializer][Derivative]")
{
    Oasis::Derivative<Oasis::Real, Oasis::Expression> derivative;
    derivative.SetMostSigOp(Oasis::Real { 5.0 });

    requireMissingLeastSignificantOperandError(derivative);
}

TEST_CASE("PALM Serialization of Derivative Missing Most Significant Operand", "[PALM][Serializer][Derivative]")
{
    Oasis::Derivative<Oasis::Expression, Oasis::Real> derivative;
    derivative.SetLeastSigOp(Oasis::Real { 5.0 });

    requireMissingMostSignificantOperandError(derivative);
}

// --- Integral ---
// Basic Tests
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

// Malformed Tests
TEST_CASE("PALM Serialization of Integral Missing Least Significant Operand", "[PALM][Serializer][Integral]")
{
    Oasis::Integral<Oasis::Variable, Oasis::Expression> integral;
    integral.SetMostSigOp(Oasis::Variable { "x" });

    requireMissingLeastSignificantOperandError(integral);
}

TEST_CASE("PALM Serialization of Integral Missing Most Significant Operand", "[PALM][Serializer][Integral]")
{
    Oasis::Integral<Oasis::Expression, Oasis::Real> integral;
    integral.SetLeastSigOp(Oasis::Real { 5.0 });

    requireMissingMostSignificantOperandError(integral);
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
// Basic Tests
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

// Malformed Tests
TEST_CASE("PALM Serialization of Magnitude Missing Operand", "[PALM][Serializer][Magnitude]")
{
    const Oasis::Magnitude<Oasis::Expression> magnitude;

    requireMissingOperandError(magnitude);
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

// --------------
// Operator Tests
// --------------

// --- Undefined ---
// Basic Tests
TEST_CASE("PALM Parsing of Undefined", "[PALM][Parser][Operator][Undefined]")
{
    const auto expr = Oasis::FromPALM("( undefined )");

    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Is<Oasis::Undefined>());
}

// Malformed Tests
TEST_CASE("PALM Parsing of Undefined with Extra Tokens", "[PALM][Parser][Operator][Undefined]")
{
    const auto expr = Oasis::FromPALM("( undefined extra_token )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::ExtraOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Identifier,
            .lexeme = "extra_token",
            .charOffset = 13,
            .tokenIndex = 2,
            .line = 1,
            .column = 14,
        },
        .message = "Unexpected token 'extra_token' when parsing operator 'undefined'."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

// --- Real ---
// Basic Tests
TEST_CASE("PALM Parsing of Real Number", "[PALM][Parser][Operator][Real]")
{
    const auto expr = Oasis::FromPALM("( real 3.14159 )");

    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Is<Oasis::Real>());

    const auto real = Oasis::RecursiveCast<Oasis::Real>(**expr);
    REQUIRE(real->GetValue() == 3.14159);
}

TEST_CASE("PALM Parsing of Negative Real Number", "[PALM][Parser][Operator][Real]")
{
    const auto expr = Oasis::FromPALM("( real -2.71828 )");

    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Is<Oasis::Real>());

    const auto real = Oasis::RecursiveCast<Oasis::Real>(**expr);
    REQUIRE(real->GetValue() == -2.71828);
}

TEST_CASE("PALM Parsing of Zero Real Number", "[PALM][Parser][Operator][Real]")
{
    const auto expr = Oasis::FromPALM("( real 0.0 )");

    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Is<Oasis::Real>());

    const auto real = Oasis::RecursiveCast<Oasis::Real>(**expr);
    REQUIRE(real->GetValue() == 0.0);
}

TEST_CASE("PALM Parsing of Scientific Notation Real Number", "[PALM][Parser][Operator][Real]")
{
    const auto expr = Oasis::FromPALM("( real 1.23e4 )");

    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Is<Oasis::Real>());

    const auto real = Oasis::RecursiveCast<Oasis::Real>(**expr);
    REQUIRE(real->GetValue() == 12300.0);
}

// Malformed Tests
TEST_CASE("PALM Parsing of Real without Number", "[PALM][Parser][Operator][Real]")
{
    const auto expr = Oasis::FromPALM("( real )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::MissingOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Punctuator,
            .lexeme = ")",
            .charOffset = 7,
            .tokenIndex = 2,
            .line = 1,
            .column = 8,
        },
        .message = "Expected a real number after 'real' token."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Real with Invalid Number", "[PALM][Parser][Operator][Real]")
{
    const auto expr = Oasis::FromPALM("( real not_a_number )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidNumberFormat,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Unknown,
            .lexeme = "not_a_number",
            .charOffset = 12,
            .tokenIndex = 3,
            .line = 1,
            .column = 13,
        },
        .message = "Invalid number format for real number: 'not_a_number'."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Real with Out of Bounds Number", "[PALM][Parser][Operator][Real]")
{
    const auto expr = Oasis::FromPALM("( real 1e4000 )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::NumberOutOfBounds,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Unknown,
            .lexeme = "1e4000",
            .charOffset = 12,
            .tokenIndex = 3,
            .line = 1,
            .column = 13,
        },
        .message = "Real number out of bounds: '1e4000'."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Real with Extra Tokens", "[PALM][Parser][Operator][Real]")
{
    const auto expr = Oasis::FromPALM("( real 3.14 extra_token )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::ExtraOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Identifier,
            .lexeme = "extra_token",
            .charOffset = 17,
            .tokenIndex = 4,
            .line = 1,
            .column = 18,
        },
        .message = "Unexpected token 'extra_token' when parsing operator 'real'."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

// --- Imaginary ---
// Basic Tests
TEST_CASE("PALM Parsing of Imaginary Number with 'i'", "[PALM][Parser][Operator][Imaginary]")
{
    const auto expr = Oasis::FromPALM("( i )");

    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Is<Oasis::Imaginary>());
}

TEST_CASE("PALM Parsing of Imaginary Number with 'j'", "[PALM][Parser][Operator][Imaginary]")
{
    const auto expr = Oasis::FromPALM("( j )");

    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Is<Oasis::Imaginary>());
}

// Malformed Tests
TEST_CASE("PALM Parsing of Imaginary Missing Closing Parenthesis", "[PALM][Parser][Operator][Imaginary]")
{
    const auto expr = Oasis::FromPALM("( i extra_token ) ");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::ExtraOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Identifier,
            .lexeme = "extra_token",
            .charOffset = 4,
            .tokenIndex = 2,
            .line = 1,
            .column = 5,
        },
        .message = "Expected closing parenthesis for 'i' operator."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

// --- Variable ---
// Basic Tests
TEST_CASE("PALM Parsing of Variable", "[PALM][Parser][Operator][Variable]")
{
    const auto expr = Oasis::FromPALM("( var myVar )");

    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Is<Oasis::Variable>());

    const auto var = Oasis::RecursiveCast<Oasis::Variable>(**expr);
    REQUIRE(var->GetName() == "myVar");
}

TEST_CASE("PALM Parsing of Variable with Single Letter Name", "[PALM][Parser][Operator][Variable]")
{
    const auto expr = Oasis::FromPALM("( var x )");

    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Is<Oasis::Variable>());

    const auto var = Oasis::RecursiveCast<Oasis::Variable>(**expr);
    REQUIRE(var->GetName() == "x");
}

TEST_CASE("PALM Parsing of Variable with numbers in Name", "[PALM][Parser][Operator][Variable]")
{
    const auto expr = Oasis::FromPALM("( var var123 )");

    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Is<Oasis::Variable>());

    const auto var = Oasis::RecursiveCast<Oasis::Variable>(**expr);
    REQUIRE(var->GetName() == "var123");
}

TEST_CASE("PALM Parsing of Variable with Underscore in Name", "[PALM][Parser][Operator][Variable]")
{
    const auto expr = Oasis::FromPALM("( var var_name )");

    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Is<Oasis::Variable>());

    const auto var = Oasis::RecursiveCast<Oasis::Variable>(**expr);
    REQUIRE(var->GetName() == "var_name");
}

// Malformed Tests
TEST_CASE("PALM Parsing of Variable with Invalid Characters in Name", "[PALM][Parser][Operator][Variable]")
{
    const auto expr = Oasis::FromPALM("( var var-name )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidVariableName,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Identifier,
            .lexeme = "var-name",
            .charOffset = 6,
            .tokenIndex = 2,
            .line = 1,
            .column = 7,
        },
        .message = "Invalid variable name: 'var-name'."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Variable with Reserved Keyword as Name", "[PALM][Parser][Operator][Variable]")
{
    const auto expr = Oasis::FromPALM("( var real )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidVariableName,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Operator,
            .lexeme = "real",
            .charOffset = 6,
            .tokenIndex = 2,
            .line = 1,
            .column = 7,
        },
        .message = "Invalid variable name: 'real'."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Variable with Number as Name", "[PALM][Parser][Operator][Variable]")
{
    const auto expr = Oasis::FromPALM("( var 123 )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidVariableName,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Number,
            .lexeme = "123",
            .charOffset = 6,
            .tokenIndex = 2,
            .line = 1,
            .column = 7,
        },
        .message = "Invalid variable name: '123'."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Variable with Expression Name", "[PALM][Parser][Operator][Variable]")
{
    const auto expr = Oasis::FromPALM("( var ( real 5 ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidVariableName,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Punctuator,
            .lexeme = "(",
            .charOffset = 6,
            .tokenIndex = 2,
            .line = 1,
            .column = 7,
        },
        .message = "Invalid variable name: '('."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Variable with Empty Name", "[PALM][Parser][Operator][Variable]")
{
    const auto expr = Oasis::FromPALM("( var  )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::MissingOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Punctuator,
            .lexeme = ")",
            .charOffset = 6,
            .tokenIndex = 2,
            .line = 1,
            .column = 7,
        },
        .message = "Expected a variable name after 'var' token."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Variable with Extra Tokens", "[PALM][Parser][Operator][Variable]")
{
    const auto expr = Oasis::FromPALM("( var x extra_token )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::ExtraOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Identifier,
            .lexeme = "extra_token",
            .charOffset = 9,
            .tokenIndex = 3,
            .line = 1,
            .column = 10,
        },
        .message = "Unexpected token 'extra_token' when parsing operator 'var'."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

// --- Addition ---
// Basic Tests
TEST_CASE("PALM Parsing of Addition", "[PALM][Parser][Operator][Addition]")
{
    const Oasis::Add<> expected {
        Oasis::Real { 5.0 },
        Oasis::Real { 3.0 },
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALM("( + ( real 5 ) ( real 3 ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("PALM Parsing of Nested Addition", "[PALM][Parser][Operator][Addition]")
{
    const Oasis::Add<> expected {
        Oasis::Real { 5.0 },
        Oasis::Add {
            Oasis::Real { 3.0 },
            Oasis::Real { 2.0 } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALM("( + ( real 5 ) ( + ( real 3 ) ( real 2 ) ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

// Malformed Tests
TEST_CASE("PALM Parsing of Addition Missing Least Significant Operand", "[PALM][Parser][Operator][Addition]")
{
    const auto expr = Oasis::FromPALM("( + ( real 5 ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::MissingOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Punctuator,
            .lexeme = ")",
            .charOffset = 15,
            .tokenIndex = 4,
            .line = 1,
            .column = 16,
        },
        .message = "Incomplete '+' expression; expected two operands."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Addition Missing Most Significant Operand", "[PALM][Parser][Operator][Addition]")
{
    const auto expr = Oasis::FromPALM("( + )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::MissingOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Punctuator,
            .lexeme = ")",
            .charOffset = 4,
            .tokenIndex = 2,
            .line = 1,
            .column = 5,
        },
        .message = "Incomplete '+' expression; expected two operands."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Addition with Too Many Operands", "[PALM][Parser][Operator][Addition]")
{
    const auto expr = Oasis::FromPALM("( + ( real 5 ) ( real 3 ) ( real 2 ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::ExtraOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Identifier,
            .lexeme = "(",
            .charOffset = 30,
            .tokenIndex = 6,
            .line = 1,
            .column = 31,
        },
        .message = "Unexpected token '(' when parsing operator '+'."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Addition with Number Operand", "[PALM][Parser][Operator][Addition]")
{
    const auto expr = Oasis::FromPALM("( + 5 ( real 3 ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidOperand,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Number,
            .lexeme = "5",
            .charOffset = 4,
            .tokenIndex = 2,
            .line = 1,
            .column = 5,
        },
        .message = "Invalid operand '5' for '+' operator; expected an expression."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Addition with Identifier Operand", "[PALM][Parser][Operator][Addition]")
{
    const auto expr = Oasis::FromPALM("( + ( real 5 ) x )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidOperand,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Identifier,
            .lexeme = "x",
            .charOffset = 18,
            .tokenIndex = 5,
            .line = 1,
            .column = 19,
        },
        .message = "Invalid operand 'x' for '+' operator; expected an expression."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

// --- Subtraction ---
// Basic Tests
TEST_CASE("PALM Parsing of Subtraction", "[PALM][Parser][Operator][Subtraction]")
{
    const Oasis::Subtract expected {
        Oasis::Real { 5.0 },
        Oasis::Real { 3.0 },
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALM("( - ( real 5 ) ( real 3 ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("PALM Parsing of Nested Subtraction", "[PALM][Parser][Operator][Subtraction]")
{
    const Oasis::Subtract expected {
        Oasis::Real { 5.0 },
        Oasis::Subtract {
            Oasis::Real { 3.0 },
            Oasis::Real { 2.0 } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALM("( - ( real 5 ) ( - ( real 3 ) ( real 2 ) ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

// Malformed Tests
TEST_CASE("PALM Parsing of Subtraction Missing Least Significant Operand", "[PALM][Parser][Operator][Subtraction]")
{
    const auto expr = Oasis::FromPALM("( - ( real 5 ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::MissingOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Punctuator,
            .lexeme = ")",
            .charOffset = 15,
            .tokenIndex = 4,
            .line = 1,
            .column = 16,
        },
        .message = "Incomplete '-' expression; expected two operands."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Subtraction Missing Most Significant Operand", "[PALM][Parser][Operator][Subtraction]")
{
    const auto expr = Oasis::FromPALM("( - )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::MissingOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Punctuator,
            .lexeme = ")",
            .charOffset = 4,
            .tokenIndex = 2,
            .line = 1,
            .column = 5,
        },
        .message = "Incomplete '-' expression; expected two operands."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Subtraction with Too Many Operands", "[PALM][Parser][Operator][Subtraction]")
{
    const auto expr = Oasis::FromPALM("( - ( real 5 ) ( real 3 ) ( real 2 ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::ExtraOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Identifier,
            .lexeme = "(",
            .charOffset = 30,
            .tokenIndex = 6,
            .line = 1,
            .column = 31,
        },
        .message = "Unexpected token '(' when parsing operator '-'."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Subtraction with Number Operand", "[PALM][Parser][Operator][Subtraction]")
{
    const auto expr = Oasis::FromPALM("( - 5 ( real 3 ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidOperand,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Number,
            .lexeme = "5",
            .charOffset = 4,
            .tokenIndex = 2,
            .line = 1,
            .column = 5,
        },
        .message = "Invalid operand '5' for '-' operator; expected an expression."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Subtraction with Identifier Operand", "[PALM][Parser][Operator][Subtraction]")
{
    const auto expr = Oasis::FromPALM("( - ( real 5 ) x )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidOperand,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Identifier,
            .lexeme = "x",
            .charOffset = 18,
            .tokenIndex = 5,
            .line = 1,
            .column = 19,
        },
        .message = "Invalid operand 'x' for '-' operator; expected an expression."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

// --- Multiplication ---
// Basic Tests
TEST_CASE("PALM Parsing of Multiplication", "[PALM][Parser][Operator][Multiplication]")
{
    const Oasis::Multiply expected {
        Oasis::Real { 5.0 },
        Oasis::Real { 3.0 },
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALM("( * ( real 5 ) ( real 3 ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("PALM Parsing of Nested Multiplication", "[PALM][Parser][Operator][Multiplication]")
{
    const Oasis::Multiply expected {
        Oasis::Real { 5.0 },
        Oasis::Multiply {
            Oasis::Real { 3.0 },
            Oasis::Real { 2.0 } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALM("( * ( real 5 ) ( * ( real 3 ) ( real 2 ) ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

// Malformed Tests
TEST_CASE("PALM Parsing of Multiplication Missing Least Significant Operand", "[PALM][Parser][Operator][Multiplication]")
{
    const auto expr = Oasis::FromPALM("( * ( real 5 ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::MissingOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Punctuator,
            .lexeme = ")",
            .charOffset = 15,
            .tokenIndex = 4,
            .line = 1,
            .column = 16,
        },
        .message = "Incomplete '*' expression; expected at least two operands."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Multiplication Missing Most Significant Operand", "[PALM][Parser][Operator][Multiplication]")
{
    const auto expr = Oasis::FromPALM("( * )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::MissingOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Punctuator,
            .lexeme = ")",
            .charOffset = 4,
            .tokenIndex = 2,
            .line = 1,
            .column = 5,
        },
        .message = "Incomplete '*' expression; expected at least two operands."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Multiplication with Too Many Operands", "[PALM][Parser][Operator][Multiplication]")
{
    const auto expr = Oasis::FromPALM("( * ( real 5 ) ( real 3 ) ( real 2 ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::ExtraOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Identifier,
            .lexeme = "(",
            .charOffset = 30,
            .tokenIndex = 6,
            .line = 1,
            .column = 31,
        },
        .message = "Unexpected token '(' when parsing operator '*'."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Multiplication with Number Operand", "[PALM][Parser][Operator][Multiplication]")
{
    const auto expr = Oasis::FromPALM("( * 5 ( real 3 ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidOperand,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Number,
            .lexeme = "5",
            .charOffset = 4,
            .tokenIndex = 2,
            .line = 1,
            .column = 5,
        },
        .message = "Invalid operand '5' for '*' operator; expected an expression."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Multiplication with Identifier Operand", "[PALM][Parser][Operator][Multiplication]")
{
    const auto expr = Oasis::FromPALM("( * ( real 5 ) x )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidOperand,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Identifier,
            .lexeme = "x",
            .charOffset = 18,
            .tokenIndex = 5,
            .line = 1,
            .column = 19,
        },
        .message = "Invalid operand 'x' for '*' operator; expected an expression."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

// --- Division ---
TEST_CASE("Parse Simple Division", "[PALM][Parser][Operator][Division]")
{
    const Oasis::Divide expected {
        Oasis::Real { 6.0 },
        Oasis::Real { 3.0 }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALM("( / ( real 6 ) ( real 3 ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Nested Division", "[PALM][Parser][Operator][Division]")
{
    const Oasis::Divide expected {
        Oasis::Real { 6.0 },
        Oasis::Divide {
            Oasis::Real { 3.0 },
            Oasis::Real { 2.0 } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALM("( / ( real 6 ) ( / ( real 3 ) ( real 2 ) ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

// Malformed Tests
TEST_CASE("PALM Parsing of Division Missing Least Significant Operand", "[PALM][Parser][Operator][Division]")
{
    const auto expr = Oasis::FromPALM("( / ( real 6 ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::MissingOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Punctuator,
            .lexeme = ")",
            .charOffset = 15,
            .tokenIndex = 4,
            .line = 1,
            .column = 16,
        },
        .message = "Incomplete '/' expression; expected two operands."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Division Missing Most Significant Operand", "[PALM][Parser][Operator][Division]")
{
    const auto expr = Oasis::FromPALM("( / )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::MissingOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Punctuator,
            .lexeme = ")",
            .charOffset = 4,
            .tokenIndex = 2,
            .line = 1,
            .column = 5,
        },
        .message = "Incomplete '/' expression; expected two operands."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Division with Too Many Operands", "[PALM][Parser][Operator][Division]")
{
    const auto expr = Oasis::FromPALM("( / ( real 6 ) ( real 3 ) ( real 2 ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::ExtraOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Identifier,
            .lexeme = "(",
            .charOffset = 30,
            .tokenIndex = 6,
            .line = 1,
            .column = 31,
        },
        .message = "Unexpected token '(' when parsing operator '/'."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Division with Number Operand", "[PALM][Parser][Operator][Division]")
{
    const auto expr = Oasis::FromPALM("( / 6 ( real 3 ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidOperand,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Number,
            .lexeme = "6",
            .charOffset = 4,
            .tokenIndex = 2,
            .line = 1,
            .column = 5,
        },
        .message = "Invalid operand '6' for '/' operator; expected an expression."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Division with Identifier Operand", "[PALM][Parser][Operator][Division]")
{
    const auto expr = Oasis::FromPALM("( / ( real 6 ) x )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidOperand,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Identifier,
            .lexeme = "x",
            .charOffset = 18,
            .tokenIndex = 5,
            .line = 1,
            .column = 19,
        },
        .message = "Invalid operand 'x' for '/' operator; expected an expression."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

// --- Exponent ---
// Basic Tests
TEST_CASE("PALM Parsing of Exponent", "[PALM][Parser][Operator][Exponent]")
{
    const Oasis::Exponent expected {
        Oasis::Real { 2.0 },
        Oasis::Real { 3.0 },
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALM("( ^ ( real 2 ) ( real 3 ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("PALM Parsing of Nested Exponent", "[PALM][Parser][Operator][Exponent]")
{
    const Oasis::Exponent expected {
        Oasis::Real { 2.0 },
        Oasis::Exponent {
            Oasis::Real { 3.0 },
            Oasis::Real { 2.0 } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALM("( ^ ( real 2 ) ( ^ ( real 3 ) ( real 2 ) ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

// Malformed Tests
TEST_CASE("PALM Parsing of Exponent Missing Least Significant Operand", "[PALM][Parser][Operator][Exponent]")
{
    const auto expr = Oasis::FromPALM("( ^ ( real 2 ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::MissingOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Punctuator,
            .lexeme = ")",
            .charOffset = 15,
            .tokenIndex = 4,
            .line = 1,
            .column = 16,
        },
        .message = "Incomplete '^' expression; expected two operands."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Exponent Missing Most Significant Operand", "[PALM][Parser][Operator][Exponent]")
{
    const auto expr = Oasis::FromPALM("( ^ )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::MissingOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Punctuator,
            .lexeme = ")",
            .charOffset = 4,
            .tokenIndex = 2,
            .line = 1,
            .column = 5,
        },
        .message = "Incomplete '^' expression; expected two operands."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Exponent with Too Many Operands", "[PALM][Parser][Operator][Exponent]")
{
    const auto expr = Oasis::FromPALM("( ^ ( real 2 ) ( real 3 ) ( real 2 ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::ExtraOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Identifier,
            .lexeme = "(",
            .charOffset = 30,
            .tokenIndex = 6,
            .line = 1,
            .column = 31,
        },
        .message = "Unexpected token '(' when parsing operator '^'."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Exponent with Number Operand", "[PALM][Parser][Operator][Exponent]")
{
    const auto expr = Oasis::FromPALM("( ^ 2 ( real 3 ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidOperand,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Number,
            .lexeme = "2",
            .charOffset = 4,
            .tokenIndex = 2,
            .line = 1,
            .column = 5,
        },
        .message = "Invalid operand '2' for '^' operator; expected an expression."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Exponent with Identifier Operand", "[PALM][Parser][Operator][Exponent]")
{
    const auto expr = Oasis::FromPALM("( ^ ( real 2 ) x )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidOperand,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Identifier,
            .lexeme = "x",
            .charOffset = 18,
            .tokenIndex = 5,
            .line = 1,
            .column = 19,
        },
        .message = "Invalid operand 'x' for '^' operator; expected an expression."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

// --- Logarithm ---
// Basic Tests
TEST_CASE("Parse Simple Logarithm", "[PALM][Parser][Operator][Logarithm]")
{
    const Oasis::Log expected {
        Oasis::Real { 2.0 },
        Oasis::Real { 8.0 }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALM("( log ( real 2 ) ( real 8 ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Nested Logarithm", "[PALM][Parser][Operator][Logarithm]")
{
    const Oasis::Log expected {
        Oasis::Real { 2.0 },
        Oasis::Log {
            Oasis::Real { 3.0 },
            Oasis::Real { 9.0 } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALM("( log ( real 2 ) ( log ( real 3 ) ( real 9 ) ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

// Malformed Tests
TEST_CASE("PALM Parsing of Logarithm Missing Least Significant Operand", "[PALM][Parser][Operator][Logarithm]")
{
    const auto expr = Oasis::FromPALM("( log ( real 2 ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::MissingOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Punctuator,
            .lexeme = ")",
            .charOffset = 15,
            .tokenIndex = 4,
            .line = 1,
            .column = 16,
        },
        .message = "Incomplete 'log' expression; expected two operands."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Logarithm Missing Most Significant Operand", "[PALM][Parser][Operator][Logarithm]")
{
    const auto expr = Oasis::FromPALM("( log )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::MissingOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Punctuator,
            .lexeme = ")",
            .charOffset = 4,
            .tokenIndex = 2,
            .line = 1,
            .column = 5,
        },
        .message = "Incomplete 'log' expression; expected two operands."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Logarithm with Too Many Operands", "[PALM][Parser][Operator][Logarithm]")
{
    const auto expr = Oasis::FromPALM("( log ( real 2 ) ( real 8 ) ( real 4 ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::ExtraOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Identifier,
            .lexeme = "(",
            .charOffset = 30,
            .tokenIndex = 6,
            .line = 1,
            .column = 31,
        },
        .message = "Unexpected token '(' when parsing operator 'log'."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Logarithm with Number Operand", "[PALM][Parser][Operator][Logarithm]")
{
    const auto expr = Oasis::FromPALM("( log 2 ( real 8 ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidOperand,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Number,
            .lexeme = "2",
            .charOffset = 4,
            .tokenIndex = 2,
            .line = 1,
            .column = 5,
        },
        .message = "Invalid operand '2' for 'log' operator; expected an expression."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Logarithm with Identifier Operand", "[PALM][Parser][Operator][Logarithm]")
{
    const auto expr = Oasis::FromPALM("( log ( real 2 ) x )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidOperand,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Identifier,
            .lexeme = "x",
            .charOffset = 18,
            .tokenIndex = 5,
            .line = 1,
            .column = 19,
        },
        .message = "Invalid operand 'x' for 'log' operator; expected an expression."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

// --- Integral ---
TEST_CASE("Parse Simple Integral", "[PALM][Parser][Operator][Integral]")
{
    const Oasis::Integral expected {
        Oasis::Variable { "x" },
        Oasis::Real { 0.0 },
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALM("( int ( var x ) ( real 0 ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Nested Integral", "[PALM][Parser][Operator][Integral]")
{
    const Oasis::Integral expected {
        Oasis::Variable { "x" },
        Oasis::Integral {
            Oasis::Variable { "y" },
            Oasis::Real { 0.0 } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALM("( int ( var x ) ( int ( var y ) ( real 0 ) ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

// Malformed Tests
TEST_CASE("PALM Parsing of Integral Missing Least Significant Operand", "[PALM][Parser][Operator][Integral]")
{
    const auto expr = Oasis::FromPALM("( int ( var x ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::MissingOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Punctuator,
            .lexeme = ")",
            .charOffset = 18,
            .tokenIndex = 5,
            .line = 1,
            .column = 19,
        },
        .message = "Incomplete 'int' expression; expected two operands."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Integral Missing Most Significant Operand", "[PALM][Parser][Operator][Integral]")
{
    const auto expr = Oasis::FromPALM("( int )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::MissingOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Punctuator,
            .lexeme = ")",
            .charOffset = 6,
            .tokenIndex = 2,
            .line = 1,
            .column = 7,
        },
        .message = "Incomplete 'int' expression; expected two operands."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Integral with Too Many Operands", "[PALM][Parser][Operator][Integral]")
{
    const auto expr = Oasis::FromPALM("( int ( var x ) ( real 0 ) ( real 1 ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::ExtraOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Identifier,
            .lexeme = "(",
            .charOffset = 30,
            .tokenIndex = 6,
            .line = 1,
            .column = 31,
        },
        .message = "Unexpected token '(' when parsing operator 'int'."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Integral with Number Operand", "[PALM][Parser][Operator][Integral]")
{
    const auto expr = Oasis::FromPALM("( int 5 ( real 0 ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidOperand,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Number,
            .lexeme = "5",
            .charOffset = 5,
            .tokenIndex = 2,
            .line = 1,
            .column = 6,
        },
        .message = "Invalid operand '5' for 'int' operator; expected an expression."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Integral with Identifier Operand", "[PALM][Parser][Operator][Integral]")
{
    const auto expr = Oasis::FromPALM("( int ( var x ) y )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidOperand,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Identifier,
            .lexeme = "y",
            .charOffset = 19,
            .tokenIndex = 6,
            .line = 1,
            .column = 20,
        },
        .message = "Invalid operand 'y' for 'int' operator; expected an expression."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

// --- Negate ---
// Basic Tests
TEST_CASE("Parse Simple Negate", "[PALM][Parser][Operator][Negate]")
{
    const Oasis::Negate expected {
        Oasis::Real { 5.0 }
    };

    const auto expr = Oasis::FromPALM("( neg ( real 5 ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

// Malformed Tests
TEST_CASE("PALM Parsing of Negate Missing Operand", "[PALM][Parser][Operator][Negate]")
{
    const auto expr = Oasis::FromPALM("( neg )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::MissingOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Punctuator,
            .lexeme = ")",
            .charOffset = 6,
            .tokenIndex = 2,
            .line = 1,
            .column = 7,
        },
        .message = "Incomplete 'neg' expression; expected one operand."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Negate with Too Many Operands", "[PALM][Parser][Operator][Negate]")
{
    const auto expr = Oasis::FromPALM("( neg ( real 5 ) ( real 3 ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::ExtraOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Identifier,
            .lexeme = "(",
            .charOffset = 20,
            .tokenIndex = 5,
            .line = 1,
            .column = 21,
        },
        .message = "Unexpected token '(' when parsing operator 'neg'."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Negate with Number Operand", "[PALM][Parser][Operator][Negate]")
{
    const auto expr = Oasis::FromPALM("( neg 5 )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidOperand,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Number,
            .lexeme = "5",
            .charOffset = 5,
            .tokenIndex = 2,
            .line = 1,
            .column = 6,
        },
        .message = "Invalid operand '5' for 'neg' operator; expected an expression."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Negate with Identifier Operand", "[PALM][Parser][Operator][Negate]")
{
    const auto expr = Oasis::FromPALM("( neg x )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidOperand,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Identifier,
            .lexeme = "x",
            .charOffset = 5,
            .tokenIndex = 2,
            .line = 1,
            .column = 6,
        },
        .message = "Invalid operand 'x' for 'neg' operator; expected an expression."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

// --- Derivative ---
// Basic Tests
TEST_CASE("Parse Simple Derivative", "[PALM][Parser][Operator][Derivative]")
{
    const Oasis::Derivative expected {
        Oasis::Variable { "x" },
        Oasis::Real { 0.0 },
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALM("( d ( var x ) ( real 0 ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Nested Derivative", "[PALM][Parser][Operator][Derivative]")
{
    const Oasis::Derivative expected {
        Oasis::Variable { "x" },
        Oasis::Derivative {
            Oasis::Variable { "y" },
            Oasis::Real { 0.0 } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALM("( d ( var x ) ( d ( var y ) ( real 0 ) ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

// Malformed Tests
TEST_CASE("PALM Parsing of Derivative Missing Least Significant Operand", "[PALM][Parser][Operator][Derivative]")
{
    const auto expr = Oasis::FromPALM("( d ( var x ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::MissingOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Punctuator,
            .lexeme = ")",
            .charOffset = 18,
            .tokenIndex = 5,
            .line = 1,
            .column = 19,
        },
        .message = "Incomplete 'd' expression; expected two operands."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Derivative Missing Most Significant Operand", "[PALM][Parser][Operator][Derivative]")
{
    const auto expr = Oasis::FromPALM("( d )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::MissingOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Punctuator,
            .lexeme = ")",
            .charOffset = 4,
            .tokenIndex = 2,
            .line = 1,
            .column = 5,
        },
        .message = "Incomplete 'd' expression; expected two operands."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Derivative with Too Many Operands", "[PALM][Parser][Operator][Derivative]")
{
    const auto expr = Oasis::FromPALM("( d ( var x ) ( real 0 ) ( real 1 ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::ExtraOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Identifier,
            .lexeme = "(",
            .charOffset = 30,
            .tokenIndex = 6,
            .line = 1,
            .column = 31,
        },
        .message = "Unexpected token '(' when parsing operator 'd'."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Derivative with Number Operand", "[PALM][Parser][Operator][Derivative]")
{
    const auto expr = Oasis::FromPALM("( d 5 ( real 0 ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidOperand,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Number,
            .lexeme = "5",
            .charOffset = 5,
            .tokenIndex = 2,
            .line = 1,
            .column = 6,
        },
        .message = "Invalid operand '5' for 'd' operator; expected an expression."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Derivative with Identifier Operand", "[PALM][Parser][Operator][Derivative]")
{
    const auto expr = Oasis::FromPALM("( d ( var x ) y )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidOperand,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Identifier,
            .lexeme = "y",
            .charOffset = 19,
            .tokenIndex = 6,
            .line = 1,
            .column = 20,
        },
        .message = "Invalid operand 'y' for 'd' operator; expected an expression."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

// --- Pi ---
// Basic Tests
TEST_CASE("Parse Constant Pi (pi)", "[PALM][Parser][Operator][Pi]")
{
    const auto expr = Oasis::FromPALM("( pi )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Is<Oasis::Pi>());
}

// Malformed Tests
TEST_CASE("PALM Parsing of Pi with Too Many Operands", "[PALM][Parser][Operator][Pi]")
{
    const auto expr = Oasis::FromPALM("( pi ( real 3 ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::ExtraOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Punctuator,
            .lexeme = "(",
            .charOffset = 8,
            .tokenIndex = 3,
            .line = 1,
            .column = 9,
        },
        .message = "Unexpected token '(' when parsing constant 'pi'."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

// --- Euler's Number ---
// Basic Tests
TEST_CASE("Parse Constant Euler's Number", "[PALM][Parser][Operator][EulerNumber]")
{
    const auto expr = Oasis::FromPALM("( e )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Is<Oasis::EulerNumber>());
}

// Malformed Tests
TEST_CASE("PALM Parsing of Euler's Number with Too Many Operands", "[PALM][Parser][Operator][EulerNumber]")
{
    const auto expr = Oasis::FromPALM("( e ( real 2 ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::ExtraOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Punctuator,
            .lexeme = "(",
            .charOffset = 6,
            .tokenIndex = 3,
            .line = 1,
            .column = 7,
        },
        .message = "Unexpected token '(' when parsing constant 'e'."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

// --- Magnitude ---
// Basic Tests
TEST_CASE("Parse Magnitude", "[PALM][Parser][Operator][Magnitude]")
{
    const Oasis::Magnitude expected {
        Oasis::Real { -7.5 }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALM("( mag ( real -7.5 ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

// Malformed Tests
TEST_CASE("PALM Parsing of Magnitude Missing Operand", "[PALM][Parser][Operator][Magnitude]")
{
    const auto expr = Oasis::FromPALM("( mag )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::MissingOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Punctuator,
            .lexeme = ")",
            .charOffset = 6,
            .tokenIndex = 2,
            .line = 1,
            .column = 7,
        },
        .message = "Incomplete 'mag' expression; expected one operand."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Magnitude with Too Many Operands", "[PALM][Parser][Operator][Magnitude]")
{
    const auto expr = Oasis::FromPALM("( mag ( real -7.5 ) ( real 3 ) )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::ExtraOperands,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Identifier,
            .lexeme = "(",
            .charOffset = 20,
            .tokenIndex = 5,
            .line = 1,
            .column = 21,
        },
        .message = "Unexpected token '(' when parsing operator 'mag'."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Magnitude with Number Operand", "[PALM][Parser][Operator][Magnitude]")
{
    const auto expr = Oasis::FromPALM("( mag  -7.5 )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidOperand,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Number,
            .lexeme = "-7.5",
            .charOffset = 5,
            .tokenIndex = 2,
            .line = 1,
            .column = 6,
        },
        .message = "Invalid operand '-7.5' for 'mag' operator; expected an expression."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

TEST_CASE("PALM Parsing of Magnitude with Identifier Operand", "[PALM][Parser][Operator][Magnitude]")
{
    const auto expr = Oasis::FromPALM("( mag x )");

    auto error = expr.error();
    auto expectedError = Oasis::PALMParseError {
        .type = Oasis::PALMParseError::PALMParseErrorType::InvalidOperand,
        .token = Oasis::PALMToken {
            .type = Oasis::PALMTokenType::Identifier,
            .lexeme = "x",
            .charOffset = 5,
            .tokenIndex = 2,
            .line = 1,
            .column = 6,
        },
        .message = "Invalid operand 'x' for 'mag' operator; expected an expression."
    };

    REQUIRE(!expr);
    REQUIRE(error == expectedError);
}

// /* Beware, entering land of to be migrated */
/*
 * Test Invalid Expressions
 */
// TEST_CASE("Parse Invalid Expression", "[PALM][Parsing]")
// {
//     const auto expr = Oasis::FromPALMOld("( unknown_op ( real 5 ) ( real 3 ) )");
//     REQUIRE(!expr);
//     REQUIRE(expr.error() == Oasis::ParseErrorOld::UnknownOperator);
// }
//
// TEST_CASE("Parse Empty Expression", "[PALM][Parsing]")
// {
//     const auto expr = Oasis::FromPALMOld("");
//     REQUIRE(!expr);
//     REQUIRE(expr.error() == Oasis::ParseErrorOld::UnexpectedEndOfInput);
// }

// Note to self, make some tests for spacing and padding and all that fun stuff
// And end of input
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
// // TODO: Remove
// TEST_CASE("TEMP RUN PALM")
// {
//     const std::string palmString = "(+(real 5) (* (var x ) \n ( ^ ( real 2 ) ( real 3 ) ) ) )";
//
//     const std::string palmString2 = "( real three.point.onefour )";
//     const auto expr = Oasis::FromPALM(palmString2);
// }
//
//
// TEST_CASE("Parse Subtraction Multiple Elements", "[PALM][Parsing]")
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
// /*
//  * Mixed Addition and Subtraction
//  */
// TEST_CASE("Parse Mixed Addition and Subtraction", "[PALM][Parsing]")
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
// TEST_CASE("Parse Mixed Addition and Subtraction 2", "[PALM][Parsing]")
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
// TEST_CASE("Parse Mixed Addition and Subtraction Multiple Elements", "[PALM][Parsing]")
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
// TEST_CASE("Parse Mixed Addition, Subtraction, and Multiplication", "[PALM][Parsing]")
// {
//     const Oasis::Add expected {
//         Oasis::Real { 5.0 },
//         Oasis::Subtract {
//             Oasis::Real { 3.0 },
//             Oasis::Multiply {
//                 Oasis::Real { 2.0 },
//                 Oasis::Real { 4.0 } } }
//     };
//
//     OASIS_CAPTURE_WITH_SERIALIZER(expected);
//
//     const auto expr = Oasis::FromPALMOld("( + ( real 5 ) ( - ( real 3 ) ( * ( real 2 ) ( real 4 ) ) ) )");
//     REQUIRE(expr.has_value());
//     REQUIRE((*expr)->Equals(expected));
// }
// TEST_CASE("Parse Mixed Addition, Subtraction, Multiplication, and Division", "[PALM][Parsing]")
// {
//     const Oasis::Add expected {
//         Oasis::Real { 5.0 },
//         Oasis::Subtract {
//             Oasis::Real { 3.0 },
//             Oasis::Multiply {
//                 Oasis::Real { 2.0 },
//                 Oasis::Divide {
//                     Oasis::Real { 4.0 },
//                     Oasis::Real { 2.0 } } } }
//     };
//
//     OASIS_CAPTURE_WITH_SERIALIZER(expected);
//
//     const auto expr = Oasis::FromPALMOld("( + ( real 5 ) ( - ( real 3 ) ( * ( real 2 ) ( / ( real 4 ) ( real 2 ) ) ) ) )");
//     REQUIRE(expr.has_value());
//     REQUIRE((*expr)->Equals(expected));
// }
// TEST_CASE("Parse Mixed Addition, Subtraction, Multiplication, Division, and Exponent", "[PALM][Parsing]")
// {
//     const Oasis::Add expected {
//         Oasis::Real { 5.0 },
//         Oasis::Subtract {
//             Oasis::Real { 3.0 },
//             Oasis::Multiply {
//                 Oasis::Real { 2.0 },
//                 Oasis::Divide {
//                     Oasis::Real { 4.0 },
//                     Oasis::Exponent {
//                         Oasis::Real { 2.0 },
//                         Oasis::Real { 3.0 } } } } }
//     };
//
//     OASIS_CAPTURE_WITH_SERIALIZER(expected);
//
//     const auto expr = Oasis::FromPALMOld("( + ( real 5 ) ( - ( real 3 ) ( * ( real 2 ) ( / ( real 4 ) ( ^ ( real 2 ) ( real 3 ) ) ) ) ) )");
//     REQUIRE(expr.has_value());
//     REQUIRE((*expr)->Equals(expected));
// }
// TEST_CASE("Parse Mixed Many", "[PALM][Parsing]")
// {
//     const Oasis::Add expected {
//         Oasis::Real { 5.0 },
//         Oasis::Subtract {
//             Oasis::Real { 3.0 },
//             Oasis::Multiply {
//                 Oasis::Real { 2.0 },
//                 Oasis::Divide {
//                     Oasis::Real { 4.0 },
//                     Oasis::Exponent {
//                         Oasis::Real { 2.0 },
//                         Oasis::Log {
//                             Oasis::Real { 2.0 },
//                             Oasis::Real { 8.0 } } } } } }
//     };
//
//     OASIS_CAPTURE_WITH_SERIALIZER(expected);
//
//     const auto expr = Oasis::FromPALMOld("( + ( real 5 ) ( - ( real 3 ) ( * ( real 2 ) ( / ( real 4 ) ( ^ ( real 2 ) ( log ( real 2 ) ( real 8 ) ) ) ) ) ) )");
//     REQUIRE(expr.has_value());
//     REQUIRE((*expr)->Equals(expected));
// }
// TEST_CASE("Parse Mixed Integral", "[PALM][Parsing]")
// {
//     const Oasis::Add expected {
//         Oasis::Real { 5.0 },
//         Oasis::Subtract {
//             Oasis::Real { 3.0 },
//             Oasis::Multiply {
//                 Oasis::Real { 2.0 },
//                 Oasis::Divide {
//                     Oasis::Real { 4.0 },
//                     Oasis::Exponent {
//                         Oasis::Real { 2.0 },
//                         Oasis::Log {
//                             Oasis::Real { 2.0 },
//                             Oasis::Integral {
//                                 Oasis::Variable { "x" },
//                                 Oasis::Real { 8.0 } } } } } } }
//     };
//
//     OASIS_CAPTURE_WITH_SERIALIZER(expected);
//
//     const auto expr = Oasis::FromPALMOld("( + ( real 5 ) ( - ( real 3 ) ( * ( real 2 ) ( / ( real 4 ) ( ^ ( real 2 ) ( log ( real 2 ) ( int ( var x ) ( real 8 ) ) ) ) ) ) ) )");
//     REQUIRE(expr.has_value());
//     REQUIRE((*expr)->Equals(expected));
// }
// TEST_CASE("Parse Mixed Derivative", "[PALM][Parsing]")
// {
//     const Oasis::Add expected {
//         Oasis::Real { 5.0 },
//         Oasis::Subtract {
//             Oasis::Real { 3.0 },
//             Oasis::Multiply {
//                 Oasis::Real { 2.0 },
//                 Oasis::Divide {
//                     Oasis::Real { 4.0 },
//                     Oasis::Exponent {
//                         Oasis::Real { 2.0 },
//                         Oasis::Log {
//                             Oasis::Real { 2.0 },
//                             Oasis::Derivative {
//                                 Oasis::Variable { "x" },
//                                 Oasis::Real { 8.0 } } } } } } }
//     };
//
//     OASIS_CAPTURE_WITH_SERIALIZER(expected);
//
//     const auto expr = Oasis::FromPALMOld("( + ( real 5 ) ( - ( real 3 ) ( * ( real 2 ) ( / ( real 4 ) ( ^ ( real 2 ) ( log ( real 2 ) ( d ( var x ) ( real 8 ) ) ) ) ) ) ) )");
//     REQUIRE(expr.has_value());
//     REQUIRE((*expr)->Equals(expected));
// }

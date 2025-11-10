#include "../../tests/Common.hpp"

#include "Oasis/RecursiveCast.hpp"
#include "catch2/catch_test_macros.hpp"

#include "Oasis/FromPALM.hpp"
#include "Oasis/PALMSerializer.hpp"

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

// Serializer
TEST_CASE("PALM Serialization of Real with various precisions", "[PALM][Serializer][Real]")
{
    const Oasis::Real real { 3.1415926535 };

    { // Test with 5 decimal places
        Oasis::PALMSerializer serializer { { .numPlaces = 5 } };

        auto result = real.Accept(serializer).value();
        std::string expected = "( real 3.14159 )";

        REQUIRE(expected == result);
    }

    { // Test with 2 decimal places
        Oasis::PALMSerializer serializer { { .numPlaces = 2 } };

        auto result = real.Accept(serializer).value();
        std::string expected = "( real 3.14 )";

        REQUIRE(expected == result);
    }

    { // Test with 0 decimal places
        Oasis::PALMSerializer serializer { { .numPlaces = 0 } };

        auto result = real.Accept(serializer).value();
        std::string expected = "( real 3 )";

        REQUIRE(expected == result);
    }

    { // Test Default (5 decimal places)}
        Oasis::PALMSerializer serializer {};

        auto result = real.Accept(serializer).value();
        std::string expected = "( real 3.14159 )";

        REQUIRE(expected == result);
    }
}

TEST_CASE("PALM Serialization of Real with negative value", "[PALM][Serializer][Real]")
{
    const Oasis::Real real { -2.71828 };

    Oasis::PALMSerializer serializer {};

    auto result = real.Accept(serializer).value();
    std::string expected = "( real -2.71828 )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Imaginary with different characters", "[PALM][Serializer][Imaginary]")
{
    const Oasis::Imaginary imaginary {};

    { // Test with default character 'i'
        Oasis::PALMSerializer serializer { { .imaginarySymbol = Oasis::PALMSerializationOpts::ImgSymType::I } };

        auto result = imaginary.Accept(serializer).value();
        std::string expected = "( i )";

        REQUIRE(expected == result);
    }

    { // Test with character 'j'
        Oasis::PALMSerializer serializer { { .imaginarySymbol = Oasis::PALMSerializationOpts::ImgSymType::J } };

        auto result = imaginary.Accept(serializer).value();
        std::string expected = "( j )";

        REQUIRE(expected == result);
    }

    { // Test Default
        Oasis::PALMSerializer serializer {};

        auto result = imaginary.Accept(serializer).value();
        std::string expected = "( i )";

        REQUIRE(expected == result);
    }
}

TEST_CASE("PALM Serialization of Variable", "[PALM][Serializer][Variable]")
{
    const Oasis::Variable variable { "x_variable" };

    Oasis::PALMSerializer serializer {};

    auto result = variable.Accept(serializer).value();
    std::string expected = "( var x_variable )";

    REQUIRE(expected == result);
}

TEST_CASE("PALM Serialization of Addition", "[PALM][Serializer][Addition]")
{
    const Oasis::Add<> addition {
        Oasis::Real { 5.0 },
        Oasis::Real { 3.0 }
    };

    Oasis::PALMSerializer serializer {};

    auto result = addition.Accept(serializer).value();
    std::string expected = "( + ( real 5 ) ( real 3 ) )";

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

TEST_CASE("PALM Serialization of Malformed Addition", "[PALM][Serializer][Addition]")
{
    { // Missing least significant operand
        Oasis::Add<Oasis::Real, Oasis::Expression> addition;
        addition.SetMostSigOp(Oasis::Real { 5.0 });

        Oasis::PALMSerializer serializer = Oasis::PALMSerializer();

        auto result = addition.Accept(serializer);

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Expression missing least significant operand");
    }

    { // Missing most significant operand
        Oasis::Add<Oasis::Expression, Oasis::Real> addition;
        addition.SetLeastSigOp(Oasis::Real { 3.0 });

        Oasis::PALMSerializer serializer = Oasis::PALMSerializer();

        auto result = addition.Accept(serializer);

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Expression missing most significant operand");
    }
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
                    Oasis::Real { 3.0 }
                }
            }
        };

        auto result = expression.Accept(serializer).value();
        std::string expected = "( + ( real 5 ) ( * ( var x ) ( ^ ( real 2 ) ( real 3 ) ) ) )";

        REQUIRE(expected == result);
    }

    { // Test 2
        const Oasis::Subtract<> expression {
            Oasis::Divide {
                Oasis::Real { 10.0 },
                Oasis::Real { 2.0 }
            },
            Oasis::Log {
                Oasis::Real { 10.0 },
                Oasis::Real { 100.0 }
            }
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
                    Oasis::Real { 3.0 }
                }
            }
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
                    Oasis::Real { 3.0 }
                }
            }
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
                    Oasis::Real { 3.0 }
                }
            }
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
                    Oasis::Real { 3.0 }
                }
            }
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
                    Oasis::Real { 3.0 }
                }
            }
        };

        auto result = expression.Accept(serializer).value();
        std::string expected = "( +%( real%5 )%( *%( var%x )%( ^%( real%2 )%( real%3 ) ) ) )";

        REQUIRE(expected == result);
    }
}

// Parser
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

// TEST_CASE("PALM Parsing for Malformed Real", "[PALM][Parser][Real]")
// {
//     { // Missing value
//         // const auto expr = Oasis::FromPALM("( real )");
//         // const auto expectedError = Oasis::ParseError {
//         //     .type = Oasis::ParseErrorType::TooFewOperands,
//         //     .token_index = 4,
//         //     .char_offset = 7,
//         //     .got = "",
//         //     .expected = "real number",
//         //     .message = "Expected a real number after 'real' token."
//         // };
//         //
//         // REQUIRE(!expr);
//         // REQUIRE(expr.error() == expectedError);
//     }
//
//     { // Invalid format
//         const auto expr = Oasis::FromPALM("( real three.point.onefour )");
//
//         REQUIRE(!expr);
//         REQUIRE(expr.error() == Oasis::ParseError::InvalidNumberFormat);
//     }
//
//     { // Extra tokens
//         const auto expr = Oasis::FromPALM("( real 3.14 extra )");
//
//         REQUIRE(!expr);
//         REQUIRE(expr.error() == Oasis::ParseError::UnexpectedToken);
//     }
//
//
// }

//TODO: Remove
TEST_CASE("TEMP RUN PALM")
{
    const std::string palmString = "(+(rea%l 5) (* (var x ) \n ( ^ ( real 2 ) ( real 3 ) ) ) )";

    const auto expr = Oasis::FromPALMNew(palmString);
}


/*
 * Real Operation
 */


TEST_CASE("Parse Multiple Real", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALM("( real 5 3 )");

    REQUIRE(!expr);
    REQUIRE(expr.error() == Oasis::ParseError::MissingClosingParen);
}

TEST_CASE("Parse Negative Integer", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALM("( real -5 )");

    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Is<Oasis::Real>());

    const auto real = Oasis::RecursiveCast<Oasis::Real>(**expr);
    REQUIRE(real->GetValue() == -5.0);
}

TEST_CASE("Parse Float", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALM("( real 5.5 )");

    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Is<Oasis::Real>());

    const auto real = Oasis::RecursiveCast<Oasis::Real>(**expr);
    REQUIRE(real->GetValue() == 5.5);
}

TEST_CASE("Parse Float with Exponent", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALM("( real 5.5e3 )");

    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Is<Oasis::Real>());

    const auto real = Oasis::RecursiveCast<Oasis::Real>(**expr);
    REQUIRE(real->GetValue() == 5500.0);
}

TEST_CASE("Parse Invalid Real", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALM("( real five )");

    REQUIRE(!expr);
    REQUIRE(expr.error() == Oasis::ParseError::InvalidNumberFormat);
}

TEST_CASE("Parse Out of Range Real", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALM("( real 1e5000 )");

    REQUIRE(!expr);
    REQUIRE(expr.error() == Oasis::ParseError::InvalidNumberFormat);
}

/*
 * Imaginary Operation
 */
TEST_CASE("Parse Imaginary i", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALM("( i )");

    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Is<Oasis::Imaginary>());
}

TEST_CASE("Parse Imaginary j", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALM("( j )");

    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Is<Oasis::Imaginary>());
}

/*
 * Variable Operation
 */
TEST_CASE("Parse Single Letter Variable", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALM("( var x )");

    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Is<Oasis::Variable>());

    const auto var = Oasis::RecursiveCast<Oasis::Variable>(**expr);
    REQUIRE(var->GetName() == "x");
}

TEST_CASE("Parse Multi Letter Variable", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALM("( var myVariable )");

    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Is<Oasis::Variable>());

    const auto var = Oasis::RecursiveCast<Oasis::Variable>(**expr);
    REQUIRE(var->GetName() == "myVariable");
}

TEST_CASE("Parse Variable With Numbers", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALM("( var var123 )");

    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Is<Oasis::Variable>());

    const auto var = Oasis::RecursiveCast<Oasis::Variable>(**expr);
    REQUIRE(var->GetName() == "var123");
}

TEST_CASE("Parse Variable With Underscore", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALM("( var var_name )");

    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Is<Oasis::Variable>());

    const auto var = Oasis::RecursiveCast<Oasis::Variable>(**expr);
    REQUIRE(var->GetName() == "var_name");
}

TEST_CASE("Parse without Variable Name", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALM("( var )");

    REQUIRE(!expr);
    REQUIRE(expr.error() == Oasis::ParseError::IncompleteExpression);
}

/*
 * Addition Operation
 */
TEST_CASE("Parse Simple Addition", "[FromPALM][Parsing]")
{
    const Oasis::Add expected {
        Oasis::Real { 5.0 },
        Oasis::Real { 3.0 }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALM("( + ( real 5 ) ( real 3 ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Nested Addition", "[FromPALM][Parsing]")
{
    const Oasis::Add expected {
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

TEST_CASE("Parse Malformed Addition", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALM("( + ( real 5 ) )");
    REQUIRE(!expr);
    REQUIRE(expr.error() == Oasis::ParseError::IncompleteExpression);
}

TEST_CASE("Parse Addition Multiple Elements", "[FromPALM][Parsing]")
{
    const Oasis::Add<> expected {
        Oasis::Real { 5.0 },
        Oasis::Real { 3.0 },
        Oasis::Real { 2.0 },
        Oasis::Real { 1.0 }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    auto expr = Oasis::FromPALM("( + ( real 5 ) ( real 3 ) ( real 2 ) ( real 1 ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Missing Parens", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALM("( real 5 ");

    REQUIRE(!expr);
    REQUIRE(expr.error() == Oasis::ParseError::MissingClosingParen);
}

TEST_CASE("Parse Expression Unexpected Token", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALM("'");

    REQUIRE(!expr);
    REQUIRE(expr.error() == Oasis::ParseError::UnexpectedToken);
}

TEST_CASE("Parse Incomplete Expression", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALM("( real 5 ) e ");

    REQUIRE(!expr);
    REQUIRE(expr.error() == Oasis::ParseError::UnexpectedToken);
}

/*
 * Subtraction Operation
 */
TEST_CASE("Parse Simple Subtraction", "[FromPALM][Parsing]")
{
    const Oasis::Subtract expected {
        Oasis::Real { 5.0 },
        Oasis::Real { 3.0 }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALM("( - ( real 5 ) ( real 3 ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Nested Subtraction", "[FromPALM][Parsing]")
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

TEST_CASE("Parse Subtraction Multiple Elements", "[FromPALM][Parsing]")
{
    const Oasis::Subtract<> expected {
        Oasis::Real { 5.0 },
        Oasis::Subtract {
            Oasis::Real { 3.0 },
            Oasis::Subtract {
                Oasis::Real { 2.0 },
                Oasis::Real { 1.0 } } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALM("( - ( real 5 ) ( real 3 ) ( real 2 ) ( real 1 ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Malformed Subtraction", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALM("( - ( real 5 ) )");
    REQUIRE(!expr);
    REQUIRE(expr.error() == Oasis::ParseError::IncompleteExpression);
}

TEST_CASE("Parse Subtraction Invalid Subexpression", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALM("( - ( real 5 ) ( + ( real 3 ) ) )");
    REQUIRE(!expr);
    REQUIRE(expr.error() == Oasis::ParseError::IncompleteExpression);
}

/*
 * Mixed Addition and Subtraction
 */
TEST_CASE("Parse Mixed Addition and Subtraction", "[FromPALM][Parsing]")
{
    const Oasis::Add expected {
        Oasis::Real { 5.0 },
        Oasis::Subtract {
            Oasis::Real { 3.0 },
            Oasis::Real { 2.0 } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALM("( + ( real 5 ) ( - ( real 3 ) ( real 2 ) ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Mixed Addition and Subtraction 2", "[FromPALM][Parsing]")
{
    const Oasis::Subtract expected {
        Oasis::Real { 5.0 },
        Oasis::Add {
            Oasis::Real { 3.0 },
            Oasis::Real { 2.0 } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALM("( - ( real 5 ) ( + ( real 3 ) ( real 2 ) ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Mixed Addition and Subtraction Multiple Elements", "[FromPALM][Parsing]")
{
    const Oasis::Add<> expected {
        Oasis::Real { 5.0 },
        Oasis::Subtract {
            Oasis::Real { 3.0 },
            Oasis::Add {
                Oasis::Real { 2.0 },
                Oasis::Real { 1.0 } } }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(expected);

    const auto expr = Oasis::FromPALM("( + ( real 5 ) ( - ( real 3 ) ( + ( real 2 ) ( real 1 ) ) ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

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

    const auto expr = Oasis::FromPALM("( * ( real 5 ) ( real 3 ) )");
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

    const auto expr = Oasis::FromPALM("( * ( real 5 ) ( * ( real 3 ) ( real 2 ) ) )");
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

    const auto expr = Oasis::FromPALM("( * ( real 5 ) ( real 3 ) ( real 2 ) ( real 1 ) )");
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

    const auto expr = Oasis::FromPALM("( + ( real 5 ) ( - ( real 3 ) ( * ( real 2 ) ( real 4 ) ) ) )");
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

    const auto expr = Oasis::FromPALM("( / ( real 6 ) ( real 3 ) )");
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

    const auto expr = Oasis::FromPALM("( / ( real 6 ) ( / ( real 3 ) ( real 2 ) ) )");
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

    const auto expr = Oasis::FromPALM("( / ( real 6 ) ( real 3 ) ( real 2 ) ( real 1 ) )");
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

    const auto expr = Oasis::FromPALM("( + ( real 5 ) ( - ( real 3 ) ( * ( real 2 ) ( / ( real 4 ) ( real 2 ) ) ) ) )");
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

    const auto expr = Oasis::FromPALM("( ^ ( real 2 ) ( real 3 ) )");
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

    const auto expr = Oasis::FromPALM("( ^ ( real 2 ) ( ^ ( real 3 ) ( real 2 ) ) )");
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

    const auto expr = Oasis::FromPALM("( ^ ( real 2 ) ( real 3 ) ( real 2 ) ( real 1 ) )");
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

    const auto expr = Oasis::FromPALM("( + ( real 5 ) ( - ( real 3 ) ( * ( real 2 ) ( / ( real 4 ) ( ^ ( real 2 ) ( real 3 ) ) ) ) ) )");
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

    const auto expr = Oasis::FromPALM("( log ( real 2 ) ( real 8 ) )");
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

    const auto expr = Oasis::FromPALM("( log ( real 2 ) ( log ( real 3 ) ( real 9 ) ) )");
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

    const auto expr = Oasis::FromPALM("( log ( real 2 ) ( real 3 ) ( real 4 ) ( real 16 ) )");
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

    const auto expr = Oasis::FromPALM("( + ( real 5 ) ( - ( real 3 ) ( * ( real 2 ) ( / ( real 4 ) ( ^ ( real 2 ) ( log ( real 2 ) ( real 8 ) ) ) ) ) ) )");
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

    const auto expr = Oasis::FromPALM("( int ( var x ) ( real 0 ) )");
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

    const auto expr = Oasis::FromPALM("( int ( var x ) ( int ( var y ) ( real 0 ) ) )");
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

    const auto expr = Oasis::FromPALM("( int ( var x ) ( var y ) ( int ( var z ) ( real 0 ) ) )");
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

    const auto expr = Oasis::FromPALM("( + ( real 5 ) ( - ( real 3 ) ( * ( real 2 ) ( / ( real 4 ) ( ^ ( real 2 ) ( log ( real 2 ) ( int ( var x ) ( real 8 ) ) ) ) ) ) ) )");
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

    const auto expr = Oasis::FromPALM("( neg ( real 5 ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

TEST_CASE("Parse Invalid Negate", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALM("( neg )");
    REQUIRE(!expr);
    REQUIRE(expr.error() == Oasis::ParseError::UnexpectedToken);
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

    const auto expr = Oasis::FromPALM("( d ( var x ) ( real 0 ) )");
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

    const auto expr = Oasis::FromPALM("( d ( var x ) ( d ( var y ) ( real 0 ) ) )");
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

    const auto expr = Oasis::FromPALM("( d ( var x ) ( var y ) ( d ( var z ) ( real 0 ) ) )");
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

    const auto expr = Oasis::FromPALM("( + ( real 5 ) ( - ( real 3 ) ( * ( real 2 ) ( / ( real 4 ) ( ^ ( real 2 ) ( log ( real 2 ) ( d ( var x ) ( real 8 ) ) ) ) ) ) ) )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Equals(expected));
}

/*
 * Parse Constants Pi and Euler's Number
 */
TEST_CASE("Parse Constant Pi (pi)", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALM("( pi )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Is<Oasis::Pi>());
}

TEST_CASE("Parse Constant Euler's Number", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALM("( e )");
    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Is<Oasis::EulerNumber>());
}

/*
 * Test Invalid Expressions
 */
TEST_CASE("Parse Invalid Expression", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALM("( unknown_op ( real 5 ) ( real 3 ) )");
    REQUIRE(!expr);
    REQUIRE(expr.error() == Oasis::ParseError::UnknownOperator);
}

TEST_CASE("Parse Empty Expression", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALM("");
    REQUIRE(!expr);
    REQUIRE(expr.error() == Oasis::ParseError::UnexpectedEndOfInput);
}
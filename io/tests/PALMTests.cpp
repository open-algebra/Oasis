#include "../../tests/Common.hpp"

#include "Oasis/RecursiveCast.hpp"
#include "catch2/catch_test_macros.hpp"

#include "Oasis/FromPALM.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Derivative.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/EulerNumber.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Matrix.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Negate.hpp"
#include "Oasis/Pi.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"

/*
 * Real Operation
 */
TEST_CASE("Parse Real", "[FromPALM][Parsing]")
{
    const auto expr = Oasis::FromPALM("( real 5 )");

    REQUIRE(expr.has_value());
    REQUIRE((*expr)->Is<Oasis::Real>());

    const auto real = Oasis::RecursiveCast<Oasis::Real>(**expr);
    REQUIRE(real->GetValue() == 5.0);
}

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
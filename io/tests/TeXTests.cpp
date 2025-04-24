/**
 * Created by Andrew Nazareth on 9/30/24.
 */

#include "catch2/catch_test_macros.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Derivative.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/EulerNumber.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/FromString.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Magnitude.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Negate.hpp"
#include "Oasis/Pi.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/TeXSerializer.hpp"
#include "Oasis/Variable.hpp"

TEST_CASE("LaTeX serialization for various precision", "[LaTeX][Serializer][Real]"){
    Oasis::Real r{111215.0123456};

    Oasis::TeXSerializer serializer1 { { .numPlaces = 5 } };
    Oasis::TeXSerializer serializer2 { { .numPlaces = 2 } };

    auto actual1 = r.Accept(serializer1).value();
    auto actual2 = r.Accept(serializer2).value();
    std::string expected1 = "111215";
    std::string expected2 = "1.11e+05";

    REQUIRE(actual1 == expected1);
    REQUIRE(actual2 == expected2);
}

TEST_CASE("LaTeX serialization for different imaginary character", "[LaTeX][Serializer][Imaginary]"){
    Oasis::Imaginary i{};

    Oasis::TeXSerializer serializer {};
    Oasis::TeXSerializer serializerJ { { .character = Oasis::TeXOpts::ImgSym::J } };

    auto actual = i.Accept(serializer).value();
    auto actualJ = i.Accept(serializerJ).value();

    std::string expected = "i";
    std::string expectedJ = "j";

    REQUIRE(actual == expected);
    REQUIRE(actualJ == expectedJ);

}

TEST_CASE("LaTeX Serialization for Addition", "[LaTeX][Serializer][Add]"){
    Oasis::Add a{Oasis::Real{5.0}, Oasis::Variable{"x_0"}};

    Oasis::TeXSerializer serializer;

    auto result = a.Accept(serializer).value();
    std::string expected = "\\left(5+x_0\\right)";

    REQUIRE(expected == result);
}

TEST_CASE("LaTeX Serialization for Subtraction", "[LaTeX][Serializer][Subtract]"){
    Oasis::Subtract s{Oasis::Real{5.0}, Oasis::Variable{"x_0"}};

    Oasis::TeXSerializer serializer;

    auto result = s.Accept(serializer).value();
    std::string expected = "\\left(5-x_0\\right)";

    REQUIRE(expected == result);
}

TEST_CASE("LaTeX Serialization for Multiplication", "[LaTeX][Serializer][Multiply]")
{
    Oasis::Multiply s{Oasis::Real{5.0}, Oasis::Variable{"x_0"}};

    Oasis::TeXSerializer serializer;

    auto result = s.Accept(serializer).value();
    std::string expected = "\\left(5*x_0\\right)";

    REQUIRE(expected == result);
}

TEST_CASE("LaTeX Serialization for Division", "[LaTeX][Serializer][Divide]")
{
    Oasis::Divide d{Oasis::Real{5.0}, Oasis::Variable{"x_0"}};

    Oasis::TeXSerializer serializer;

    auto result = d.Accept(serializer).value();
    std::string expected = R"(\left(\frac{5}{x_0}\right))";

    REQUIRE(expected == result);
}

TEST_CASE("LaTeX Serialization for Division with \\div", "[LaTeX][Serializer][Division]")
{
    Oasis::Divide d{Oasis::Real{5.0}, Oasis::Variable{"x_0"}};

    Oasis::TeXSerializer serializer { { .divType = Oasis::TeXOpts::DivType::DIV } };
    assert(serializer.GetDivType() == Oasis::TeXOpts::DivType::DIV);

    auto result = d.Accept(serializer).value();
    std::string expected = R"(\left({5}\div{x_0}\right))";

    REQUIRE(expected == result);
}

TEST_CASE("LaTeX Serialization with spacing options", "[LaTeX][Serializer][TexOptions]")
{
    Oasis::Multiply s{Oasis::Real{5.0}, Oasis::Variable{"x_0"}};

    Oasis::TeXOpts options {.spacing = Oasis::TeXOpts::Spacing::REGULAR };
    Oasis::TeXSerializer serializer{options};

    auto result = s.Accept(serializer).value();
    std::string expected = "\\left(5 * x_0\\right)";

    REQUIRE(expected == result);
}

TEST_CASE("LaTeX Serialization for Exponents", "[LaTeX][Serializer][Exponent]")
{
    Oasis::Exponent e{Oasis::EulerNumber{}, Oasis::Multiply{Oasis::Imaginary{}, Oasis::Variable{"x"}}};

    Oasis::TeXSerializer serializer{};

    auto result = e.Accept(serializer).value();
    std::string expected = R"(\left(e\right)^{\left(i*x\right)})";

    REQUIRE(expected == result);

}

TEST_CASE("LaTeX Serialization for Logarithms", "[LaTeX][Serializer][Log]")
{
    Oasis::Log e{Oasis::EulerNumber{}, Oasis::Variable{"x"}};

    Oasis::TeXSerializer serializer{};

    auto result = e.Accept(serializer).value();
    std::string expected = R"(\log_{e}\left(x\right))";

    REQUIRE(expected == result);

}

TEST_CASE("LaTeX Serialization for Negate", "[LaTeX][Serializer][Negate]")
{
    Oasis::Negate<Oasis::Expression> e{Oasis::EulerNumber{}};

    Oasis::TeXSerializer serializer{};

    auto result = e.Accept(serializer).value();
    std::string expected = R"(-\left(e\right))";

    REQUIRE(expected == result);

}

TEST_CASE("LaTeX Serialization for Magnitude", "[LaTeX][Serializer][Magnitude]")
{
    Oasis::Magnitude e{Oasis::Real{-5}};

    Oasis::TeXSerializer serializer{};

    auto result = e.Accept(serializer).value();
    std::string expected = R"(\left|-5\right|)";

    REQUIRE(expected == result);

}

TEST_CASE("LaTeX Serialization for Derivative", "[LaTeX][Serializer][Derivative]")
{
    Oasis::Derivative e{Oasis::Variable{"x"}, Oasis::Variable{"x"}};

    Oasis::TeXSerializer serializer{};

    auto result = e.Accept(serializer).value();
    std::string expected = R"(\frac{d}{dx}\left(x\right))";

    REQUIRE(expected == result);

}

TEST_CASE("LaTeX Serialization for Integral", "[LaTeX][Serializer][Integral]")
{
    Oasis::Integral e{Oasis::Variable{"x"}, Oasis::Variable{"x"}};

    Oasis::TeXSerializer serializer{};

    auto result = e.Accept(serializer).value();
    std::string expected = R"(\int\left(x\right)dx)";

    REQUIRE(expected == result);

}

TEST_CASE("LaTeX Serialization for Matrices", "[LaTeX][Serializer][Matrix]")
{
    Oasis::Matrix mat{Oasis::MatrixXXD{{1,2,3},{4,5,6.2}}};
    Oasis::TeXSerializer serializer{};

    auto result =mat.Accept(serializer).value();
    auto expected = R"(\begin{bmatrix}
1&2&3\\
4&5&6.2\\
\end{bmatrix}
)";

    REQUIRE(expected == result);
}
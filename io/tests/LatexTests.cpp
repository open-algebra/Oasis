//
// Created by Andrew Nazareth on 9/30/24.
//

#include "catch2/catch_test_macros.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/EulerNumber.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/FromString.hpp"
#include "Oasis/LatexSerializer.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Pi.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"
#include "Oasis/Negate.hpp"
#include "Oasis/Magnitude.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Derivative.hpp"

TEST_CASE("LaTeX serialization for various precision", "[LaTeX][Serializer][Real]"){
    Oasis::Real r{111215.0123456};
    Oasis::LatexOptions options1 {};
    Oasis::LatexSerializer serializer1{options1};

    Oasis::LatexOptions options2 {Oasis::CHARACTER_I, 2};
    Oasis::LatexSerializer serializer2{options2};

    r.Serialize(serializer1);
    r.Serialize(serializer2);

    auto actual1 = serializer1.getResult();
    auto actual2 = serializer2.getResult();
    std::string expected1 = "111215";
    std::string expected2 = "1.11e+05";

    REQUIRE(actual1 == expected1);
    REQUIRE(actual2 == expected2);
}

TEST_CASE("LaTeX serialization for different imaginary character", "[LaTeX][Serializer][Imaginary]"){
    Oasis::Imaginary i{};

    Oasis::LatexSerializer serializer{};
    Oasis::LatexSerializer serializerJ{Oasis::LatexOptions{Oasis::CHARACTER_J}};

    i.Serialize(serializer);
    i.Serialize(serializerJ);

    auto actual = serializer.getResult();
    auto actualJ = serializerJ.getResult();

    std::string expected = "i";
    std::string expectedJ = "j";

    REQUIRE(actual == expected);
    REQUIRE(actualJ == expectedJ);

}

TEST_CASE("Latex Serialization for Addition", "[Latex][Serializer][Add]"){
    Oasis::Add a{Oasis::Real{5.0}, Oasis::Variable{"x_0"}};

    Oasis::LatexSerializer serializer;

    a.Serialize(serializer);

    auto result = serializer.getResult();
    std::string expected = "\\left(5+x_0\\right)";

    REQUIRE(expected == result);
}

TEST_CASE("Latex Serialization for Subtraction", "[Latex][Serializer][Subtract]"){
    Oasis::Subtract s{Oasis::Real{5.0}, Oasis::Variable{"x_0"}};

    Oasis::LatexSerializer serializer;

    s.Serialize(serializer);

    auto result = serializer.getResult();
    std::string expected = "\\left(5-x_0\\right)";

    REQUIRE(expected == result);
}

TEST_CASE("Latex Serialization for Multiplication", "[Latex][Serializer][Multiply]")
{
    Oasis::Multiply s{Oasis::Real{5.0}, Oasis::Variable{"x_0"}};

    Oasis::LatexSerializer serializer;

    s.Serialize(serializer);

    auto result = serializer.getResult();
    std::string expected = "\\left(5*x_0\\right)";

    REQUIRE(expected == result);
}

TEST_CASE("Latex Serialization for Division", "[Latex][Serializer][Divide]")
{
    Oasis::Divide d{Oasis::Real{5.0}, Oasis::Variable{"x_0"}};

    Oasis::LatexSerializer serializer;

    d.Serialize(serializer);

    auto result = serializer.getResult();
    std::string expected = R"(\left(\frac{5}{x_0}\right))";

    REQUIRE(expected == result);
}

TEST_CASE("Latex Serialization with spacing options", "[Latex][Serializer][LatexOptions]")
{
    Oasis::Multiply s{Oasis::Real{5.0}, Oasis::Variable{"x_0"}};

    Oasis::LatexOptions options{Oasis::CHARACTER_I, 5, Oasis::SPACING};

    Oasis::LatexSerializer serializer{options};

    s.Serialize(serializer);

    auto result = serializer.getResult();
    std::string expected = "\\left(5 * x_0\\right)";

    REQUIRE(expected == result);
}

TEST_CASE("Latex Serialization for Exponents", "[Latex][Serializer][Exponent]")
{
    Oasis::Exponent e{Oasis::EulerNumber{}, Oasis::Multiply{Oasis::Imaginary{}, Oasis::Variable{"x"}}};

    Oasis::LatexSerializer serializer{};

    e.Serialize(serializer);

    auto result = serializer.getResult();
    std::string expected = R"(\left(e\right)^{\left(i*x\right)})";

    REQUIRE(expected == result);

}

TEST_CASE("Latex Serialization for Logarithms", "[Latex][Serializer][Log]")
{
    Oasis::Log e{Oasis::EulerNumber{}, Oasis::Variable{"x"}};

    Oasis::LatexSerializer serializer{};

    e.Serialize(serializer);

    auto result = serializer.getResult();
    std::string expected = R"(\log_{e}\left(x\right))";

    REQUIRE(expected == result);

}

TEST_CASE("Latex Serialization for Negate", "[Latex][Serializer][Negate]")
{
    Oasis::Negate<Oasis::Expression> e{Oasis::EulerNumber{}};

    Oasis::LatexSerializer serializer{};

    e.Serialize(serializer);

    auto result = serializer.getResult();
    std::string expected = R"(\left(-e\right))";

    REQUIRE(expected == result);

}

TEST_CASE("Latex Serialization for Magnitude", "[Latex][Serializer][Magnitude]")
{
    Oasis::Magnitude e{Oasis::Real{-5}};

    Oasis::LatexSerializer serializer{};

    e.Serialize(serializer);

    auto result = serializer.getResult();
    std::string expected = R"(\left|-5\right|)";

    REQUIRE(expected == result);

}

TEST_CASE("Latex Serialization for Derivative", "[Latex][Serializer][Derivative]")
{
    Oasis::Derivative e{Oasis::Variable{"x"}, Oasis::Variable{"x"}};

    Oasis::LatexSerializer serializer{};

    e.Serialize(serializer);

    auto result = serializer.getResult();
    std::string expected = R"(\frac{d}{dx}\left(x\right))";

    REQUIRE(expected == result);

}

TEST_CASE("Latex Serialization for Integral", "[Latex][Serializer][Integral]")
{
    Oasis::Integral e{Oasis::Variable{"x"}, Oasis::Variable{"x"}};

    Oasis::LatexSerializer serializer{};

    e.Serialize(serializer);

    auto result = serializer.getResult();
    std::string expected = R"(\int\left(x\right)dx)";

    REQUIRE(expected == result);

}

TEST_CASE("Latex Serialization for Matrices", "[Latex][Serializer][Matrix]")
{
    Oasis::Matrix mat{Oasis::MatrixXXD{{1,2,3},{4,5,6.2}}};
    Oasis::LatexSerializer serializer{};

    mat.Serialize(serializer);

    auto result = serializer.getResult();
    auto expected = R"(\begin{bmatrix}
1&2&3\\
4&5&6.2\\
\end{bmatrix}
)";

    REQUIRE(expected == result);
}
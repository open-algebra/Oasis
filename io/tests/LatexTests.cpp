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

    Oasis::LatexOptions options{Oasis::SPACING};

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

    std::cout<<result<<std::endl;
}

TEST_CASE("Latex Serialization for Logarithms", "[Latex][Serializer][Log]")
{
    Oasis::Log e{Oasis::EulerNumber{}, Oasis::Variable{"x"}};

    Oasis::LatexSerializer serializer{};

    e.Serialize(serializer);

    auto result = serializer.getResult();
    std::string expected = R"(\log_{e}\left(x\right))";

    REQUIRE(expected == result);

    std::cout<<result<<std::endl;
}

TEST_CASE("Latex Serialization for Negate", "[Latex][Serializer][Negate]")
{
    Oasis::Negate<Oasis::Expression> e{Oasis::EulerNumber{}};

    Oasis::LatexSerializer serializer{};

    e.Serialize(serializer);

    auto result = serializer.getResult();
    std::string expected = R"(\left(-e\right))";

    REQUIRE(expected == result);

    std::cout<<result<<std::endl;
}

TEST_CASE("Latex Serialization for Magnitude", "[Latex][Serializer][Magnitude]")
{
    Oasis::Magnitude e{Oasis::Real{5}};

    Oasis::LatexSerializer serializer{};

    e.Serialize(serializer);

    auto result = serializer.getResult();
    std::string expected = R"(\left|-5\right|)";

    REQUIRE(expected == result);

    std::cout<<result<<std::endl;
}

TEST_CASE("Latex Serialization for Derivative", "[Latex][Serializer][Derivative]")
{
    Oasis::Derivative e{Oasis::Variable{"x"}, Oasis::Variable{"x"}};

    Oasis::LatexSerializer serializer{};

    e.Serialize(serializer);

    auto result = serializer.getResult();
    std::string expected = R"(\frac{d}{dx}\left(x\right))";

    REQUIRE(expected == result);

    std::cout<<result<<std::endl;
}

TEST_CASE("Latex Serialization for Integral", "[Latex][Serializer][Integral]")
{
    Oasis::Integral e{Oasis::Variable{"x"}, Oasis::Variable{"x"}};

    Oasis::LatexSerializer serializer{};

    e.Serialize(serializer);

    auto result = serializer.getResult();
    std::string expected = R"(\int\left(x\right)dx)";

    REQUIRE(expected == result);

    std::cout<<result<<std::endl;
}
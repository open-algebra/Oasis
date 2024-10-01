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
    std::string expected = "\\left(\\frac{5}{x_0}\\right)";

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
    std::string expected = "\\left(e\\right)^{\\left(i*x\\right)}";

    REQUIRE(expected == result);

    std::cout<<result<<std::endl;
}
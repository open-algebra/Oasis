#include "catch2/catch_test_macros.hpp"

#include <Oasis/Add.hpp>
#include <Oasis/Divide.hpp>
#include <Oasis/Multiply.hpp>
#include <Oasis/Variable.hpp>
#include <Oasis/MathMLSerializer.hpp>

TEST_CASE("ToMathML Works", "[MathML]")
{
    Oasis::Divide divide {
        Oasis::Add {
            Oasis::Variable { "x" },
            Oasis::Variable { "y" } },
        Oasis::Multiply {
            Oasis::Variable { "z" },
            Oasis::Variable { "w" } }
    };

    tinyxml2::XMLDocument doc;
    Oasis::MathMLSerializer serializer(doc);

    divide.Serialize(serializer);
    doc.InsertEndChild(serializer.GetResult());

    tinyxml2::XMLPrinter printer;
    doc.Print(&printer);

    std::string mathml(printer.CStr());

    std::string expected = R"(<mfrac>
    <mrow>
        <mi>x</mi>
        <mo>+</mo>
        <mi>y</mi>
    </mrow>
    <mrow>
        <mi>z</mi>
        <mo>*</mo>
        <mi>w</mi>
    </mrow>
</mfrac>
)";

    REQUIRE(expected == mathml);
}
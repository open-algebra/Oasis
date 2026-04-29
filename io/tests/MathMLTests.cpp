#include "catch2/catch_test_macros.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Matrix.hpp"
#include "Oasis/Variable.hpp"
#include "Oasis/MathMLSerializer.hpp"
#include "Oasis/Magnitude.hpp"

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
    doc.InsertEndChild(divide.Accept(serializer).value());

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
        <mi>w</mi>
    </mrow>
</mfrac>
)";

    REQUIRE(expected == mathml);
}

TEST_CASE("Matrix to MathML 3x3", "[Matrix][MathML]")
{
Oasis::Matrix mat{Oasis::MatrixXXD{{1,2,3},{4,5,6},{7,8,9}}};

    tinyxml2::XMLDocument doc;
    Oasis::MathMLSerializer serializer(doc);
    doc.InsertEndChild(mat.Accept(serializer).value());

    tinyxml2::XMLPrinter printer;
    doc.Print(&printer);

    std::string mathml(printer.CStr());

    std::string expected = R"(<mrow>
    <mo>[</mo>
    <mtable>
        <mtr>
            <mtd>
                <mn>1</mn>
            </mtd>
            <mtd>
                <mn>2</mn>
            </mtd>
            <mtd>
                <mn>3</mn>
            </mtd>
        </mtr>
        <mtr>
            <mtd>
                <mn>4</mn>
            </mtd>
            <mtd>
                <mn>5</mn>
            </mtd>
            <mtd>
                <mn>6</mn>
            </mtd>
        </mtr>
        <mtr>
            <mtd>
                <mn>7</mn>
            </mtd>
            <mtd>
                <mn>8</mn>
            </mtd>
            <mtd>
                <mn>9</mn>
            </mtd>
        </mtr>
    </mtable>
    <mo>]</mo>
</mrow>
)";

    // std::cout<<mathml<<std::endl;
    REQUIRE(expected == mathml);
}

TEST_CASE("Matrix to MathML 2x2", "[Matrix][MathML]")
{
    Oasis::Matrix mat{Oasis::MatrixXXD{{1,2},{3,4}}};

    tinyxml2::XMLDocument doc;
    Oasis::MathMLSerializer serializer(doc);
    doc.InsertEndChild(mat.Accept(serializer).value());

    tinyxml2::XMLPrinter printer;
    doc.Print(&printer);

    std::string mathml(printer.CStr());

    std::string expected = R"(<mrow>
    <mo>[</mo>
    <mtable>
        <mtr>
            <mtd>
                <mn>1</mn>
            </mtd>
            <mtd>
                <mn>2</mn>
            </mtd>
        </mtr>
        <mtr>
            <mtd>
                <mn>3</mn>
            </mtd>
            <mtd>
                <mn>4</mn>
            </mtd>
        </mtr>
    </mtable>
    <mo>]</mo>
</mrow>
)";

    // std::cout<<mathml<<std::endl;
    REQUIRE(expected == mathml);
}

TEST_CASE("Matrix to MathML 2x3", "[Matrix][MathML]")
{
    Oasis::Matrix mat{Oasis::MatrixXXD{{1,2,3},{4,5,6}}};

    tinyxml2::XMLDocument doc;
    Oasis::MathMLSerializer serializer(doc);
    doc.InsertEndChild(mat.Accept(serializer).value());

    tinyxml2::XMLPrinter printer;
    doc.Print(&printer);

    std::string mathml(printer.CStr());

    std::string expected = R"(<mrow>
    <mo>[</mo>
    <mtable>
        <mtr>
            <mtd>
                <mn>1</mn>
            </mtd>
            <mtd>
                <mn>2</mn>
            </mtd>
            <mtd>
                <mn>3</mn>
            </mtd>
        </mtr>
        <mtr>
            <mtd>
                <mn>4</mn>
            </mtd>
            <mtd>
                <mn>5</mn>
            </mtd>
            <mtd>
                <mn>6</mn>
            </mtd>
        </mtr>
    </mtable>
    <mo>]</mo>
</mrow>
)";

    // std::cout<<mathml<<std::endl;
    REQUIRE(expected == mathml);
}

TEST_CASE("Matrix to MathML 3x2", "[Matrix][MathML]")
{
    Oasis::Matrix mat{Oasis::MatrixXXD{{1,2},{4,5},{7,8}}};

    tinyxml2::XMLDocument doc;
    Oasis::MathMLSerializer serializer(doc);
    doc.InsertEndChild(mat.Accept(serializer).value());

    tinyxml2::XMLPrinter printer;
    doc.Print(&printer);

    std::string mathml(printer.CStr());

    std::string expected = R"(<mrow>
    <mo>[</mo>
    <mtable>
        <mtr>
            <mtd>
                <mn>1</mn>
            </mtd>
            <mtd>
                <mn>2</mn>
            </mtd>
        </mtr>
        <mtr>
            <mtd>
                <mn>4</mn>
            </mtd>
            <mtd>
                <mn>5</mn>
            </mtd>
        </mtr>
        <mtr>
            <mtd>
                <mn>7</mn>
            </mtd>
            <mtd>
                <mn>8</mn>
            </mtd>
        </mtr>
    </mtable>
    <mo>]</mo>
</mrow>
)";

    // std::cout<<mathml<<std::endl;
    REQUIRE(expected == mathml);
}

TEST_CASE("Integral works", "[Integral][MathML]")
{
    Oasis::Integral in {
        Oasis::Real { 1 },
        Oasis::Variable { "x" }
    };


    tinyxml2::XMLDocument doc;
    Oasis::MathMLSerializer serializer(doc);
    doc.InsertEndChild(in.Accept(serializer).value());

    tinyxml2::XMLPrinter printer;
    doc.Print(&printer);

    std::string mathml { printer.CStr() };

    std::string expected = reinterpret_cast<const char*>(u8R"(<mrow>
    <mo>∫</mo>
    <mn>1</mn>
    <mrow>
        <mo>d</mo>
        <mi>x</mi>
    </mrow>
</mrow>
)");

    REQUIRE(expected == mathml);
}
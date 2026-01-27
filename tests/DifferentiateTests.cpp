//
// Created by bachia on 4/5/2024.
//
#include "catch2/catch_test_macros.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"
#include "Oasis/Derivative.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/EulerNumber.hpp"
#include "Oasis/SimplifyVisitor.hpp"

inline Oasis::SimplifyVisitor simplifyVisitor{};

TEST_CASE("Differentiate Nonzero number", "[Differentiate][Real][Nonzero]")
{
    Oasis::Real zero { 0.0f };

    Oasis::Real base { 2.0f };
    Oasis::Variable var { "x" };

    auto diff2 = base.Differentiate(var);
    REQUIRE(zero.Equals(*diff2));
}

TEST_CASE("Differentiate Zero", "[Differentiate][Real][Zero]")
{
    Oasis::Variable constant { "C" };
    Oasis::Real zero { 0.0f };
    Oasis::Variable var { "x" };

    auto diff = zero.Differentiate(var);
    REQUIRE(zero.Equals(*diff));
}

TEST_CASE("Differentiate Same Variable", "[Differentiate][Variable][Same]")
{
    Oasis::Variable var { "x" };
    Oasis::Real result {1.0};

    auto integrated = var.Differentiate(var);
    REQUIRE(result.Equals(*integrated));
}

TEST_CASE("Differentiate Different Variable", "[Differentiate][Variable][Different]")
{
    Oasis::Variable var { "x" };
    Oasis::Variable var2 { "y" };
    Oasis::Real zero { 0.0f };

    auto diff = var2.Differentiate(var);
    REQUIRE(zero.Equals(*diff));
}

TEST_CASE("Differentiate Power Rule", "[Differentiate][Exponent][Power]")
{
    Oasis::Variable var { "x" };
    Oasis::Exponent<Oasis::Variable, Oasis::Real> differentiate { Oasis::Variable { var.GetName() }, Oasis::Real { 3.0f } };
    Oasis::Multiply<Oasis::Exponent<Oasis::Variable, Oasis::Real>, Oasis::Real> diff1 {
        Oasis::Multiply {
            Oasis::Exponent { Oasis::Variable { var.GetName() }, Oasis::Real { 2.0f } },
            Oasis::Real { 3.0f } }};
    auto ptr = diff1.Accept(simplifyVisitor).value();

    auto diffed = differentiate.Differentiate(var);
    REQUIRE((*diffed).Equals(*ptr));
}

TEST_CASE("Differentiate Constant Rule Multiply", "[Differentiate][Multiply][Constant]")
{
    Oasis::Variable var { "x" };
    Oasis::Multiply<Oasis::Real, Oasis::Variable> diff1 { Oasis::Real { 3.0f }, Oasis::Variable { var.GetName() } };
    Oasis::Real three {3.0};

    auto ptr = three.Accept(simplifyVisitor).value();
    auto diffed = diff1.Differentiate(var);
    REQUIRE((diffed->Equals(*ptr)));
}

TEST_CASE("Differentiate Constant Rule Divide", "[Differentiate][Divide][Constant]")
{
    Oasis::Variable var { "x" };
    Oasis::Divide<Oasis::Variable, Oasis::Real> diff1 { Oasis::Variable { var.GetName() }, Oasis::Real { 2.0f } };
    Oasis::Real half {0.5f};
    auto diffed = diff1.Differentiate(var);
    auto simplified = diffed->Accept(simplifyVisitor).value();
    REQUIRE((simplified->Equals(*(half.Accept(simplifyVisitor).value()))));
}

TEST_CASE("Differentiate Add Rule Different Terms", "[Differentiate][Add][Different]")
{
    Oasis::Variable var { "x" };
    Oasis::Add<Oasis::Variable, Oasis::Real>diff1 { Oasis::Variable { var.GetName() }, Oasis::Real { 2.0f } };

    Oasis::Real one {1};
    auto diffed = diff1.Differentiate(var);
    auto simplified = diffed->Accept(simplifyVisitor).value();

    REQUIRE(simplified->Equals(*(one.Accept(simplifyVisitor).value())));
}

TEST_CASE("Differentiate Subtract Rule Different Terms", "[Differentiate][Subtract][Different]")
{
    Oasis::Variable var { "x" };
    Oasis::Subtract<Oasis::Variable, Oasis::Real> diff1 { Oasis::Variable { var.GetName() }, Oasis::Real { 2.0f } };
    Oasis::Real one {1};
    auto diffed = diff1.Differentiate(var);
    auto simplified = diffed->Accept(simplifyVisitor).value();

    REQUIRE(simplified->Equals(*(one.Accept(simplifyVisitor).value())));
}

TEST_CASE("Differentiate Add Rule Like Terms", "[Differentiate][Add][Like]")
{
    Oasis::Variable var { "x" };
    Oasis::Add<Oasis::Variable, Oasis::Variable> diff1 { Oasis::Variable { var.GetName() }, Oasis::Variable { var.GetName() } };

    Oasis::Real two {2};

    auto diffed = diff1.Differentiate(var);
    auto simplified = diffed->Accept(simplifyVisitor).value();

    REQUIRE(simplified->Equals(*(two.Accept(simplifyVisitor).value())));
}

TEST_CASE("Differentiate Quotient Rule Like Terms", "[Differentiate][Divide][Like]")
{
    Oasis::Variable var{"x"}; // dx
    // derivative of [(x + 2)/(x + 3)]
    Oasis::Divide<Oasis::Add<Oasis::Variable, Oasis::Real>, Oasis::Add<Oasis::Variable, Oasis::Real>> diff1
    {Oasis::Add<Oasis::Variable, Oasis::Real> {Oasis::Variable {var.GetName() },
                                               Oasis::Real {2}},Oasis::Add<Oasis::Variable, Oasis::Real>
                                                       {Oasis::Variable {var.GetName() }, Oasis::Real
                                                       {3}}};
    Oasis::Divide<Oasis::Expression, Oasis::Multiply<Oasis::Expression, Oasis::Expression>> answer
            {Oasis::Subtract<Oasis::Multiply<Oasis::Add<Oasis::Variable, Oasis::Real>, Oasis::Real>>
            {Oasis::Multiply<Oasis::Add<Oasis::Variable, Oasis::Real>, Oasis::Real>
                    {Oasis::Add<Oasis::Variable, Oasis::Real> {Oasis::Variable {var.GetName()}, Oasis::Real {3}},
                     Oasis::Real {1}},
             Oasis::Multiply<Oasis::Add<Oasis::Variable, Oasis::Real>, Oasis::Real>{
                Oasis::Add<Oasis::Variable, Oasis::Real> {Oasis::Variable {var.GetName()}, Oasis::Real {2}},
                Oasis::Real {1}}},
             Oasis::Multiply<Oasis::Expression, Oasis::Expression>{Oasis::Add<Oasis::Variable, Oasis::Real>
            {Oasis::Variable {var.GetName() }, Oasis::Real
            {3}}, Oasis::Add<Oasis::Variable, Oasis::Real>
            {Oasis::Variable {var.GetName() }, Oasis::Real
            {3}}}};
    auto diffed = diff1.Differentiate(var);
    auto simplified = diffed->Accept(simplifyVisitor).value();

    auto simplifiedAns = answer.Accept(simplifyVisitor).value();

    REQUIRE(simplified->Equals(*(simplifiedAns)));
}

TEST_CASE("Differentiate Multiple Variables Differentiate", "[Differentiate][Multiply][Different]")
{
    Oasis::Variable x {"x"};
    Oasis::Variable y {"y"};
    Oasis::Multiply<Oasis::Variable, Oasis::Variable> xy
    {Oasis::Variable {x.GetName()}, Oasis::Variable {y.GetName()}};
    auto diffed = xy.Differentiate(x);
    auto simplified = diffed->Accept(simplifyVisitor).value();
    REQUIRE(simplified->Equals(*(y.Accept(simplifyVisitor).value())));
}

TEST_CASE("Differentiate Multiple Variables + Real Differentiate", "[Differentiate][Multiply][Different]")
{
    Oasis::Variable x {"x"};
    Oasis::Variable y {"y"};
    Oasis::Multiply<Oasis::Variable, Oasis::Variable> xy
            {Oasis::Variable {x.GetName()}, Oasis::Variable {y.GetName()}};

    Oasis::Multiply<Oasis::Real, Oasis::Expression> threexy
            {Oasis::Real {3}, *(xy.Copy())};
    Oasis::Multiply<Oasis::Real, Oasis::Variable> threex
            {Oasis::Real{3}, Oasis::Variable{x.GetName()}};
    auto diffed = threexy.Differentiate(y);
    auto simplified = diffed->Accept(simplifyVisitor).value();
    REQUIRE(simplified->Equals(*(threex.Accept(simplifyVisitor).value())));
}

TEST_CASE("Derivative Wrapper Class Add", "[Differentiate][Add][Like]")
{
    Oasis::Variable x {"x"};
    Oasis::Derivative<Oasis::Add<Oasis::Variable, Oasis::Real>, Oasis::Variable> diff1
    {Oasis::Add<Oasis::Variable, Oasis::Real> {Oasis::Variable {x.GetName()},
                                               Oasis::Real{2}}, Oasis::Variable {x.GetName()}};
    Oasis::Real one {1};
    auto diffed = diff1.Accept(simplifyVisitor).value();
    REQUIRE(diffed->Equals(*(one.Accept(simplifyVisitor).value())));
}

TEST_CASE("Derivative Wrapper Class Subtract", "[Differentiate][Subtract][Like]")
{
    Oasis::Variable x {"x"};
    Oasis::Derivative<Oasis::Subtract<Oasis::Variable, Oasis::Real>, Oasis::Variable> diff1
            {Oasis::Subtract<Oasis::Variable, Oasis::Real> {Oasis::Variable {x.GetName()},
                                                       Oasis::Real{2}}, Oasis::Variable {x.GetName()}};
    Oasis::Real one {1};
    auto diffed = diff1.Accept(simplifyVisitor).value();
    REQUIRE(diffed->Equals(*(one.Accept(simplifyVisitor).value())));
}

TEST_CASE("Derivative Wrapper Class Multiply", "[Differentiate][Multiply][Like]")
{
    Oasis::Variable x {"x"};
    Oasis::Derivative<Oasis::Multiply<Oasis::Variable, Oasis::Real>, Oasis::Variable> diff1
            {Oasis::Multiply<Oasis::Variable, Oasis::Real> {Oasis::Variable {x.GetName()},
                                                       Oasis::Real{2}}, Oasis::Variable {x.GetName()}};
    Oasis::Real two {2};
    auto diffed = diff1.Accept(simplifyVisitor).value();
    REQUIRE(diffed->Equals(*(two.Accept(simplifyVisitor).value())));
}

TEST_CASE("Derivative Wrapper Class Divide", "[Differentiate][Divide][Like]")
{
    Oasis::Variable x {"x"};
    Oasis::Derivative<Oasis::Divide<Oasis::Variable, Oasis::Real>, Oasis::Variable> diff1
            {Oasis::Divide<Oasis::Variable, Oasis::Real> {Oasis::Variable {x.GetName()},
                                                       Oasis::Real{2}}, Oasis::Variable {x.GetName()}};
    Oasis::Real half {0.5f};
    auto diffed = diff1.Accept(simplifyVisitor).value();
    REQUIRE(diffed->Equals(*(half.Accept(simplifyVisitor).value())));
}

TEST_CASE("Derivative Wrapper Class Real", "[Differentiate][Real][Like]")
{
    Oasis::Variable x {"x"};
    Oasis::Derivative<Oasis::Real, Oasis::Variable> diff1
            {Oasis::Real {2}, Oasis::Variable {x.GetName()}};
    Oasis::Real zero {0};
    auto diffed = diff1.Accept(simplifyVisitor).value();
    REQUIRE(diffed->Equals(*(zero.Accept(simplifyVisitor).value())));
}

TEST_CASE("Derivative Wrapper Class Variable", "[Differentiate][Variable][Like]")
{
    Oasis::Variable x {"x"};
    Oasis::Derivative<Oasis::Variable, Oasis::Variable> diff1
            {Oasis::Variable {x.GetName()}, Oasis::Variable {x.GetName()}};
    Oasis::Real one {1};
    auto diffed = diff1.Accept(simplifyVisitor).value();
    REQUIRE(diffed->Equals(*(one.Accept(simplifyVisitor).value())));
}

TEST_CASE("Derivative Wrapper Class Exponent", "[Differentiate][Exponent][Like]")
{
    Oasis::Variable x {"x"};
    Oasis::Derivative<Oasis::Exponent<Oasis::Variable, Oasis::Real>, Oasis::Variable> diff1
            {Oasis::Exponent<Oasis::Variable, Oasis::Real> {Oasis::Variable {x.GetName()},
                                                       Oasis::Real{2}}, Oasis::Variable {x.GetName()}};
    Oasis::Multiply<Oasis::Variable, Oasis::Real> twox {Oasis::Variable {x.GetName()}, Oasis::Real {2}};
    auto diffed = diff1.Accept(simplifyVisitor).value();
    REQUIRE(diffed->Equals(*(twox.Accept(simplifyVisitor).value())));
}

TEST_CASE("Product Rule", "[Differentiate][Product]")
{
    Oasis::Variable f{"x"};
    Oasis::Multiply g{Oasis::Real{6.0}, Oasis::Variable{"x"}};
    Oasis::Derivative diff{Oasis::Multiply{f, g}, Oasis::Variable{"x"}};
    auto simplified = diff.Accept(simplifyVisitor).value();
    Oasis::Multiply expected{Oasis::Real{12.0}, Oasis::Variable{"x"}};

    REQUIRE(simplified->Equals(expected));
}

TEST_CASE("Natural Exponential Derivative", "[Derivative][Exponent][Euler's Number]")
{
    Oasis::Exponent exp{Oasis::EulerNumber{}, Oasis::Variable{"x"}};
    Oasis::Exponent exp2{Oasis::EulerNumber{}, Oasis::Multiply{Oasis::Real{2.0},Oasis::Variable{"x"}}};
    Oasis::Derivative diffExp{exp, Oasis::Variable{"x"}};
    Oasis::Derivative diffExp2{exp2, Oasis::Variable{"x"}};
    Oasis::Multiply expected2{Oasis::Real{2.0}, exp2};
    auto simplified = diffExp.Accept(simplifyVisitor).value();
    auto simplified2 = diffExp2.Accept(simplifyVisitor).value();

    REQUIRE(simplified->Equals(exp));
    REQUIRE(simplified2->Equals(expected2));
}

TEST_CASE("Natural Logarithm Derivative", "[Derivative][Logarithm][Euler's Number]")
{
    Oasis::Derivative diffLog{
        Oasis::Log{Oasis::EulerNumber{},Oasis::Multiply{Oasis::Real{6},Oasis::Variable{"x"}}}, Oasis::Variable{"x"}};
    auto raw = diffLog.Accept(simplifyVisitor);
    auto diff = std::move(raw).value();
    Oasis::Divide expected{Oasis::Real{1.0}, Oasis::Variable{"x"}};
    REQUIRE(diff->Equals(expected));
}

TEST_CASE("Real Base Logarithm Derivative", "[Derivative][Logarithm][Euler's Real]")
{
    Oasis::Derivative diffLog{
        Oasis::Log{Oasis::Real{10.0},Oasis::Multiply{Oasis::Real{6},Oasis::Variable{"x"}}}, Oasis::Variable{"x"}};
    auto diff = diffLog.Accept(simplifyVisitor).value();
    Oasis::Divide expected{Oasis::Real{1.0}, Oasis::Multiply{Oasis::Variable{"x"},
                                                      Oasis::Log{Oasis::EulerNumber{}, Oasis::Real{10.0}}}};
    REQUIRE(diff->Equals(*expected.Accept(simplifyVisitor).value()));
}

TEST_CASE("Variable Base Logarithm Derivative", "[Derivative][Logarithm][Variable]")
{
    Oasis::Derivative diffLog{
        Oasis::Log{Oasis::Variable{"y"},Oasis::Multiply{Oasis::Real{6},Oasis::Variable{"x"}}}, Oasis::Variable{"x"}};
    auto diff = diffLog.Accept(simplifyVisitor).value();
    Oasis::Divide expected{Oasis::Real{1.0}, Oasis::Multiply{Oasis::Variable{"x"},
                                                      Oasis::Log{Oasis::EulerNumber{}, Oasis::Variable{"y"}}}};
    REQUIRE(diff->Equals(expected));
}

TEST_CASE("Any Base Exponential Derivative", "[Derivative][Exponent]")
{
    Oasis::Exponent exp{Oasis::Variable{"a"}, Oasis::Multiply{Oasis::Real{2.0},Oasis::Variable{"x"}}};
    Oasis::Derivative diffExp{exp, Oasis::Variable{"x"}};
    Oasis::Multiply expected{Oasis::Multiply{Oasis::Real{2.0}, exp}, Oasis::Log{Oasis::EulerNumber{}, Oasis::Variable{"a"}}};
    auto simplified = diffExp.Accept(simplifyVisitor).value();
    REQUIRE(simplified->Equals(expected));
}

TEST_CASE("Variable Base Exponential Derivative", "[Derivative][Exponent]")
{
    Oasis::Exponent exp{Oasis::Variable{"x"}, Oasis::Multiply{Oasis::Real{2.0},Oasis::Variable{"x"}}};
    Oasis::Derivative diffExp{exp, Oasis::Variable{"x"}};
    Oasis::Multiply expected{exp , Oasis::Add{ Oasis::Multiply {Oasis::Log{Oasis::EulerNumber{}, Oasis::Variable{"x"}},
                                    Oasis::Real{2.0}},Oasis::Real{2.0}},};
    auto simplified = diffExp.Accept(simplifyVisitor).value();
    REQUIRE(simplified->Equals(expected));
}

TEST_CASE("General Exponential Derivative", "[Derivative][Exponent]")
{
    Oasis::Exponent exp{ Oasis::Add { Oasis::Variable{"x"}, Oasis::Real{3.0} },
                                      Oasis::Multiply{Oasis::Real{4.0},Oasis::Variable{"x"}}};
    Oasis::Derivative diffExp{exp, Oasis::Variable{"x"}};
    Oasis::Multiply expected{ exp ,
                                Oasis::Add{
                                    Oasis::Divide {  Oasis::Multiply {Oasis::Real{4.0},Oasis::Variable{"x"} } ,
                                        Oasis::Add { Oasis::Variable{"x"}, Oasis::Real{3.0} } }  ,
                                    Oasis::Multiply { Oasis::Real{4.0} ,
                                        Oasis::Log{Oasis::EulerNumber{},
                                            Oasis::Add { Oasis::Variable{"x"}, Oasis::Real{3.0} }}} } };
    auto simplified = diffExp.Accept(simplifyVisitor).value();
    REQUIRE(simplified->Equals(expected));
}

TEST_CASE("Expression Base Exponential Derivative", "[Derivative][Exponent]")
{
    Oasis::Exponent exp{ Oasis::Add { Oasis::Variable{"x"}, Oasis::Real{3.0} } , Oasis::Real{2.0}};
    Oasis::Derivative diffExp{exp, Oasis::Variable{"x"}};

    Oasis::Multiply expected{ Oasis::Add {Oasis::Variable{"x"} , Oasis::Real {3.0} }, Oasis::Real{ 2.0}    };

    auto simplified = diffExp.Accept(simplifyVisitor).value();
    REQUIRE(simplified->Equals(expected));
}
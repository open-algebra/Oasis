#include "catch2/catch_test_macros.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/EulerNumber.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/SimplifyVisitor.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"
#include "Oasis/Log.hpp"

inline Oasis::SimplifyVisitor simplifyVisitor{};

TEST_CASE("Integrate Nonzero number", "[Integrate][Real][Nonzero]")
{
    Oasis::Add<Oasis::Multiply<Oasis::Real, Oasis::Variable>, Oasis::Variable> integral {
        Oasis::Multiply {
            Oasis::Real { 2.0f },
            Oasis::Variable { "x" } },
        Oasis::Variable { "C" }
    };

    Oasis::Real base { 2.0f };
    Oasis::Variable var { "x" };

    auto integrated = base.Integrate(var);
    REQUIRE(integral.Equals(*integrated));
}

TEST_CASE("Integrate Zero", "[Integrate][Real][Zero]")
{
    Oasis::Variable constant { "C" };
    Oasis::Real zero { 0.0f };
    Oasis::Variable var { "x" };

    auto integrated = zero.Integrate(var);
    REQUIRE(constant.Equals(*integrated));
}

TEST_CASE("Integrate Same Variable", "[Integrate][Variable][Same]")
{
    Oasis::Variable var { "x" };
    Oasis::Add<Oasis::Divide<Oasis::Exponent<Oasis::Variable, Oasis::Real>, Oasis::Real>, Oasis::Variable> integral {
        Oasis::Divide {
            Oasis::Exponent { Oasis::Variable { var.GetName() }, Oasis::Real { 2.0f } },
            Oasis::Real { 2.0f } },
        Oasis::Variable { "C" }
    };

    auto ptr = integral.Accept(simplifyVisitor).value();
    auto integrated = var.Integrate(var);
    REQUIRE(ptr->Equals(*integrated));
}

TEST_CASE("Integrate Different Variable", "[Integrate][Variable][Different]")
{
    Oasis::Variable var { "x" };
    Oasis::Variable var2 { "y" };
    Oasis::Add<Oasis::Multiply<Oasis::Variable, Oasis::Variable>, Oasis::Variable> integral {
        Oasis::Multiply {
            Oasis::Variable { var2.GetName() },
            Oasis::Variable { var.GetName() } },
        Oasis::Variable { "C" }
    };

    auto integrated = var2.Integrate(var);
    REQUIRE(integral.Equals(*integrated));
}

TEST_CASE("Integrate Power Rule", "[Integrate][Exponent][Power]")
{
    Oasis::Variable var { "x" };
    Oasis::Exponent<Oasis::Variable, Oasis::Real> integrand { Oasis::Variable { var.GetName() }, Oasis::Real { 2.0f } };
    Oasis::Add<Oasis::Divide<Oasis::Exponent<Oasis::Variable, Oasis::Real>, Oasis::Real>, Oasis::Variable> integral { Oasis::Add {
        Oasis::Divide {
            Oasis::Exponent { Oasis::Variable { var.GetName() }, Oasis::Real { 3.0f } },
            Oasis::Real { 3.0f } },
        Oasis::Variable { "C" } } };
    auto ptr = integral.Accept(simplifyVisitor).value();

    auto integrated = integrand.Integrate(var);
    REQUIRE((*integrated).Equals(*ptr));
}

TEST_CASE("Integrate Constant Rule Multiply", "[Integrate][Multiply][Constant]")
{
    Oasis::Variable var { "x" };
    Oasis::Multiply<Oasis::Real, Oasis::Variable> integrand { Oasis::Real { 3.0f }, Oasis::Variable { var.GetName() } };
    Oasis::Add<Oasis::Multiply<Oasis::Real, Oasis::Divide<Oasis::Exponent<Oasis::Variable, Oasis::Real>, Oasis::Real>>, Oasis::Variable> integral {
        Oasis::Add {
            Oasis::Multiply {
                Oasis::Real { 3.0f },
                Oasis::Divide {
                    Oasis::Exponent { Oasis::Variable { var.GetName() }, Oasis::Real { 2.0f } },
                    Oasis::Real { 2.0f } },
            },
            Oasis::Variable {
                "C" } }
    };

    auto ptr = integral.Accept(simplifyVisitor).value();
    auto integrated = integrand.Integrate(var);
    REQUIRE((integrated->Equals(*ptr)));
}

TEST_CASE("Integrate Constant Rule Divide", "[Integrate][Divide][Constant]")
{
    Oasis::Variable var { "x" };
    Oasis::Divide<Oasis::Variable, Oasis::Real> integrand { Oasis::Variable { var.GetName() }, Oasis::Real { 3.0f } };
    Oasis::Add<Oasis::Multiply<Oasis::Divide<Oasis::Real>, Oasis::Exponent<Oasis::Variable, Oasis::Real>>, Oasis::Variable> integral {
        Oasis::Add {
            Oasis::Multiply {
                Oasis::Divide {
                    Oasis::Real { 1.0f }, Oasis::Real { 6.0f } },
                Oasis::Exponent {
                    Oasis::Variable { var.GetName() },
                    Oasis::Real { 2.0f } } },
            Oasis::Variable { "C" } }
    };
    auto integrated = integrand.Integrate(var);
    auto simplified = integrated->Accept(simplifyVisitor).value();

    REQUIRE((simplified->Equals(*(integral.Accept(simplifyVisitor).value()))));
}

TEST_CASE("Integrate Add Rule Different Terms", "[Integrate][Add][Different]")
{
    Oasis::Variable var { "x" };
    Oasis::Add<Oasis::Variable, Oasis::Real> integrand { Oasis::Variable { var.GetName() }, Oasis::Real { 2.0f } };

    Oasis::Add<Oasis::Add<Oasis::Divide<Oasis::Exponent<Oasis::Variable, Oasis::Real>, Oasis::Real>, Oasis::Multiply<Oasis::Real, Oasis::Variable>>, Oasis::Variable> integral {
        Oasis::Add {
            Oasis::Add {
                Oasis::Divide {
                    Oasis::Exponent { Oasis::Variable { var.GetName() }, Oasis::Real { 2.0f } },
                    Oasis::Real { 2.0f } },
                Oasis::Multiply {
                    Oasis::Real { 2.0f },
                    Oasis::Variable { var.GetName() } } },
            Oasis::Variable { "C" } }
    };
    auto integrated = integrand.Integrate(var);
    auto simplified = integrated->Accept(simplifyVisitor).value();

    REQUIRE(simplified->Equals(*(integral.Accept(simplifyVisitor).value())));
}

TEST_CASE("Integrate Subtract Rule Different Terms", "[Integrate][Subtract][Different]")
{
    Oasis::Variable var { "x" };
    Oasis::Subtract<Oasis::Variable, Oasis::Real> integrand { Oasis::Variable { var.GetName() }, Oasis::Real { 2.0f } };

    Oasis::Add<Oasis::Subtract<Oasis::Divide<Oasis::Exponent<Oasis::Variable, Oasis::Real>, Oasis::Real>, Oasis::Multiply<Oasis::Real, Oasis::Variable>>, Oasis::Variable> integral {
        Oasis::Add {
            Oasis::Subtract {
                Oasis::Divide {
                    Oasis::Exponent { Oasis::Variable { var.GetName() }, Oasis::Real { 2.0f } },
                    Oasis::Real { 2.0f } },
                Oasis::Multiply {
                    Oasis::Real { 2.0f },
                    Oasis::Variable { var.GetName() } } },
            Oasis::Variable { "C" } }
    };
    auto integrated = integrand.Integrate(var);

    REQUIRE(integrated->Equals(*(integral.Accept(simplifyVisitor).value())));
}

TEST_CASE("Integrate Add Rule Like Terms", "[Integrate][Add][Like]")
{
    Oasis::Variable var { "x" };
    Oasis::Add<Oasis::Variable, Oasis::Variable> integrand { Oasis::Variable { var.GetName() }, Oasis::Variable { var.GetName() } };

    Oasis::Add<Oasis::Exponent<Oasis::Variable, Oasis::Real>, Oasis::Variable> integral {
        Oasis::Add {
            Oasis::Exponent { Oasis::Variable { var.GetName() }, Oasis::Real { 2.0f } },
            Oasis::Variable { "C" } }
    };

    auto integrated = integrand.Integrate(var);
    auto simplified = integrated->Accept(simplifyVisitor).value();

    REQUIRE(simplified->Equals(*(integral.Accept(simplifyVisitor).value())));
}

TEST_CASE("Integration By Parts: Variable and Euler's Number", "[Integrate][Variable][Euler]")
{
    Oasis::Variable var { "x" };

    Oasis::Multiply<Oasis::Variable, Oasis::Exponent<Oasis::EulerNumber, Oasis::Variable>> integrand {
        Oasis::Variable { var.GetName() },
        Oasis::Exponent { Oasis::EulerNumber{}, Oasis::Variable { var.GetName() } }
    };

    Oasis::Add<Oasis::Subtract<Oasis::Multiply<Oasis::Variable, Oasis::Exponent<Oasis::EulerNumber, Oasis::Variable>>, Oasis::Exponent<Oasis::EulerNumber, Oasis::Variable>>, Oasis::Variable> integral {
        Oasis::Add {
            Oasis::Subtract {
                Oasis::Multiply {
                    Oasis::Variable { var.GetName() },
                    Oasis::Exponent { Oasis::EulerNumber{}, Oasis::Variable { var.GetName() } } },
                Oasis::Exponent { Oasis::EulerNumber{}, Oasis::Variable { var.GetName() } } },
            Oasis::Variable { "C" }
        }
    };

    auto ptr = integral.Accept(simplifyVisitor).value();
    auto integrated = integrand.Integrate(var);
    REQUIRE((integrated->Equals(*ptr)));

    integrated = integrand.SwapOperands().Integrate(var);
    REQUIRE((integrated->Equals(*ptr)));
}

TEST_CASE("Integration By Parts: Exponent and Euler's Number", "[Integrate][Exponent][Euler]")
{
    Oasis::Variable var { "x" };

    Oasis::Multiply<Oasis::Exponent<Oasis::Variable, Oasis::Real>, Oasis::Exponent<Oasis::EulerNumber, Oasis::Variable>> integrand {
        Oasis::Exponent { Oasis::Variable { var.GetName() }, Oasis::Real { 2 } },
        Oasis::Exponent { Oasis::EulerNumber{}, Oasis::Variable { var.GetName() } }
    };

    Oasis::Add<Oasis::Multiply<Oasis::Exponent<Oasis::EulerNumber, Oasis::Variable>,
    Oasis::Add<Oasis::Exponent<Oasis::Variable, Oasis::Real>, Oasis::Multiply<Oasis::Real,
    Oasis::Add<Oasis::Variable, Oasis::Real>>>>, Oasis::Variable> integral {
        Oasis::Add {
            Oasis::Multiply {
                Oasis::Exponent { Oasis::EulerNumber{}, Oasis::Variable { var.GetName() } },
                Oasis::Add {
                    Oasis::Exponent { Oasis::Variable { var.GetName() }, Oasis::Real { 2 } },
                    Oasis::Multiply {
                        Oasis::Real {-2},
                        Oasis::Add {
                            Oasis::Variable { var.GetName() },
                            Oasis::Real { -1 }
                        }
                    },
                }
            },
            Oasis::Variable { "C" }
        }
    };


    auto ptr = integral.Accept(simplifyVisitor).value();
    auto integrated = integrand.Integrate(var);
    REQUIRE((integrated->Equals(*ptr)));

    integrated = integrand.SwapOperands().Integrate(var);
    REQUIRE((integrated->Equals(*ptr)));

}

TEST_CASE("Integration By Parts: Variable and Logarithm", "[Integrate][Variable][Logarithm]")
{
    Oasis::Variable var { "x" };

    Oasis::Multiply<Oasis::Variable, Oasis::Log<Oasis::EulerNumber, Oasis::Variable>> integrand {
        Oasis::Variable { var.GetName() },
        Oasis::Log { Oasis::EulerNumber{}, Oasis::Variable { var.GetName() } }
    };

    Oasis::Add<Oasis::Multiply<Oasis::Exponent<Oasis::Variable, Oasis::Real>,
    Oasis::Add<Oasis::Multiply< Oasis::Divide<Oasis::Real, Oasis::Real>,
    Oasis::Log<Oasis::EulerNumber, Oasis::Variable>>, Oasis::Divide<Oasis::Real, Oasis::Real>>>, Oasis::Variable> integral {
        Oasis::Add {
            Oasis::Multiply {
                Oasis::Exponent { Oasis::Variable { var.GetName() }, Oasis::Real { 2 }  },
                Oasis::Add {
                    Oasis::Multiply {
                        Oasis::Divide { Oasis::Real { 1 }, Oasis::Real { 2 } },
                        Oasis::Log { Oasis::EulerNumber{}, Oasis::Variable { var.GetName() } }
                    },
                    Oasis::Divide { Oasis::Real { -1 }, Oasis::Real { 4 } },
                 }
            },
            Oasis::Variable { "C" }
        }
    };

    auto ptr = integral.Accept(simplifyVisitor).value();
    auto integrated = integrand.Integrate(var);
    REQUIRE((integrated->Equals(*ptr)));

    integrated = integrand.SwapOperands().Integrate(var);
    REQUIRE((integrated->Equals(*ptr)));
}

TEST_CASE("Integration By Parts: Exponent and Logarithm", "[Integrate][Exponent][Logarithm]")
{
    Oasis::Variable var { "x" };

    Oasis::Multiply<Oasis::Exponent<Oasis::Variable, Oasis::Real>, Oasis::Log<Oasis::EulerNumber, Oasis::Variable>> integrand {
        Oasis::Exponent { Oasis::Variable { var.GetName() }, Oasis::Real { 2 } },
        Oasis::Log { Oasis::EulerNumber{}, Oasis::Variable { var.GetName() } }
    };

    Oasis::Add<Oasis::Multiply<Oasis::Exponent<Oasis::Variable, Oasis::Real>,
    Oasis::Add<Oasis::Multiply< Oasis::Divide<Oasis::Real, Oasis::Real>,
    Oasis::Log<Oasis::EulerNumber, Oasis::Variable>>, Oasis::Divide<Oasis::Real, Oasis::Real>>>, Oasis::Variable> integral {
        Oasis::Add {
            Oasis::Multiply {
                Oasis::Exponent { Oasis::Variable { var.GetName() }, Oasis::Real { 3 }  },
                Oasis::Add {
                   Oasis::Multiply {
                       Oasis::Divide { Oasis::Real { 1 }, Oasis::Real { 3 } },
                       Oasis::Log { Oasis::EulerNumber{}, Oasis::Variable { var.GetName() } }
                   },
                   Oasis::Divide { Oasis::Real { -1 }, Oasis::Real { 9 } },
                }
            },
            Oasis::Variable { "C" }
        }
    };

     auto ptr = integral.Accept(simplifyVisitor).value();
     auto integrated = integrand.Integrate(var);
     REQUIRE((integrated->Equals(*ptr)));

     integrated = integrand.SwapOperands().Integrate(var);
     REQUIRE((integrated->Equals(*ptr)));
}
//
// Created by Blake Kessler on 10/31/23
//

#include "catch2/catch_test_macros.hpp"
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "Oasis/Add.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/EulerNumber.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/RecursiveCast.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Undefined.hpp"
#include "Oasis/Variable.hpp"

#define EPSILON 1E-6

TEST_CASE("Log with invalid base", "[Log][Invalid Base]")
{
    Oasis::Log negativeBase = Oasis::Log {
        Oasis::Real { -5.0 },
        Oasis::Real { 10.0 }
    };
    Oasis::Log baseOne = Oasis::Log {
        Oasis::Real { 1.0 },
        Oasis::Real { 10.0 }
    };

    auto negativeBaseSimplified = negativeBase.Simplify();
    REQUIRE(negativeBaseSimplified->Is<Oasis::Undefined>());

    auto baseOneSimplified = baseOne.Simplify();
    REQUIRE(baseOneSimplified->Is<Oasis::Undefined>());
}

TEST_CASE("Log with invalid argument", "[Log][Invalid Argument]")
{
    Oasis::Log negativeArgument {
        Oasis::Real { 15.0 },
        Oasis::Real { -5.0 }
    };

    auto negativeArgumentSimplified = negativeArgument.Simplify();
    REQUIRE(negativeArgumentSimplified->Is<Oasis::Undefined>());
}

TEST_CASE("Log of Real Numbers", "[Log][Real]")
{
    //log[5](1) = 0
    Oasis::Log base5of1 {
        Oasis::Real { 5.0 },
        Oasis::Real { 1.0 }
    };

    auto base5of1_Simplified = base5of1.Simplify();
    REQUIRE(base5of1_Simplified->Is<Oasis::Real>());

    auto simplifiedReal5_1 = dynamic_cast<Oasis::Real&>(*base5of1_Simplified);
    REQUIRE(simplifiedReal5_1.GetValue() == 0.0);
    
    //log[5](5) = 1
    Oasis::Log base5of5 {
        Oasis::Real { 5.0 },
        Oasis::Real { 5.0 }
    };

    auto base5of5_Simplified = base5of5.Simplify();
    REQUIRE(base5of5_Simplified->Is<Oasis::Real>());

    auto simplifiedReal5_5 = dynamic_cast<Oasis::Real&>(*base5of5_Simplified);
    REQUIRE_THAT(simplifiedReal5_5.GetValue(), Catch::Matchers::WithinAbs(1.0, EPSILON));

    //log[5](25) = 2
    Oasis::Log base5of25 {
        Oasis::Real { 5.0 },
        Oasis::Real { 25.0 }
    };

    auto base5of25_Simplified = base5of25.Simplify();
    REQUIRE(base5of25_Simplified->Is<Oasis::Real>());

    auto simplifiedReal5_25 = dynamic_cast<Oasis::Real&>(*base5of25_Simplified);
    REQUIRE_THAT(simplifiedReal5_25.GetValue(), Catch::Matchers::WithinAbs(2.0, EPSILON));
}

TEST_CASE("Log of Exponentiation", "[Log][Exponent]")
{
    //log[5](5^x) = x
    Oasis::Log base5of5toX {
        Oasis::Real { 5.0 },
        Oasis::Exponent {
             Oasis::Real { 5.0 },
             Oasis::Variable { "x" }
        }
    };

    auto base5of5toX_Simplified = base5of5toX.Simplify();
    auto simplifiedSpecialized = RecursiveCast<Oasis::Multiply<Oasis::Real, Oasis::Variable>>(*base5of5toX_Simplified);

    REQUIRE(simplifiedSpecialized != nullptr);
    REQUIRE_THAT(simplifiedSpecialized->GetMostSigOp().GetValue(), Catch::Matchers::WithinAbs(1.0, EPSILON));
    REQUIRE(simplifiedSpecialized->GetLeastSigOp().Equals(Oasis::Variable { "x" }));

    //log[5](x^x) = xlog[5](x)
    Oasis::Log logOfExp {
        Oasis::Real { 5.0 },
        Oasis::Exponent {
             Oasis::Variable { "x" },
             Oasis::Variable { "x" }
        }
    };

    Oasis::Multiply logOfExpResult {
        Oasis::Variable { "x" },
        Oasis::Log {
            Oasis::Real { 5.0 },
            Oasis::Variable { "x" }
        }
    };

    auto logOfExp_Simplified = logOfExp.Simplify();
    REQUIRE(logOfExpResult.Equals(*logOfExp_Simplified));
}

TEST_CASE("Sum of logs", "[Log][Add]")
{
    Oasis::Log logOf2 {
        Oasis::Real { 5.0 },
        Oasis::Real { 2.0 }
    };
    Oasis::Log logOf3 {
        Oasis::Real { 5.0 },
        Oasis::Real { 3.0 }
    };
    Oasis::Log logOf4 {
        Oasis::Real { 5.0 },
        Oasis::Real { 4.0 }
    };
    Oasis::Log logOf6 {
        Oasis::Real { 5.0 },
        Oasis::Real { 6.0 }
    };

    auto logOf4Simplified = dynamic_cast<Oasis::Real&>(*Oasis::Add {
        logOf2,
        logOf2
    }.Simplify());
    auto logOf6Simplified = dynamic_cast<Oasis::Real&>(*Oasis::Add {
        logOf2,
        logOf3
    }.Simplify());
    
    REQUIRE_THAT(logOf4Simplified.GetValue(), Catch::Matchers::WithinAbs(dynamic_cast<Oasis::Real&>(*logOf4.Simplify()).GetValue(), EPSILON));
    REQUIRE_THAT(logOf6Simplified.GetValue(), Catch::Matchers::WithinAbs(dynamic_cast<Oasis::Real&>(*logOf6.Simplify()).GetValue(), EPSILON));
}

TEST_CASE("Difference of logs", "[Log][Subtract]")
{
    Oasis::Log logOf2 {
        Oasis::Real { 5.0 },
        Oasis::Real { 2.0 }
    };
    Oasis::Log logOf3 {
        Oasis::Real { 5.0 },
        Oasis::Real { 3.0 }
    };
    Oasis::Log logOf4 {
        Oasis::Real { 5.0 },
        Oasis::Real { 4.0 }
    };
    Oasis::Log logOf6 {
        Oasis::Real { 5.0 },
        Oasis::Real { 6.0 }
    };

    auto logOf2Simplified = Oasis::Subtract {
        logOf4,
        logOf2
    }.Simplify();
    auto logOf3Simplified = dynamic_cast<Oasis::Real&>(*Oasis::Subtract {
        logOf6,
        logOf2
    }.Simplify());

    REQUIRE(logOf2Simplified->Equals(*(logOf2.Simplify())));
    REQUIRE_THAT(logOf3Simplified.GetValue(), Catch::Matchers::WithinAbs(dynamic_cast<Oasis::Real&>(*(logOf3.Simplify())).GetValue(), EPSILON));
}

TEST_CASE("Change of Base", "[Log][Divide]")
{
    Oasis::Log logOf2 {
        Oasis::Real { 5.0 },
        Oasis::Real { 2.0 }
    };
    Oasis::Log logOf3 {
        Oasis::Real { 5.0 },
        Oasis::Real { 3.0 }
    };
    Oasis::Log logOf4 {
        Oasis::Real { 5.0 },
        Oasis::Real { 4.0 }
    };
    Oasis::Log logOf6 {
        Oasis::Real { 5.0 },
        Oasis::Real { 6.0 }
    };
    Oasis::Log logBase3 {
        Oasis::Real { 3.0 },
        Oasis::Real { 6.0 }
    };
    Oasis::Real two { 2.0 };

    auto logBase2Simplified = Oasis::Divide {
        logOf4,
        logOf2
    }.Simplify();
    auto logBase3Simplified = dynamic_cast<Oasis::Real&>(*Oasis::Divide {
        logOf6,
        logOf3
    }.Simplify());

    REQUIRE(logBase2Simplified->Equals(two));
    REQUIRE_THAT(logBase3Simplified.GetValue(), Catch::Matchers::WithinAbs(dynamic_cast<Oasis::Real&>(*(logBase3.Simplify())).GetValue(), EPSILON)); //PROBLEM
}

TEST_CASE("Undefined", "[UNDEFINED][UNDEFINED]") {
    REQUIRE(!Oasis::Undefined().Equals(Oasis::Undefined()));
    REQUIRE(Oasis::Log<Oasis::Real, Oasis::Real>(Oasis::Real(-1.0), Oasis::Real(5.0)).Simplify()->Is<Oasis::Undefined>());
    REQUIRE(Oasis::Log<Oasis::Real, Oasis::Real>(Oasis::Real(5.0), Oasis::Real(-5.0)).Simplify()->Is<Oasis::Undefined>());
    REQUIRE(Oasis::Log<Oasis::Real, Oasis::Real>(Oasis::Real(1.0), Oasis::Real(5.0)).Simplify()->Is<Oasis::Undefined>());
}

TEST_CASE("Integral of Natural Log", "[Integral][Log][Euler]")
{
    Oasis::Integral int1{Oasis::Log{Oasis::EulerNumber{}, Oasis::Variable{"x"}}, Oasis::Variable{"x"}};
    Oasis::Integral int2{Oasis::Log{Oasis::EulerNumber{}, Oasis::Multiply{Oasis::Real{5}, Oasis::Variable{"x"}}}, Oasis::Variable{"x"}};

    auto simp1 = int1.Simplify();
    auto simp2 = int2.Simplify();

    REQUIRE(simp1->Equals(*Oasis::Subtract{Oasis::Multiply{Oasis::Variable{"x"}, Oasis::Log{Oasis::EulerNumber{}, Oasis::Variable{"x"}}}, Oasis::Variable{"x"}}.Simplify()));
    REQUIRE(simp2->Equals(*Oasis::Subtract{Oasis::Multiply{Oasis::Variable{"x"}, Oasis::Log{Oasis::EulerNumber{}, Oasis::Multiply{Oasis::Real{5}, Oasis::Variable{"x"}}}}, Oasis::Variable{"x"}}.Simplify()));
}

TEST_CASE("Integral of Non-Natural variable base Log", "[Integral][Log][Variable]")
{
    Oasis::Integral int1{Oasis::Log{Oasis::Variable{"y"}, Oasis::Variable{"x"}}, Oasis::Variable{"x"}};
    Oasis::Integral int2{Oasis::Log{Oasis::Variable{"y"}, Oasis::Multiply{Oasis::Real{5}, Oasis::Variable{"x"}}}, Oasis::Variable{"x"}};
    Oasis::Integral intEq{Oasis::Log{Oasis::Variable{"x"}, Oasis::Variable{"x"}}, Oasis::Variable{"x"}};

    Oasis::Divide eq1{Oasis::Subtract{Oasis::Multiply{Oasis::Variable{"x"}, Oasis::Log{Oasis::EulerNumber{}, Oasis::Variable{"x"}}}, Oasis::Variable{"x"}},
                        Oasis::Log{Oasis::EulerNumber{}, Oasis::Variable{"y"}}};
    Oasis::Divide eq2{Oasis::Subtract{Oasis::Multiply{Oasis::Variable{"x"}, Oasis::Log{Oasis::EulerNumber{}, Oasis::Multiply{Oasis::Real{5}, Oasis::Variable{"x"}}}}, Oasis::Variable{"x"}},
                        Oasis::Log{Oasis::EulerNumber{}, Oasis::Variable{"y"}}};

    auto simp1 = int1.Simplify();
    auto simp2 = int2.Simplify();
    auto simpEq = intEq.Simplify();

    REQUIRE(simp1->Equals(*eq1.Simplify()));
    REQUIRE(simp2->Equals(*eq2.Simplify()));
    REQUIRE(simpEq->Equals(Oasis::Variable{"x"}));
}

TEST_CASE("Integral of Non-natural real base log", "[Integral][Log][Variable]")
{
    Oasis::Integral int1{Oasis::Log{Oasis::Real{10}, Oasis::Variable{"x"}}, Oasis::Variable{"x"}};
    Oasis::Integral int2{Oasis::Log{Oasis::Real{10}, Oasis::Multiply{Oasis::Real{5}, Oasis::Variable{"x"}}}, Oasis::Variable{"x"}};

    Oasis::Divide eq1{Oasis::Subtract{Oasis::Multiply{Oasis::Variable{"x"}, Oasis::Log{Oasis::EulerNumber{}, Oasis::Variable{"x"}}}, Oasis::Variable{"x"}},
        Oasis::Log{Oasis::EulerNumber{}, Oasis::Real{10}}};
    Oasis::Divide eq2{Oasis::Subtract{Oasis::Multiply{Oasis::Variable{"x"}, Oasis::Log{Oasis::EulerNumber{}, Oasis::Multiply{Oasis::Real{5}, Oasis::Variable{"x"}}}}, Oasis::Variable{"x"}},
        Oasis::Log{Oasis::EulerNumber{}, Oasis::Real{10}}};

    auto simp1 = int1.Simplify();
    auto simp2 = int2.Simplify();

    REQUIRE(simp1->Equals(*eq1.Simplify()));
    REQUIRE(simp2->Equals(*eq2.Simplify()));
}
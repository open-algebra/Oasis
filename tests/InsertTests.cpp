
#include "catch2/catch_test_macros.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Expression.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Variable.hpp"

TEST_CASE("Addition Value", "[Add][Value]")
{
//    Oasis::Add add {
//        Oasis::Add {
//            Oasis::Real { 1.0 },
//            Oasis::Real { 2.0 } },
//        Oasis::Real { 4.0 }
//    };

    auto add = Oasis::Real{1.0};
    auto add1 = add.Insert(Oasis::ExpressionType::Add, Oasis::Real { 2.0 });
    auto add2 = add1->Insert(Oasis::ExpressionType::Add, Oasis::Real { 3.0 });
    auto add3 = add2->Insert(Oasis::ExpressionType::Add, Oasis::Real { 4.0 });

    auto simplified = add3->Simplify();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 10.0);
}

TEST_CASE("Addition Structure", "[Add][Structure]")
{
        Oasis::Add addTraditional {
            Oasis::Add {
                Oasis::Real{1.0},
                Oasis::Real{3.0}
                    },
            Oasis::Add {
                Oasis::Real { 2.0 },
                Oasis::Real { 4.0 }
                    }

        };

    auto add = Oasis::Real{1.0};
    auto add1 = add.Insert(Oasis::ExpressionType::Add, Oasis::Real { 2.0 });
    auto add2 = add1->Insert(Oasis::ExpressionType::Add, Oasis::Real { 3.0 });
    auto add3 = add2->Insert(Oasis::ExpressionType::Add, Oasis::Real { 4.0 });

    REQUIRE(add3->StructurallyEquivalent(addTraditional));
}

TEST_CASE("Multiply Value", "[Multiply][Value]")
{
    //    Oasis::Multiply mul {
    //        Oasis::Multiply {
    //            Oasis::Real { 1.0 },
    //            Oasis::Real { 2.0 } },
    //        Oasis::Real { 4.0 }
    //    };

    auto mul = Oasis::Real{1.0};
    auto mul1 = mul.Insert(Oasis::ExpressionType::Multiply, Oasis::Real { 2.0 });
    auto mul2 = mul1->Insert(Oasis::ExpressionType::Multiply, Oasis::Real { 3.0 });
    auto mul3 = mul2->Insert(Oasis::ExpressionType::Multiply, Oasis::Real { 4.0 });

    auto simplified = mul3->Simplify();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 24.0);
}

TEST_CASE("Multiply Structure", "[Multiply][Structure]")
{
    Oasis::Multiply mulTraditional {
        Oasis::Multiply {
            Oasis::Real{1.0},
            Oasis::Real{3.0}
        },
        Oasis::Multiply {
            Oasis::Real { 2.0 },
            Oasis::Real { 4.0 }
        }

    };

    auto mul = Oasis::Real{1.0};
    auto mul1 = mul.Insert(Oasis::ExpressionType::Multiply, Oasis::Real { 2.0 });
    auto mul2 = mul1->Insert(Oasis::ExpressionType::Multiply, Oasis::Real { 3.0 });
    auto mul3 = mul2->Insert(Oasis::ExpressionType::Multiply, Oasis::Real { 4.0 });

    REQUIRE(mul3->StructurallyEquivalent(mulTraditional));
}

TEST_CASE("Subtract Value", "[Subtract][Value]")
{
    //    Oasis::Subtract sub {
    //        Oasis::Add {
    //            Oasis::Real { 3.0 },
    //            Oasis::Real { 2.0 } },
    //        Oasis::Real { 4.0 }
    //    };

    auto sub = Oasis::Real{3.0};
    auto sub1 = sub.Insert(Oasis::ExpressionType::Add, Oasis::Real { 2.0 });
    auto sub2 = sub1->Insert(Oasis::ExpressionType::Subtract, Oasis::Real { 4.0 });

    auto simplified = sub2->Simplify();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 1.0);
}

TEST_CASE("Subtract Structure", "[Subtract][Structure]")
{
    Oasis::Subtract subTraditional {
        Oasis::Add {
            Oasis::Real{3.0},
            Oasis::Real{2.0}
        },
        Oasis::Real{4.0}
    };

    auto sub = Oasis::Real{3.0};
    auto sub1 = sub.Insert(Oasis::ExpressionType::Add, Oasis::Real { 2.0 });
    auto sub2 = sub1->Insert(Oasis::ExpressionType::Subtract, Oasis::Real { 4.0 });

    REQUIRE(sub2->StructurallyEquivalent(subTraditional));
}
#include "Common.hpp"

#include "catch2/matchers/catch_matchers_floating_point.hpp"
#include "catch2/catch_test_macros.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/RecursiveCast.hpp"
#include "Oasis/Variable.hpp"
#include "Oasis/SimplifyVisitor.hpp"
#include "Oasis/Sine.hpp"
#include "Oasis/Cosine.hpp"
#include "Oasis/Tan.hpp"
#include "Oasis/Arcsine.hpp"
#include "Oasis/Arccosine.hpp"
#include "Oasis/Arctan.hpp"
#include "Oasis/Cotan.hpp"
#include "Oasis/Secant.hpp"
#include "Oasis/Cosecant.hpp"
#include "Oasis/Add.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Pi.hpp"

namespace { Oasis::SimplifyVisitor simplifyVisitor{}; }

#define EPSILON 10E-6

TEST_CASE("Unit Cirlce Sine", "[Sine]")
{

    Oasis::Sine sinepi6 {
        Oasis::Divide {
            Oasis::Pi{},
            Oasis::Real {6.0},
        }
    };

    Oasis::Sine sinepi4 {
        Oasis::Divide {
            Oasis::Pi{},
            Oasis::Real {4.0},
        }
    };

    Oasis::Sine sinepi3 {
        Oasis::Divide {
            Oasis::Pi{},
            Oasis::Real {3.0},
        }
    };

    Oasis::Sine sinepi2 {
        Oasis::Divide {
            Oasis::Pi{},
            Oasis::Real {2.0},
        }
    };

    Oasis::Sine sine2pi3 {
        Oasis::Divide {
            Oasis::Multiply{
                Oasis::Real{2.0},
                Oasis::Pi{},
            },
            Oasis::Real {3.0},
        }
    };

    Oasis::Sine sine3pi4 {
        Oasis::Divide {
            Oasis::Multiply{
                Oasis::Real{3.0},
                Oasis::Pi{},
            },
            Oasis::Real {4.0},
        }
    };

    Oasis::Sine sine5pi6 {
        Oasis::Divide {
            Oasis::Multiply{
                Oasis::Real{5.0},
                Oasis::Pi{},
            },
            Oasis::Real {6.0},
        }
    };

    Oasis::Sine sinepi {
        Oasis::Pi{},
    };

    Oasis::Sine sine7pi6 {
        Oasis::Divide {
            Oasis::Multiply{
                Oasis::Real{7.0},
                Oasis::Pi{},
            },
            Oasis::Real {6.0},
        }
    };

    Oasis::Sine sine5pi4 {
        Oasis::Divide {
            Oasis::Multiply{
                Oasis::Real{5.0},
                Oasis::Pi{},
            },
            Oasis::Real {4.0},
        }
    };

    Oasis::Sine sine4pi3 {
        Oasis::Divide {
            Oasis::Multiply{
                Oasis::Real{4.0},
                Oasis::Pi{},
            },
            Oasis::Real {3.0},
        }
    };

    Oasis::Sine sine3pi2 {
        Oasis::Divide {
            Oasis::Multiply{
                Oasis::Real{3.0},
                Oasis::Pi{},
            },
            Oasis::Real {2.0},
        }
    };
    
    Oasis::Sine sine5pi3 {
        Oasis::Divide {
            Oasis::Multiply{
                Oasis::Real{5.0},
                Oasis::Pi{},
            },
            Oasis::Real {3.0},
        }
    };

    Oasis::Sine sine7pi4 {
        Oasis::Divide {
            Oasis::Multiply{
                Oasis::Real{7.0},
                Oasis::Pi{},
            },
            Oasis::Real {4.0},
        }
    };

    Oasis::Sine sine11pi6 {
        Oasis::Divide {
            Oasis::Multiply{
                Oasis::Real{11.0},
                Oasis::Pi{},
            },
            Oasis::Real {6.0},
        }
    };


    OASIS_CAPTURE_WITH_SERIALIZER(sine11pi6);
    

    auto simplifiedpi6 = sinepi6.Accept(simplifyVisitor).value();
    REQUIRE(simplifiedpi6->Is<Oasis::Real>());

    auto simplifiedRealpi6 = dynamic_cast<Oasis::Real&>(*simplifiedpi6);
    REQUIRE_THAT(simplifiedRealpi6.GetValue(), Catch::Matchers::WithinAbs(0.5, EPSILON));


    auto simplifiedpi4 = sinepi4.Accept(simplifyVisitor).value();
    REQUIRE(simplifiedpi4->Is<Oasis::Real>());

    auto simplifiedRealpi4 = dynamic_cast<Oasis::Real&>(*simplifiedpi4);
    REQUIRE_THAT(simplifiedRealpi4.GetValue(), Catch::Matchers::WithinAbs(std::sqrt(2)/2, EPSILON));

    auto simplifiedpi3 = sinepi3.Accept(simplifyVisitor).value();
    REQUIRE(simplifiedpi3->Is<Oasis::Real>());

    auto simplifiedRealpi3 = dynamic_cast<Oasis::Real&>(*simplifiedpi3);
    REQUIRE_THAT(simplifiedRealpi3.GetValue(), Catch::Matchers::WithinAbs(std::sqrt(3)/2, EPSILON));

    auto simplifiedpi2 = sinepi2.Accept(simplifyVisitor).value();
    REQUIRE(simplifiedpi2->Is<Oasis::Real>());

    auto simplifiedRealpi2 = dynamic_cast<Oasis::Real&>(*simplifiedpi2);
    REQUIRE_THAT(simplifiedRealpi2.GetValue(), Catch::Matchers::WithinAbs(1, EPSILON));

    auto simplified2pi3 = sine2pi3.Accept(simplifyVisitor).value();
    REQUIRE(simplified2pi3->Is<Oasis::Real>());

    auto simplifiedReal2pi3 = dynamic_cast<Oasis::Real&>(*simplified2pi3);
    REQUIRE_THAT(simplifiedReal2pi3.GetValue(), Catch::Matchers::WithinAbs(std::sqrt(3)/2, EPSILON));

    auto simplified3pi4 = sine3pi4.Accept(simplifyVisitor).value();
    REQUIRE(simplified3pi4->Is<Oasis::Real>());

    auto simplifiedReal3pi4 = dynamic_cast<Oasis::Real&>(*simplified3pi4);
    REQUIRE_THAT(simplifiedReal3pi4.GetValue(), Catch::Matchers::WithinAbs(std::sqrt(2)/2, EPSILON));

    auto simplified5pi6 = sine5pi6.Accept(simplifyVisitor).value();
    REQUIRE(simplified5pi6->Is<Oasis::Real>());

    auto simplifiedReal5pi6 = dynamic_cast<Oasis::Real&>(*simplified5pi6);
    REQUIRE_THAT(simplifiedReal5pi6.GetValue(), Catch::Matchers::WithinAbs(0.5, EPSILON));

    auto simplifiedpi = sinepi.Accept(simplifyVisitor).value();
    REQUIRE(simplifiedpi->Is<Oasis::Real>());

    auto simplifiedRealpi = dynamic_cast<Oasis::Real&>(*simplifiedpi);
    REQUIRE_THAT(simplifiedRealpi.GetValue(), Catch::Matchers::WithinAbs(0, EPSILON));

    auto simplified7pi6 = sine7pi6.Accept(simplifyVisitor).value();
    REQUIRE(simplified7pi6->Is<Oasis::Real>());

    auto simplifiedReal7pi6 = dynamic_cast<Oasis::Real&>(*simplified7pi6);
    REQUIRE_THAT(simplifiedReal7pi6.GetValue(), Catch::Matchers::WithinAbs(-0.5, EPSILON));

    auto simplified5pi4 = sine5pi4.Accept(simplifyVisitor).value();
    REQUIRE(simplified5pi4->Is<Oasis::Real>());

    auto simplifiedReal5pi4 = dynamic_cast<Oasis::Real&>(*simplified5pi4);
    REQUIRE_THAT(simplifiedReal5pi4.GetValue(), Catch::Matchers::WithinAbs(-1 * std::sqrt(2)/2, EPSILON));

    auto simplified4pi3 = sine4pi3.Accept(simplifyVisitor).value();
    REQUIRE(simplified4pi3->Is<Oasis::Real>());

    auto simplifiedReal4pi3 = dynamic_cast<Oasis::Real&>(*simplified4pi3);
    REQUIRE_THAT(simplifiedReal4pi3.GetValue(), Catch::Matchers::WithinAbs(-1 * std::sqrt(3)/2, EPSILON));

    auto simplified3pi2 = sine3pi2.Accept(simplifyVisitor).value();
    REQUIRE(simplified3pi2->Is<Oasis::Real>());

    auto simplifiedReal3pi2 = dynamic_cast<Oasis::Real&>(*simplified3pi2);
    REQUIRE_THAT(simplifiedReal3pi2.GetValue(), Catch::Matchers::WithinAbs(-1, EPSILON));

    auto simplified5pi3 = sine5pi3.Accept(simplifyVisitor).value();
    REQUIRE(simplified5pi3->Is<Oasis::Real>());

    auto simplifiedReal5pi3 = dynamic_cast<Oasis::Real&>(*simplified5pi3);
    REQUIRE_THAT(simplifiedReal5pi3.GetValue(), Catch::Matchers::WithinAbs(-1 * std::sqrt(3)/2, EPSILON));

    auto simplified7pi4 = sine7pi4.Accept(simplifyVisitor).value();
    REQUIRE(simplified7pi4->Is<Oasis::Real>());

    auto simplifiedReal7pi4 = dynamic_cast<Oasis::Real&>(*simplified7pi4);
    REQUIRE_THAT(simplifiedReal7pi4.GetValue(), Catch::Matchers::WithinAbs(-1 * std::sqrt(2)/2, EPSILON));

    auto simplified11pi6 = sine11pi6.Accept(simplifyVisitor).value();
    REQUIRE(simplified11pi6->Is<Oasis::Real>());

    auto simplifiedReal11pi6 = dynamic_cast<Oasis::Real&>(*simplified11pi6);
    REQUIRE_THAT(simplifiedReal11pi6.GetValue(), Catch::Matchers::WithinAbs(-0.5, EPSILON));
}
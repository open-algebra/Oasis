#include "Common.hpp"

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

TEST_CASE("Exact Sine", "[Sine]")
{

    Oasis::Sine sinepi6 {
        Oasis::Divide {
            Oasis::Pi{},
            Oasis::Real {6.0},
        }
    };

    OASIS_CAPTURE_WITH_SERIALIZER(sinepi6);

    auto simplified = sinepi6.Accept(simplifyVisitor).value();
    REQUIRE(simplified->Is<Oasis::Real>());

    auto simplifiedReal = dynamic_cast<Oasis::Real&>(*simplified);
    REQUIRE(simplifiedReal.GetValue() == 0.5);
}
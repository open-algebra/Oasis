/**
 * Created by Matthew McCall on 3/10/25.
 */
#include <emscripten/bind.h>

#include "Oasis/Expression.hpp"
#include "Oasis/FromString.hpp"
#include "Oasis/MathMLSerializer.hpp"

namespace
{
    auto Simplify(std::shared_ptr<Oasis::Expression> expression_ptr) -> std::shared_ptr<Oasis::Expression>
    {
        return expression_ptr->Simplify();
    }

    auto FromInFixTrivial(const std::string& str) -> std::shared_ptr<Oasis::Expression>
    {
        return Oasis::FromInFix(str).value_or(nullptr);
    }

    auto ToMathMLString(std::shared_ptr<Oasis::Expression> expression_ptr) -> std::string
    {
        const auto& expression = *expression_ptr;

        tinyxml2::XMLDocument doc;
        Oasis::MathMLSerializer serializer(doc);
        doc.InsertEndChild(expression.Accept(serializer).value());

        tinyxml2::XMLPrinter printer;
        doc.Print(&printer);

        return{ printer.CStr() };
    }
}

EMSCRIPTEN_BINDINGS(Oasis)
{
    emscripten::class_<Oasis::Expression>("Expression")
        .smart_ptr<std::shared_ptr<Oasis::Expression>>("Expression");

    emscripten::function("Simplify", &Simplify);

    emscripten::function("PreProcessInFix", &Oasis::PreProcessInFix);
    emscripten::function("FromInFix", &FromInFixTrivial);

    emscripten::function("ToMathMLString", &ToMathMLString);
}
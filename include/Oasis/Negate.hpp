//
// Created by Matthew McCall on 3/29/24.
//

#ifndef NEGATE_HPP
#define NEGATE_HPP

#include "Multiply.hpp"
#include "fmt/core.h"

#include "UnaryExpression.hpp"

namespace Oasis {

template <typename OperandT>
class Negate final : public UnaryExpression<Negate, OperandT> {
public:
    Negate() = default;
    Negate(const Negate& other)
        : UnaryExpression<Negate, OperandT>(other)
    {
    }

    explicit Negate(const OperandT& operand)
        : UnaryExpression<Negate, OperandT>(operand)
    {
    }

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> override
    {
        return Multiply {
            Real { -1.0 },
            this->GetOperand()
        }
            .Simplify();
    }

    auto Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression> override
    {
        return Multiply {
            Real { -1.0 },
            this->GetOperand()
        }
            .Simplify(subflow);
    }

    [[nodiscard]] auto ToString() const -> std::string override
    {
        return fmt::format("-({})", this->GetOperand().ToString());
    }

    auto ToMathMLElement(tinyxml2::XMLDocument& doc) const -> tinyxml2::XMLElement* override
    {
        // mrow
        tinyxml2::XMLElement* const mrow = doc.NewElement("mrow");

        // mo
        tinyxml2::XMLElement* const mo = doc.NewElement("mo");
        mo->SetText("-");
        mrow->InsertEndChild(mo);

        // (
        tinyxml2::XMLElement* const leftParen = doc.NewElement("mo");
        leftParen->SetText("(");
        mrow->InsertEndChild(leftParen);

        mrow->InsertEndChild(this->op->ToMathMLElement(doc));

        // )
        tinyxml2::XMLElement* const rightParen = doc.NewElement("mo");
        rightParen->SetText(")");
        mrow->InsertEndChild(rightParen);

        return mrow;
    }

    IMPL_SPECIALIZE_UNARYEXPR(Negate, OperandT)

    EXPRESSION_TYPE(Negate)
    EXPRESSION_CATEGORY(UnExp)
};

} // Oasis

#endif // NEGATE_HPP

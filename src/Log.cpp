//
// Created by Matthew McCall on 10/6/23.
//

#include "Oasis/Log.hpp"

namespace Oasis {
Log<Expression>::Log(const Expression& base, const Expression& argument)
    : BinaryExpression(base, argument)
{
}

auto Log<Expression>::Simplify() const -> std::unique_ptr<Expression>
{
    return Copy();
}

auto Log<Expression>::Simplify(tf::Subflow& subflow) const -> std::unique_ptr<Expression>
{
    return Copy(subflow);
}

auto Log<Expression>::ToString() const -> std::string
{
    return fmt::format("log({}, {})", mostSigOp->ToString(), leastSigOp->ToString());
}

auto Log<Expression>::Specialize(const Expression& other) -> std::unique_ptr<Log>
{
    if (!other.Is<Log>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize();
    return std::make_unique<Log>(dynamic_cast<const Log<Expression>&>(*otherGeneralized));
}

auto Log<Expression>::Specialize(const Expression& other, tf::Subflow& subflow) -> std::unique_ptr<Log>
{
    if (!other.Is<Log>()) {
        return nullptr;
    }

    auto otherGeneralized = other.Generalize(subflow);
    return std::make_unique<Log>(dynamic_cast<const Log<Expression>&>(*otherGeneralized));
}

} // Oasis

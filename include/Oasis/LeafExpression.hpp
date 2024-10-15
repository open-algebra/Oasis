//
// Created by Matthew McCall on 7/2/23.
//

#ifndef OASIS_LEAFEXPRESSION_HPP
#define OASIS_LEAFEXPRESSION_HPP

#include "Expression.hpp"
#include "Serialization.hpp"

namespace Oasis {

/**
 * A leaf expression.
 *
 * A leaf expression is an expression that has no children.
 *
 * @tparam DerivedT The type of the derived class.
 */
template <typename DerivedT>
class LeafExpression : public Expression {
public:
    [[nodiscard]] auto Copy() const -> std::unique_ptr<Expression> final
    {
        return std::make_unique<DerivedT>(*static_cast<const DerivedT*>(this));
    }

    [[nodiscard]] auto StructurallyEquivalent(const Expression& other) const -> bool final
    {
        return this->GetType() == other.GetType();
    }

    [[nodiscard]] auto Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression> override
    {
        return Generalize()->Integrate(integrationVariable);
    }

    [[nodiscard]] auto Differentiate(const Expression& differentiationVariable) const -> std::unique_ptr<Expression> override
    {
        return Generalize()->Differentiate(differentiationVariable);
    }
    auto Substitute(const Expression&, const Expression&) -> std::unique_ptr<Expression> override
    {
        return this->Copy();
    }

    void Serialize(SerializationVisitor& visitor) const override
    {
        const auto generalized = Generalize();
        const auto& derivedGeneralized = dynamic_cast<const DerivedT&>(*generalized);
        visitor.Serialize(derivedGeneralized);
    }
};

} // Oasis

#endif // OASIS_LEAFEXPRESSION_HPP

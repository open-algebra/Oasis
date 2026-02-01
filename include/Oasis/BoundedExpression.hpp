//
// Created by Matthew McCall on 4/30/24.
//

#ifndef OAISIS_BOUNDEDEXPRESSION_HPP
#define OAISIS_BOUNDEDEXPRESSION_HPP

#include <cassert>

#include "Expression.hpp"

namespace Oasis {

/**
 * A concept base class for both Unary and BoundedBinary expressions.
 * @note This class is not intended to be used directly by end users.
 *
 * @section Parameters
 * @tparam LowerBoundT The lower bound of the expression.
 * @tparam UpperBoundT The upper bound of the expression.
 */
template <IExpression LowerBoundT = Expression, IExpression UpperBoundT = Expression>
class BoundedExpression : public Expression {
public:
    BoundedExpression() = default;

    BoundedExpression(const BoundedExpression& other)
    {
        if (other.HasLowerBound())
            SetLowerBound(other.GetLowerBound());
        if (other.HasUpperBound())
            SetUpperBound(other.GetUpperBound());
    }

    BoundedExpression(const LowerBoundT& lowerBound, const UpperBoundT& upperBound)
    {
        SetLowerBound(lowerBound);
        SetUpperBound(upperBound);
    }

    [[nodiscard]] auto HasLowerBound() const -> bool
    {
        return lowerBound != nullptr;
    }

    [[nodiscard]] auto HasUpperBound() const -> bool
    {
        return upperBound != nullptr;
    }

    [[nodiscard]] auto GetLowerBound() const -> const LowerBoundT&
    {
        assert(HasLowerBound() && "LowerBound is not set.");
        return *lowerBound;
    }

    [[nodiscard]] auto GetUpperBound() const -> const UpperBoundT&
    {
        assert(HasUpperBound() && "UpperBound is not set.");
        return *upperBound;
    }

    template <typename T>
        requires IsAnyOf<T, LowerBoundT, Expression>
    void SetLowerBound(const T& expr)
    {
        if constexpr (std::is_same_v<T, LowerBoundT>) {
            lowerBound = std::make_unique<LowerBoundT>(expr);
        } else {
            lowerBound = expr.Copy();
        }
    }

    template <typename T>
        requires IsAnyOf<T, UpperBoundT, Expression>
    void SetUpperBound(const T& expr)
    {
        if constexpr (std::is_same_v<T, UpperBoundT>) {
            upperBound = std::make_unique<UpperBoundT>(expr);
        } else {
            upperBound = expr.Copy();
        }
    }

private:
    std::unique_ptr<LowerBoundT> lowerBound;
    std::unique_ptr<UpperBoundT> upperBound;
};

} // Oasis

#endif // OAISIS_BOUNDEDEXPRESSION_HPP

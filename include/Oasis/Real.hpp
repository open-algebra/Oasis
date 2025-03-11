//
// Created by Matthew McCall on 7/2/23.
//

#ifndef OASIS_REAL_HPP
#define OASIS_REAL_HPP

#include "LeafExpression.hpp"
#include "Unit.hpp"
#include "GlobalUnitGraph.hpp"
#include <boost/units/systems/si.hpp>
#include <boost/units/io.hpp>
#include <boost/units/cmath.hpp>
#include <boost/units/systems/si/length.hpp>
#include <boost/units/systems/si/io.hpp>
#include <boost/units/conversion.hpp>


namespace Oasis {

/**
 * A real number.
 */
class Real : public LeafExpression<Real> {
public:
    Real() = default;
    Real(const Real& other) = default;

    explicit Real(double value, Unit unit = Unit::None);

    [[nodiscard]] auto Equals(const Expression& other) const -> bool final;

    EXPRESSION_TYPE(Real)
    EXPRESSION_CATEGORY(UnExp)

    auto ConvertTo(Unit targetUnit) const -> Real;
    /**
     * Gets the value of the real number.
     * @return The value of the real number.
     */
    [[nodiscard]] auto GetValue() const -> double;

    [[nodiscard]] auto GetUnit() const -> Unit;

    [[nodiscard]] auto Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto Differentiate(const Expression&) const -> std::unique_ptr<Expression> final;

    auto operator=(const Real& other) -> Real& = default;

private:
    double value {};
    Unit unit = Unit::None;

};

} // Oasis

#endif // OASIS_REAL_HPP

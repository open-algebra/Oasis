//
// Created by Matthew McCall on 4/21/24.
//

#ifndef FROMSTRING_HPP
#define FROMSTRING_HPP

#include <expected>
#include <memory>

#include "Oasis/Expression.hpp"

namespace Oasis {

enum class ParseImaginaryOption {
    UseI,
    UseJ
};

/**
 * The PreProcessInFix class is a class used by end users to convert string representations of their
 * expressions and equations to usable objects that may be used within other Oasis objects.
 *
 * Oasis objects do not accept raw strings for expressions and must be processed by using a combination of:
 * -PreProcessInFix
 * -FromInFix
 *
 * @note Cast PreProcessInFix return values as const auto.
 *
 * @section param Parameters:
 * @tparam str a string representation of an expression to convert.
 *
 * @return an std::string to be used by the Oasis::FromInFix class.
 *
 * @section ex Example usage:
 * @code
 * std::string expr = {"5x-5y+10z-15*(x+5)"};
 *
 * const auto preproc = Oasis::PreProcessInFix(exp1);
 * // Now may be fed into FromInFix
 * @endcode
 */
auto PreProcessInFix(const std::string& str) -> std::string;

/**
 * The FromInFix class uses the results of a processed PreProcessInFix object and processes it for use
 * in other Oasis objects.
 *
 * Oasis objects do not accept raw strings for expressions and must be processed by using a combination of:
 * -PreProcessInFix
 * -FromInFix
 *
 * @section param Parameters:
 * @tparam str an std::string that will be converted into an expression Oasis may use.
 *
 * @return an std::unique_ptr<Oasis::Expression> object to be used in Oasis classes.
 *
 * @section exam Example usage:
 * @code
 * std::string expr = {"5x-5y+10z-15*(x+5)"};
 *
 * const auto preproc = Oasis::PreProcessInFix(exp1);
 *
 * auto midResult = Oasis::FromInFix(preproc1); // Call FromInFix on the PreProcessInFix object.
 *
 * const std::unique_ptr<Oasis::Expression> expression = std::move(midResult1).value();
 * // Use std::move to now move the processed result into an Oasis Expression.
 * // expression may now be used in Oasis classes, such as:
 *
 * Oasis::Integral myIntegral {
 *  *expression, // Make sure to use a pointer for the expression
 *  Oasis::Variable{"x"}
 * };
 * // myIntegral holds the integral of "5x-5y+10z-15*(x+5)" integrated with respect to x.
 *
 * @endcode
 */
using FromInFixResult = std::expected<std::unique_ptr<Expression>, std::string>;
auto FromInFix(const std::string& str, ParseImaginaryOption option = ParseImaginaryOption::UseI) -> FromInFixResult;

}

#endif // FROMSTRING_HPP

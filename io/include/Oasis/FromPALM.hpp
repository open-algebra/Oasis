//
// Created by codin on 10/14/25.
//

#ifndef OASIS_FROMPALM_HPP
#define OASIS_FROMPALM_HPP

#include "Oasis/Expression.hpp"

namespace Oasis {

enum class ParseError {
    None,
    UnexpectedToken,
    MissingClosingParen,
    InvalidNumberFormat,
    IncompleteExpression,
    UnknownOperator,
    UnexpectedEndOfInput
};

/** Parses an expression from a PALM string.
 *
 * @param palmString The PALM string to parse.
 * @return The parsed expression, or nullptr if the string could not be parsed.
 */
auto FromPALM(const std::string& palmString) -> std::expected<std::unique_ptr<Expression>, ParseError>;
}

#endif // OASIS_FROMPALM_HPP

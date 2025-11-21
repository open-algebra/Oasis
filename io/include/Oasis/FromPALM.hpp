//
// Created by codin on 10/14/25.
//

#ifndef OASIS_FROMPALM_HPP
#define OASIS_FROMPALM_HPP

#include "Oasis/Expression.hpp"
#include "../src/PALMTypes.hpp"

namespace Oasis {

enum class ParseErrorOld {
    None,
    IncompleteExpression, // expression opens but required operands/args missing
    MissingClosingParen, // '(' without matching ')'
    UnexpectedEndOfInput, // input ended while more tokens were required
    UnexpectedToken, // token present but not valid in this position
    InvalidNumberFormat, // number failed to parse
    UnknownOperator, // operator not recognized
    TooManyOperands, // more expressions than operator's arity allows
    TooFewOperands, // fewer expressions than operator's arity requires
    AmbiguousParse, // grammar ambiguity or overload not resolvable
    LexicalError, // tokenization error (bad character, unterminated string)
    Other // fallback
};

/* Types of parse errors. */
enum struct PALMParseErrorType {
    None,
    InvalidNumberFormat,
    LexicalError,
    MissingOperands,
    ExtraOperands,
};

/* Represents a parse error in PALM parsing. */
struct PALMParseError {
    PALMToken token;
    PALMParseErrorType type;
    std::string message;
};

/** Equality operator for PALMParseError.
 *
 * @param lhs The left-hand side PALMParseError.
 * @param rhs The right-hand side PALMParseError.
 * @return True if the two errors are equal, false otherwise.
 */
bool operator==(const PALMParseError& lhs, const PALMParseError& rhs);

/** Parses an expression from a PALM string.
 *
 * @param palmString The PALM string to parse.
 * @return The parsed expression, or nullptr if the string could not be parsed.
 */
auto FromPALM(const std::string& palmString) -> std::expected<std::unique_ptr<Expression>, ParseErrorOld>;

auto FromPALMNew(const std::string& palmString) -> std::expected<std::unique_ptr<Expression>, PALMParseError>;
}

#endif // OASIS_FROMPALM_HPP

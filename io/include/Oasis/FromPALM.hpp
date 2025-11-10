//
// Created by codin on 10/14/25.
//

#ifndef OASIS_FROMPALM_HPP
#define OASIS_FROMPALM_HPP

#include "Oasis/Expression.hpp"

namespace Oasis {

enum class ParseError {
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

// struct ParseError {
//     ParseErrorType type;
//     size_t token_index; // index in token stream where error detected
//     size_t char_offset; // approximate character offset in source
//     std::string got; // token text that caused the error (if any)
//     std::string expected; // brief description of what was expected
//     std::string message; // human-friendly diagnostic
// };

/** Parses an expression from a PALM string.
 *
 * @param palmString The PALM string to parse.
 * @return The parsed expression, or nullptr if the string could not be parsed.
 */
auto FromPALM(const std::string& palmString) -> std::expected<std::unique_ptr<Expression>, ParseError>;

auto FromPALMNew(const std::string& palmString) -> std::expected<std::unique_ptr<Oasis::Expression>, Oasis::ParseError>;
}

#endif // OASIS_FROMPALM_HPP

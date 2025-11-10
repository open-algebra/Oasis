//
// Created by codin on 11/9/25.
//

#ifndef OASIS_PALMTYPES_HPP
#define OASIS_PALMTYPES_HPP

#include "Oasis/Expression.hpp"

#include <boost/bimap/bimap.hpp>
#include <boost/bimap/unordered_multiset_of.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <string>
#include <string_view>

namespace Oasis {
/** Types of PALM tokens. */
enum class PALMTokenType {
    Operator,
    Identifier,
    Number,
    Punctuator,
    EndOfInput,
    Unknown
};

/** Represents a token in the PALM language. */
struct PALMToken {
    PALMTokenType type;   // type of token
    std::string text;     // raw lexeme
    size_t offset = 0;    // byte offset in source
    size_t length = 0;    // length in bytes
    size_t line = 1;      // optional
    size_t column = 1;    // optional
    size_t tokenIndex = 0; // index in token stream
};

/** Operators are taken from ExpressionType enum in Expression.hpp
 * @see ExpressionType
 */

/** Types of PALM punctuators. */
enum class PALMPunctuatorType {
    StartExpression,
    EndExpression
};

/** Bi-directional mapping between PALM operator tokens and their corresponding ExpressionType. */
using PALMOperatorBimap = boost::bimaps::bimap<
    boost::bimaps::unordered_multiset_of<std::string_view>,
    boost::bimaps::unordered_set_of<ExpressionType>>;

/** Bi-directional mapping between PALM punctuator tokens and their corresponding PALMPunctuatorType. */
using PALMPunctuatorBimap = boost::bimaps::bimap<std::string_view, PALMPunctuatorType>;







/** Checks if a character is a PALM operator.
 *
 * @param ch The character to check.
 * @return True if the character is a PALM operator, false otherwise.
 */
auto isPALMOperator(char ch) -> bool;
}

#endif // OASIS_PALMTYPES_HPP

//
// Created by codin on 11/9/25.
//

#ifndef OASIS_PALMSPEC_HPP
#define OASIS_PALMSPEC_HPP

#include "PALMTypes.hpp"
#include "PALMConsts.hpp"

#include <regex>

namespace Oasis {

/** Checks if a token is a PALM operator.
 *
 * @param token The token to check.
 * @return True if the token is a PALM operator, false otherwise.
 */
static auto isTokenPALMOperator(const std::string& token) -> bool
{
    return kPALMOperatorBimap.left.count(token) != 0;
}

/** Checks if a token is a PALM identifier.
 *
 * @param token The token to check.
 * @return True if the token is a PALM identifier, false otherwise.
 */
static auto isTokenPALMIdentifier(const std::string& token) -> bool
{
    static const std::regex identifierRegex { std::string(kPALMIdentifierRegex)};
    return std::regex_match(token, identifierRegex);
}

/** Checks if a token is a PALM number.
 *
 * @param token The token to check.
 * @return True if the token is a PALM number, false otherwise.
 */
static auto isTokenPALMNumber(const std::string& token) -> bool
{
    static const std::regex numberRegex { std::string(kPALMNumberRegex)};
    return std::regex_match(token, numberRegex);
}

/** Checks if a token is a PALM punctuator.
 *
 * @param token The token to check.
 * @return True if the token is a PALM punctuator, false otherwise.
 */
static auto isTokenPALMPunctuator(const std::string& token) -> bool
{
    return kPALMPunctuatorBimap.left.count(token) != 0;
}


}

#endif // OASIS_PALMSPEC_HPP
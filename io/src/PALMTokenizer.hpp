//
// Created by codin on 11/9/25.
//

#ifndef OASIS_PALMTOKENIZER_HPP
#define OASIS_PALMTOKENIZER_HPP

#include "PALMTypes.hpp"

#include <unordered_set>

namespace Oasis {

/** A tokenizer for the PALM language.
 *
 * This class reads from an input stream and produces PALM tokens.
 */
class PALMTokenizer {
public:
    explicit PALMTokenizer(std::istream& inputStream)
        : inputStream(inputStream)
        , currentToken(nullptr)
    {
    }

    auto lookahead() -> PALMToken;
    auto match(const PALMToken& token) -> bool;
    auto match(const std::string& lexeme) -> bool;

    auto eof() const -> bool;

private:
    std::istream& inputStream;
    std::unique_ptr<PALMToken> currentToken;

    static std::unordered_set<std::string_view> buildPunctuatorPrefixes();
    static const std::unordered_set<std::string_view>& punctuatorPrefixes();

    auto nextToken() const -> PALMToken;

    static auto tryMatchPunctuator(std::istream& in) -> std::string;
};

}
#endif // OASIS_PALMTOKENIZER_HPP

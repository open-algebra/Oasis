//
// Created by codin on 11/9/25.
//

#include "PALMTokenizer.hpp"
#include "PALMSpec.hpp"

/** Builds a set of all prefixes of PALM punctuators.
 *
 * If a punctuator is "===", its prefixes are "=", "==", and "===".
 * @return A set of string views representing all prefixes of PALM punctuators.
 */
std::unordered_set<std::string_view> Oasis::PALMTokenizer::buildPunctuatorPrefixes()
{
    std::unordered_set<std::string_view> prefixes;
    for (const auto& [text, _] : PALMPunctuatorBimap().left) {
        for (size_t i = 1; i <= text.size(); ++i) {
            prefixes.insert(text.substr(0, i));
        }
    }
    return prefixes;
}

/** Returns the set of all prefixes of PALM punctuators.
 *
 * @return A reference to the set of string views representing all prefixes of PALM punctuators.
 */
const std::unordered_set<std::string_view>& Oasis::PALMTokenizer::punctuatorPrefixes()
{
    static const auto prefixes = buildPunctuatorPrefixes();
    return prefixes;
}

/** Reads the next token from the input stream.
 *
 * @return The next PALM token.
 */
auto Oasis::PALMTokenizer::nextToken() const -> PALMToken
{
    // Create new token
    PALMToken token = {};

    if (currentToken) { // Update position based on previous token
        token.tokenIndex = currentToken->tokenIndex + 1;
        token.offset = currentToken->offset + currentToken->length;
        token.line = currentToken->line;
        token.column = currentToken->column + currentToken->length;
    }

    // Skip whitespace
    char ch;
    while (inputStream.get(ch)) {
        if (!std::isspace(static_cast<unsigned char>(ch))) {
            inputStream.unget(); // Put back the non-whitespace character
            break;
        }
        if (ch == '\n') {
            token.line++;
            token.column = 1;
        } else {
            token.column++;
        }
        token.offset++;
    }

    // End of Input Token
    if (!inputStream.get(ch)) {
        token.type = PALMTokenType::EndOfInput;
        token.text = "";
        token.length = 0;
        return token;
    }

    // Put back the character for further processing
    inputStream.unget();

    // Punctuator Token
    if (const auto punctuator = tryMatchPunctuator(inputStream); !punctuator.empty()) {
        token.type = PALMTokenType::Punctuator;
        token.text = punctuator;
        token.length = punctuator.length();
        return token;
    }

    // Build lexeme until whitespace or punctuator
    std::stringbuf lexemeBuffer;
    std::streampos lexemeTail = inputStream.tellg();

    while (inputStream.get(ch)) {
        if (std::isspace(static_cast<unsigned char>(ch))) {
            inputStream.unget(); // Put back the whitespace character
            break;
        }

        // Check for punctuator match
        inputStream.unget();
        if (!tryMatchPunctuator(inputStream).empty()) { // Found a punctuator
            inputStream.seekg(lexemeTail); // Reset to start of punctuator
            break;
        }
        inputStream.get(ch); // Consume character
        lexemeBuffer.sputc(ch);
        lexemeTail = inputStream.tellg();
    }

    // Save lexeme info
    token.text = lexemeBuffer.str();
    token.length = token.text.length();

    // Operator Token
    if (isTokenPALMOperator(lexemeBuffer.str())) {
        token.type = PALMTokenType::Operator;
        return token;
    }

    // Identifier Token
    if (isTokenPALMIdentifier(lexemeBuffer.str())) {
        token.type = PALMTokenType::Identifier;
        return token;
    }

    // Number Token
    if (isTokenPALMNumber(lexemeBuffer.str())) {
        token.type = PALMTokenType::Number;
        return token;
    }

    // Unknown Token
    token.type = PALMTokenType::Unknown;
    return token;
}

/** Tries to match a PALM punctuator from the input stream.
 *
 * @param in The input stream to read from.
 * @return The matched punctuator string, or an empty string if no match.
 */
auto Oasis::PALMTokenizer::tryMatchPunctuator(std::istream& in) -> std::string
{
    std::string buffer;
    const std::streampos startPos = in.tellg();

    // Find Matching Punctuator
    while (true) {
        char ch;
        if (!in.get(ch)) {
            break; // End of stream
        }
        buffer.push_back(ch);

        if (isTokenPALMPunctuator(buffer)) {
            // Found a matching punctuator
            return buffer;
        }

        if (!punctuatorPrefixes().contains(buffer)) {
            // No longer a valid prefix
            break;
        }
    }

    in.clear();
    in.seekg(startPos); // Reset stream position
    return ""; // No match found
}



auto Oasis::PALMTokenizer::lookahead() -> PALMToken
{
    if (!currentToken) {
        currentToken = std::make_unique<PALMToken>(nextToken());
    }
    return *currentToken;
}

auto Oasis::PALMTokenizer::match(const PALMToken& token) -> bool
{
    return match(token.text);
}

auto Oasis::PALMTokenizer::match(const std::string& lexeme) -> bool
{
    if (lookahead().text == lexeme) {
        currentToken = std::make_unique<PALMToken>(nextToken());
        return true;
    }
    return false;
}

auto Oasis::PALMTokenizer::eof() const -> bool
{
    return inputStream.eof();
}



//
// Created by codin on 10/14/25.
//

#include <sstream>
#include <string>
#include <vector>

#include "Oasis/Add.hpp"
#include "Oasis/Derivative.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/EulerNumber.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/FromPALM.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Matrix.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Negate.hpp"
#include "Oasis/Pi.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Subtract.hpp"

#include "Oasis/Variable.hpp"

// Define PALM tokens
constexpr std::string_view PALM_OPEN_PARENS = "(";
constexpr std::string_view PALM_CLOSE_PARENS = ")";

constexpr std::string_view PALM_REAL = "real";
constexpr std::array<std::string_view, 2> PALM_IMAGINARY = { "i", "j" };
constexpr std::string_view PALM_VARIABLE = "var";
constexpr std::string_view PALM_ADD = "+";
constexpr std::string_view PALM_SUBTRACT = "-";
constexpr std::string_view PALM_MULTIPLY = "*";
constexpr std::string_view PALM_DIVIDE = "/";
constexpr std::string_view PALM_EXPONENT = "^";
constexpr std::string_view PALM_LOG = "log";
constexpr std::string_view PALM_INTEGRAL = "int";
constexpr std::string_view PALM_NEGATE = "neg";
constexpr std::string_view PALM_DERIVATIVE = "d";
// constexpr std::string_view PALM_MATRIX = "matrix";
constexpr std::array<std::string_view, 1> PALM_PI = { "pi" };
constexpr std::string_view PALM_EULER = "e";

namespace Oasis {
/** A simple token stream for parsing PALM strings.
 *
 * This class provides basic functionality to read tokens from a string stream,
 * including peeking at the next token without consuming it.
 */
class TokenStream {
    std::istringstream stream;
    std::istringstream::pos_type pos;

public:
    explicit TokenStream(const std::string& input)
        : stream(input)
        , pos(0)
    {
    }

    auto peek() -> std::string
    {
        const std::streampos stream_pos = stream.tellg();
        std::string token;
        stream >> token;
        stream.clear(); // Clear EOF/fail flags
        stream.seekg(stream_pos); // Restore position
        return token;
    }

    auto pop() -> std::string
    {
        std::string token;
        stream >> token;
        pos = stream.tellg();
        return token;
    }

    auto eof() const -> bool
    {
        return stream.eof();
    }

    auto index() const -> std::istringstream::pos_type
    {
        return pos;
    }
};

/** Tokenizes a PALM string into a stream of tokens.
 *
 * @param palmString The PALM string to tokenize.
 * @return A stream of tokens.
 */
auto TokenizePALM(const std::string& palmString) -> TokenStream;

/** Checks if a token is in a given array of strings.
 * @param token The token to check.
 * @param array The array of strings to check against.
 * @return True if the token is in the array, false otherwise.
 */
template <size_t N>
auto isTokenFromArray(const std::string& token, std::array<std::string_view, N> array) -> bool;

/** Checks if a token is a real number token.
 * @param token The token to check.
 * @return True if the token is a real number token, false otherwise.
 */
auto isImaginaryToken(const std::string& token) -> bool;

/** Checks if a token is a pi token.
 * @param token The token to check.
 * @return True if the token is a pi token, false otherwise.
 */
auto isPiToken(const std::string& token) -> bool;

/** Parses an expression from a stream of tokens.
 * Expression -> '(' Operation ')' | Number
 * @param tokens The stream of tokens to parse.
 * @return The parsed expression.
 */
auto ParseExpression(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>;

/** Parses an operation from a stream of tokens.
 * Operation -> Add | Subtract | Multiply | Divide | Exponent
 * @param tokens The stream of tokens to parse.
 * @return The parsed expression.
 */
auto ParseOperation(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>;

/** Parses a real number from a stream of tokens.
 * Real -> std::stod(token)
 * @param tokens The stream of tokens to parse.
 * @return The parsed expression.
 */
auto ParseReal(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>;

/** Parses an imaginary unit from a stream of tokens.
 * Imaginary -> 'i' | 'j'
 * @param tokens The stream of tokens to parse.
 * @return The parsed expression.
 */
auto ParseImaginary(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>;

/** Parses a variable from a stream of tokens.
 * Variable -> std::string(token)
 * @param tokens The stream of tokens to parse.
 * @return The parsed expression.
 */
auto ParseVariable(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>;

/** Parses an addition operation from a stream of tokens.
 * Add -> '+' BinaryElements
 * @param tokens The stream of tokens to parse.
 * @return The parsed expression.
 */
auto ParseAdd(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>;

/** Parses a subtraction operation from a stream of tokens.
 * Subtract -> '-' BinaryElements
 * @param tokens The stream of tokens to parse.
 * @return The parsed expression.
 */
auto ParseSubtract(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>;

/** Parses a multiplication operation from a stream of tokens.
 * Multiply -> '*' BinaryElements
 * @param tokens The stream of tokens to parse.
 * @return The parsed expression.
 */
auto ParseMultiply(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>;

/** Parses a division operation from a stream of tokens.
 * Divide -> '/' BinaryElements
 * @param tokens The stream of tokens to parse.
 * @return The parsed expression.
 */
auto ParseDivide(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>;

/** Parses an exponentiation operation from a stream of tokens.
 * Exponent -> '^' BinaryElements
 * @param tokens The stream of tokens to parse.
 * @return The parsed expression.
 */
auto ParseExponent(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>;

/** Parses a logarithm operation from a stream of tokens.
 * Log -> 'log' BinaryElements
 * @param tokens The stream of tokens to parse.
 * @return The parsed expression.
 */
auto ParseLog(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>;

/** Parses an integral operation from a stream of tokens.
 * Integral -> 'int' BinaryElements
 * @param tokens The stream of tokens to parse.
 * @return The parsed expression.
 */
auto ParseIntegral(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>;

/** Parses a negate operation from a stream of tokens.
 * Negate -> 'neg' UnaryElements
 * @param tokens The stream of tokens to parse.
 * @return The parsed expression.
 */
auto ParseNegate(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>;

/** Parses a derivative operation from a stream of tokens.
 * Derivative -> 'd' BinaryElements
 * @param tokens The stream of tokens to parse.
 * @return The parsed expression.
 */
auto ParseDerivative(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>;

/** Parses a matrix operation from a stream of tokens.
 * Matrix -> 'matrix' BinaryElements
 * @param tokens The stream of tokens to parse.
 * @return The parsed expression.
 */
auto ParseMatrix(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>;

/** Parses a pi constant from a stream of tokens.
 * Pi -> 'pi' | '\pi' | 'π'
 * @param tokens The stream of tokens to parse.
 * @return The parsed expression.
 */
auto ParsePi(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>;

/** Parses an euler number constant from a stream of tokens.
 * EulerNumber -> 'e'
 * @param tokens The stream of tokens to parse.
 * @return The parsed expression.
 */
auto ParseEulerNumber(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>;

/** Parses an operation that takes two operands from a stream of tokens.
 * @param token The operator token.
 * @param tokens The stream of tokens to parse.
 * @return The parsed expression.
 */
template <template <typename, typename> typename T>
    requires IAssociativeAndCommutative<T>
auto ParseBinaryOperationAssociativeAndCommutative(std::string_view token, TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>;

/** Parses an operation that takes two operands from a stream of tokens.
 * @param token The operator token.
 * @param tokens The stream of tokens to parse.
 * @return The parsed expression.
 */
template <template <typename, typename> typename T>
    requires IExpression<T<Expression, Expression>> && (!IAssociativeAndCommutative<T>)
auto ParseBinaryOperationNonAssociativeAndCommutative(std::string_view token, TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>;

/** Parses binary elements from a stream of tokens.
 * BinaryElements -> Expression Expression Expression*
 * @param tokens The stream of tokens to parse.
 * @return The parsed expressions.
 */
auto ParseBinaryOperationElements(TokenStream& tokens) -> std::expected<std::vector<std::unique_ptr<Expression>>, ParseError>;

/** Parses unary elements from a stream of tokens.
 * UnaryElements -> Expression Expression*
 * @param tokens The stream of tokens to parse.
 * @return The parsed expressions.
 */
auto ParseUnaryOperationElements(TokenStream& tokens) -> std::expected<std::vector<std::unique_ptr<Expression>>, ParseError>;

/** Parses binary elements from a stream of tokens.
 * BinaryElements -> Expression*
 * @param tokens The stream of tokens to parse.
 * @return The parsed expressions.
 */
auto ParseNullaryOperationElements(TokenStream& tokens) -> std::expected<std::vector<std::unique_ptr<Expression>>, ParseError>;

// PALM Grammar Start -> Expression $$
auto FromPALM(const std::string& palmString) -> std::expected<std::unique_ptr<Expression>, ParseError>
{
    // Tokenize the input string
    auto tokens = TokenizePALM(palmString);

    // Start -> Expression $$
    auto expr = ParseExpression(tokens); // Expression
    if (!expr) {
        return std::unexpected { expr.error() };
    }

    if (expr && tokens.eof()) { // $$
        return expr;
    }

    // Parsing failed
    return std::unexpected { ParseError::UnexpectedToken };
}

auto TokenizePALM(const std::string& palmString) -> TokenStream
{
    // Split by spaces
    return TokenStream(palmString);
}

template <size_t N>
auto isTokenFromArray(const std::string& token, const std::array<std::string_view, N> array) -> bool
{
    return std::ranges::find(array, token) != std::end(array);
}

auto isImaginaryToken(const std::string& token) -> bool
{
    return isTokenFromArray(token, PALM_IMAGINARY);
}

auto isPiToken(const std::string& token) -> bool
{
    return isTokenFromArray(token, PALM_PI);
}

auto ParseExpression(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>
{
    // Expression -> '(' Operation ')'
    if (tokens.peek() == PALM_OPEN_PARENS) {
        // Consume '('
        tokens.pop();

        // Consume Operation
        auto innerExpr = ParseOperation(tokens);
        if (!innerExpr) {
            return std::unexpected { innerExpr.error() };
        }

        // Consume ')'
        if (tokens.pop() != PALM_CLOSE_PARENS)
            return std::unexpected { ParseError::MissingClosingParen };

        return innerExpr;
    }

    if (tokens.peek().empty()) {
        return std::unexpected { ParseError::UnexpectedEndOfInput };
    }

    // Unable to parse expression
    return std::unexpected { ParseError::UnexpectedToken };
}

auto ParseOperation(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>
{
    // Peek at the next token
    std::string token = tokens.peek();

    // Operation -> Real
    if (token == PALM_REAL) {
        return ParseReal(tokens);
    }

    // Operation -> Imaginary
    if (isImaginaryToken(token)) {
        return ParseImaginary(tokens);
    }

    // Operation -> Variable
    if (token == PALM_VARIABLE) {
        return ParseVariable(tokens);
    }

    // Operation -> Add
    if (token == PALM_ADD) {
        return ParseAdd(tokens);
    }

    // Operation -> Subtract
    if (token == PALM_SUBTRACT) {
        return ParseSubtract(tokens);
    }

    // Operation -> Multiply
    if (token == PALM_MULTIPLY) {
        return ParseMultiply(tokens);
    }

    // Operation -> Divide
    if (token == PALM_DIVIDE) {
        return ParseDivide(tokens);
    }

    // Operation -> Exponent
    if (token == PALM_EXPONENT) {
        return ParseExponent(tokens);
    }

    // Operation -> Log
    if (token == PALM_LOG) {
        return ParseLog(tokens);
    }

    // Operation -> Integral
    if (token == PALM_INTEGRAL) {
        return ParseIntegral(tokens);
    }

    if (token == PALM_NEGATE) {
        return ParseNegate(tokens);
    }

    // Operation -> Derivative
    if (token == PALM_DERIVATIVE) {
        return ParseDerivative(tokens);
    }

    // Operation -> Pi
    if (isPiToken(token)) {
        return ParsePi(tokens);
    }

    // Operation -> EulerNumber
    if (token == PALM_EULER) {
        return ParseEulerNumber(tokens);
    }

    // Unable to parse operation
    return std::unexpected { ParseError::UnknownOperator };
}

auto ParseReal(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>
{
    // Peek at the next token
    if (tokens.peek() != PALM_REAL) {
        return std::unexpected { ParseError::UnexpectedToken };
    }
    // Consume 'real'
    tokens.pop();

    // Consume Number
    try {
        return std::make_unique<Real>(std::stod(tokens.pop()));
    } catch (const std::invalid_argument&) {
        return std::unexpected { ParseError::InvalidNumberFormat };
    } catch (const std::out_of_range&) {
        return std::unexpected { ParseError::InvalidNumberFormat };
    }
}

auto ParseImaginary(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>
{
    // Peek at the next token
    if (!isTokenFromArray(tokens.peek(), PALM_IMAGINARY)) {
        return std::unexpected { ParseError::UnexpectedToken };
    }

    // Consume 'i' or 'j'
    tokens.pop();

    return std::make_unique<Imaginary>();
}

auto ParseVariable(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>
{
    // Peek at the next token
    if (tokens.peek() != PALM_VARIABLE) {
        return std::unexpected { ParseError::UnexpectedToken };
    }

    // Consume 'var'
    tokens.pop();

    // Consume Variable Name
    std::string varName = tokens.pop();
    if (varName.empty() || varName == PALM_CLOSE_PARENS) {
        return std::unexpected { ParseError::IncompleteExpression };
    }

    return std::make_unique<Variable>(varName);
}

auto ParseAdd(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>
{
    return ParseBinaryOperationAssociativeAndCommutative<Add>(PALM_ADD, tokens);
}

auto ParseSubtract(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>
{
    return ParseBinaryOperationNonAssociativeAndCommutative<Subtract>(PALM_SUBTRACT, tokens);
}

auto ParseMultiply(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>
{
    return ParseBinaryOperationAssociativeAndCommutative<Multiply>(PALM_MULTIPLY, tokens);
}

auto ParseDivide(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>
{
    return ParseBinaryOperationNonAssociativeAndCommutative<Divide>(PALM_DIVIDE, tokens);
}

auto ParseExponent(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>
{
    return ParseBinaryOperationNonAssociativeAndCommutative<Exponent>(PALM_EXPONENT, tokens);
}

auto ParseLog(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>
{
    return ParseBinaryOperationNonAssociativeAndCommutative<Log>(PALM_LOG, tokens);
}

auto ParseIntegral(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>
{
    return ParseBinaryOperationAssociativeAndCommutative<Integral>(PALM_INTEGRAL, tokens);
}

auto ParseNegate(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>
{
    // Peek at the next token
    if (tokens.peek() != PALM_NEGATE) {
        return std::unexpected { ParseError::UnexpectedToken };
    }

    // Consume 'neg'
    tokens.pop();

    auto expr = ParseExpression(tokens);

    if (!expr) {
        return std::unexpected { expr.error() };
    }

    return std::make_unique<Negate<>>(std::move(**expr));
}

auto ParseDerivative(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>
{
    return ParseBinaryOperationNonAssociativeAndCommutative<Derivative>(PALM_DERIVATIVE, tokens);
}

auto ParsePi(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>
{
    if (!isPiToken(tokens.peek())) {
        return std::unexpected { ParseError::UnexpectedToken };
    }

    // Consume 'pi' | 'π'
    tokens.pop();
    return std::make_unique<Pi>();
}

auto ParseEulerNumber(TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>
{
    if (tokens.peek() != PALM_EULER) {
        return std::unexpected { ParseError::UnexpectedToken };
    }

    // Consume 'e'
    tokens.pop();
    return std::make_unique<EulerNumber>();
}

template <template <typename, typename> typename T>
    requires IAssociativeAndCommutative<T>
auto ParseBinaryOperationAssociativeAndCommutative(const std::string_view token, TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>
{
    // Peek at the next token
    if (tokens.peek() != token) {
        return std::unexpected { ParseError::UnexpectedToken };
    }

    // Consume operator
    tokens.pop();

    // Parse BinaryElements
    auto elements = ParseBinaryOperationElements(tokens);
    if (!elements) {
        return std::unexpected { elements.error() };
    }

    return Oasis::BuildFromVector<T>(*elements);
}

template <template <typename, typename> typename T>
    requires IExpression<T<Expression, Expression>> && (!IAssociativeAndCommutative<T>)
auto ParseBinaryOperationNonAssociativeAndCommutative(const std::string_view token, TokenStream& tokens) -> std::expected<std::unique_ptr<Expression>, ParseError>
{
    // Peek at the next token
    if (tokens.peek() != token) {
        return std::unexpected { ParseError::UnexpectedToken };
    }

    // Consume operator
    tokens.pop();

    // Parse BinaryElements
    auto elements = ParseBinaryOperationElements(tokens);
    if (!elements) {
        return std::unexpected { elements.error() };
    }

    // Loop over, building a right-heavy tree
    auto it = elements->rbegin();
    auto expr = std::move(*it);
    for (++it; it != elements->rend(); ++it) {
        expr = std::make_unique<T<Expression, Expression>>(std::move(**it), std::move(*expr));
    }

    // Tree built, return
    return expr;
}

auto ParseBinaryOperationElements(TokenStream& tokens) -> std::expected<std::vector<std::unique_ptr<Expression>>, ParseError>
{
    auto elements = ParseNullaryOperationElements(tokens);

    if (!elements || elements->size() < 2) {
        return std::unexpected { ParseError::IncompleteExpression };
    }

    return elements;
}

auto ParseUnaryOperationElements(TokenStream& tokens) -> std::expected<std::vector<std::unique_ptr<Expression>>, ParseError>
{
    auto elements = ParseNullaryOperationElements(tokens);

    if (!elements || elements->empty()) {
        return std::unexpected { ParseError::IncompleteExpression };
    }

    return elements;
}

auto ParseNullaryOperationElements(TokenStream& tokens) -> std::expected<std::vector<std::unique_ptr<Expression>>, ParseError>
{
    std::vector<std::unique_ptr<Expression>> elements;

    while (true) {
        // Peek at the next token
        std::string token = tokens.peek();

        // If we reach a closing parenthesis or end of stream, stop parsing elements
        if (token == PALM_CLOSE_PARENS || tokens.eof()) {
            break;
        }

        // Parse the next expression
        auto expr = ParseExpression(tokens);
        if (!expr) {
            break; // Stop if we can't parse an expression
        }

        elements.push_back(std::move(*expr));
    }

    return elements;
}
}
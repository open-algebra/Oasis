//
// Created by codin on 10/31/25.
//

#ifndef OASIS_PALMCONST_HPP
#define OASIS_PALMCONST_HPP

#include <array>
#include <string_view>
#include <unordered_map>

#include <boost/assign/list_of.hpp>
#include <boost/bimap/bimap.hpp>
#include <boost/bimap/unordered_multiset_of.hpp>
#include <boost/bimap/unordered_set_of.hpp>

#include "Oasis/Expression.hpp"


namespace Oasis {

/** Options for PALM serialization. */
struct PALMOpts {
    enum class ImgSymType {
        I,
        J
    } imaginarySymbol
        = ImgSymType::I;

    uint8_t numPlaces = 5;

    enum class Separator {
        SPACE
    } separator
        = Separator::SPACE;
};

/** Operators are taken from ExpressionType enum in Expression.hpp
 * @see ExpressionType
 */

/** Types of delimiters used in PALM serialization. */
enum PALMDelimiterType {
    START_EXPRESSION,
    END_EXPRESSION,
    TOKEN_SEPARATOR,
    EXPRESSION_PADDING
};

/** Bi-directional mapping between PALM operator tokens and their corresponding ExpressionType, PALMDelimiterType, or variant type. */
using PALMTokenBimap = boost::bimaps::bimap<
    boost::bimaps::unordered_multiset_of<std::string_view>,
    boost::bimaps::unordered_set_of<std::variant<
        ExpressionType,
        PALMDelimiterType,
        PALMOpts::ImgSymType
>>>;

/** A mapping from PALM tokens to con */
const PALMTokenBimap tokenBimap = ::boost::assign::list_of<PALMTokenBimap::relation>
    // Operators
    ("real", ExpressionType::Real)
    ("i", ExpressionType::Imaginary)
    ("i", PALMOpts::ImgSymType::I)
    ("j", ExpressionType::Imaginary)
    ("j", PALMOpts::ImgSymType::J)
    ("var", ExpressionType::Variable)
    ("+", ExpressionType::Add)
    ("-", ExpressionType::Subtract)
    ("*", ExpressionType::Multiply)
    ("/", ExpressionType::Divide)
    ("^", ExpressionType::Exponent)
    ("log", ExpressionType::Log)
    ("int", ExpressionType::Integral)
    ("d", ExpressionType::Derivative)
    ("neg", ExpressionType::Negate)
    ("matrix", ExpressionType::Matrix)
    ("pi",  ExpressionType::Pi)
    ("e", ExpressionType::EulerNumber)
    ("magnitude", ExpressionType::Magnitude)
    // Delimiters
    ("(", START_EXPRESSION)
    (")", END_EXPRESSION)
    (" ", TOKEN_SEPARATOR)
    (" ", EXPRESSION_PADDING);


/** Convert an ExpressionType to its corresponding PALM token.
 *
 * @param type The ExpressionType to convert.
 * @param options The PALM options to consider during conversion.
 * @return The corresponding PALM token as a string view.
 */
auto PALMExpressionToToken(ExpressionType type, const PALMOpts& options) -> std::string_view;

/** Convert a PALMDelimiterType to its corresponding PALM token.
 *
 * @param type The PALMDelimiterType to convert.
 * @param options The PALM options to consider during conversion.
 * @return The corresponding PALM token as a string view.
 */
auto PALMDelimiterToToken(PALMDelimiterType type, const PALMOpts& options) -> std::string_view;

// Define PALM tokens
constexpr inline std::string_view PALM_OPEN_PARENS = "(";
constexpr inline std::string_view PALM_CLOSE_PARENS = ")";

constexpr inline std::string_view PALM_REAL = "real";
constexpr inline std::array<std::string_view, 2> PALM_IMAGINARY = { "i", "j" };
constexpr inline std::string_view PALM_VARIABLE = "var";
constexpr inline std::string_view PALM_ADD = "+";
constexpr inline std::string_view PALM_SUBTRACT = "-";
constexpr inline std::string_view PALM_MULTIPLY = "*";
constexpr inline std::string_view PALM_DIVIDE = "/";
constexpr inline std::string_view PALM_EXPONENT = "^";
constexpr inline std::string_view PALM_LOG = "log";
constexpr inline std::string_view PALM_INTEGRAL = "int";
constexpr inline std::string_view PALM_NEGATE = "neg";
constexpr inline std::string_view PALM_DERIVATIVE = "d";
constexpr std::string_view PALM_MATRIX = "matrix";
constexpr inline std::array<std::string_view, 1> PALM_PI = { "pi" };
constexpr inline std::string_view PALM_EULER = "e";
}
#endif // OASIS_PALMCONST_HPP
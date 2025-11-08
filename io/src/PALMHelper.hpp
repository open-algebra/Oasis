//
// Created by codin on 10/31/25.
//

#ifndef OASIS_PALMCONST_HPP
#define OASIS_PALMCONST_HPP

#include <string_view>
#include <array>
#include <unordered_map>

#include <boost/bimap/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/bimap/unordered_multiset_of.hpp>

#include "Oasis/Expression.hpp"


namespace Oasis {

using TokenBimap = boost::bimaps::bimap<
    boost::bimaps::unordered_multiset_of<std::string_view>,
    boost::bimaps::unordered_set_of<ExpressionType>
>;

/** Options for PALM serialization. */
struct PALMOpts {
    enum class ImgSym {
        I,
        J
    } imaginarySymbol
        = ImgSym::I;

    uint8_t numPlaces = 5;

    enum class Separator {
        SPACE
    } separator
        = Separator::SPACE;
};

/** Types of delimiters used in PALM serialization. */
enum PALMDelimiterType {
    START_EXPRESSION,
    END_EXPRESSION,
    SEPARATOR
};

/** Mapping from PalmDelimiterType to PALM token strings. */
static const std::unordered_map<PALMDelimiterType, std::variant<
    std::string_view
>> palmDelimiterToTokenMap = {
    { START_EXPRESSION, "(" },
    { END_EXPRESSION, ")" },
    { SEPARATOR, " " }
};

/** Operators are taken from ExpressionType enum in Expression.hpp
 * @see ExpressionType
 */

/** Mapping from ExpressionType to PALM token strings. */
static const std::unordered_map<ExpressionType, std::variant<
    std::string_view,
    std::unordered_map<PALMOpts::ImgSym, std::string_view>
>> expressionTypeToPALMTokenMap = {
    { ExpressionType::Real, "real" },
    { ExpressionType::Imaginary, std::unordered_map<PALMOpts::ImgSym, std::string_view>{
        {
            { PALMOpts::ImgSym::I, "i" },
            { PALMOpts::ImgSym::J, "j" }
        }
    } },
    { ExpressionType::Variable, "var" },
    { ExpressionType::Add, "+" },
    { ExpressionType::Subtract, "-" },
    { ExpressionType::Multiply, "*" },
    { ExpressionType::Divide, "/" },
    { ExpressionType::Exponent, "^" },
    { ExpressionType::Log, "log" },
    { ExpressionType::Integral, "int" },
    { ExpressionType::Derivative, "d" },
    { ExpressionType::Negate, "neg" },
    { ExpressionType::Matrix, "matrix" },
    { ExpressionType::Pi, "pi" },
    { ExpressionType::EulerNumber, "e" },
    { ExpressionType::Magnitude, "magnitude" }
};

/** Convert an ExpressionType to its corresponding PALM token.
 *
 * @param type The ExpressionType to convert.
 * @param options The PALM options to consider during conversion.
 * @return The corresponding PALM token as a string view.
 */
auto PALMExpressionToToken(ExpressionType type, const PALMOpts& options) -> std::string_view;

auto PALMDelimiterToToken(PALMDelimiterType type, const PALMOpts& options) -> std::string_view;

struct PALMDelimiters {
    constexpr static  std::string_view START_EXPRESSION = "(";
    constexpr static  std::string_view END_EXPRESSION = ")";
};

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
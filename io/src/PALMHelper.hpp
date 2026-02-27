//
// Created by codin on 10/31/25.
//

#ifndef OASIS_PALMCONST_HPP
#define OASIS_PALMCONST_HPP

#include <array>
#include <string_view>


namespace Oasis {

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
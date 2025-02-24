//
// Created by Matthew McCall on 4/21/24.
//

#ifndef FROMSTRING_HPP
#define FROMSTRING_HPP

#include <expected>
#include <memory>

#include "Oasis/Expression.hpp"

namespace Oasis {

enum class ParseImaginaryOption {
    UseI,
    UseJ
};

auto PreProcessInFix(const std::string& str) -> std::string;

using FromInFixResult = std::expected<std::unique_ptr<Expression>, std::string>;
auto FromInFix(const std::string& str, ParseImaginaryOption option = ParseImaginaryOption::UseI) -> FromInFixResult;

}

#endif // FROMSTRING_HPP

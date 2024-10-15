//
// Created by Matthew McCall on 4/21/24.
//

#ifndef FROMSTRING_HPP
#define FROMSTRING_HPP

#include <memory>
#include <variant>

#include "Oasis/Expression.hpp"

namespace Oasis {

enum class ParseImaginaryOption {
    UseI,
    UseJ
};

class ParseResult {
public:
    explicit ParseResult(std::unique_ptr<Expression> expr);
    explicit ParseResult(std::string err);

    [[nodiscard]] bool Ok() const;
    [[nodiscard]] const Expression& GetResult() const;
    [[nodiscard]] std::string GetErrorMessage() const;

private:
    std::variant<std::unique_ptr<Expression>, std::string> result;
};

auto PreProcessInFix(const std::string& str) -> std::string;

auto FromInFix(const std::string& str, ParseImaginaryOption option = ParseImaginaryOption::UseI) -> ParseResult;

}

#endif // FROMSTRING_HPP

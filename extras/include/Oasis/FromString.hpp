//
// Created by Matthew McCall on 4/21/24.
//

#ifndef FROMSTRING_HPP
#define FROMSTRING_HPP

namespace Oasis {

class ParseResult {
public:
    explicit ParseResult(std::unique_ptr<Expression> expr);
    explicit ParseResult(std::string err);

    [[nodiscard]] bool Ok() const;
    [[nodiscard]] const Expression& GetResult() const;
    std::string GetErrorMessage() const;

private:
    std::variant<std::unique_ptr<Expression>, std::string> result;
};

auto FromInFix(const std::string& str) -> ParseResult;

}

#endif // FROMSTRING_HPP

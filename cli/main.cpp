//
// Created by Matthew McCall on 2/19/25.
//

#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <ranges>

#include "Oasis/FromString.hpp"
#include "Oasis/InFixSerializer.hpp"

#include <boost/callable_traits/return_type.hpp>
#include <fmt/color.h>
#include <isocline.h>

template <typename FnT>
auto operator|(const std::string& str, FnT fn) -> boost::callable_traits::return_type_t<FnT>
{
    return fn(str);
}

//https://en.cppreference.com/w/cpp/string/byte/isspace#Notes
bool safe_isspace(const unsigned char ch) { return std::isspace(ch); }

auto trim_whitespace(const std::string& str) -> std::string
{
    return str
        | std::views::drop_while(safe_isspace)
        | std::views::reverse
        | std::views::drop_while(safe_isspace)
        | std::views::reverse
        | std::ranges::to<std::string>();
}

// Calling Oasis::FromInFix passed as template fails because defaulted parameters aren't represented in the type, so a wrapper is needed
auto Parse(const std::string& in) -> Oasis::FromInFixResult { return Oasis::FromInFix(in); };

int main(int argc, char** argv)
{
    Oasis::InFixSerializer serializer;
    constexpr auto err_style = fg(fmt::color::indian_red);
    constexpr auto success_style = fg(fmt::color::green);

    const char* line;
    while ((line = ic_readline(nullptr)) != nullptr) {
        std::string input { line };
        delete[] line;

        input = input | trim_whitespace;

        if (input.empty())  continue;
        if (input == "exit") break;

        // Calling Oasis::FromInFix passed as template fails because defaulted parameters aren't represented in the type, so a wrapper is needed
        auto result = (input | Oasis::PreProcessInFix | Parse)
            .transform([](const std::unique_ptr<Oasis::Expression>& expr) -> std::unique_ptr<Oasis::Expression> {
                return expr->Simplify();
            })
            .and_then([&serializer](const std::unique_ptr<Oasis::Expression>& expr) -> std::expected<std::string, std::string> {
                return expr->Accept(serializer);
            });

        fmt::println("  {}", fmt::styled(result.value_or(result.error()), result.has_value() ? success_style : err_style));
        std::fflush(stdout);
    }

    return EXIT_SUCCESS;
}
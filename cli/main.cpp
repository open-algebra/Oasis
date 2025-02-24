//
// Created by Matthew McCall on 2/19/25.
//

#include <cstdlib>
#include <cstdio>

#include "Oasis/FromString.hpp"
#include "Oasis/InFixSerializer.hpp"

#include <boost/callable_traits/return_type.hpp>
#include <fmt/color.h>
#include <linenoise.h>

template <typename FnT>
auto operator|(const std::string& str, FnT fn) -> boost::callable_traits::return_type_t<FnT>
{
    return fn(str);
}

int main(int argc, char** argv)
{
    linenoiseHistorySetMaxLen(16);

    Oasis::InFixSerializer serializer;
    constexpr auto err_style = fg(fmt::color::indian_red);
    constexpr auto success_style = fg(fmt::color::green);

    const char* line;
    while ((line = linenoise("> ")) != nullptr) {
        std::string input{ line };
        delete[] line;
        if (input.empty())  continue;

        linenoiseHistoryAdd(input.c_str());

        // Calling Oasis::FromInFix passed as template fails because defaulted parameters aren't represented in the type, so a wrapper is needed
        auto Parse = [](const std::string& in) { return Oasis::FromInFix(in); };
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
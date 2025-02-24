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
    auto err_style = fg(fmt::color::indian_red);

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

        if (result.has_value()) fmt::println("  {}", result.value());
        else fmt::println("  {}", styled(result.error(), err_style));

        std::fflush(stdout);
    }

    return EXIT_SUCCESS;
}
//
// Created by Matthew McCall on 2/19/25.
//

#include <cstdlib>
#include <cstdio>

#include "Oasis/FromString.hpp"
#include "Oasis/InFixSerializer.hpp"

#include <fmt/color.h>
#include <linenoise.h>

int main(int argc, char **argv) {
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    linenoiseHistorySetMaxLen(16);

    Oasis::InFixSerializer serializer;

    const char *line;
    while((line = linenoise("> ")) != nullptr) {
        std::string input { line };
        delete[] line;

        if (input.empty()) {
            continue;
        }

        linenoiseHistoryAdd(input.c_str());

        auto parseResult = Oasis::FromInFix(Oasis::PreProcessInFix(input));

        Oasis::FromInFix(Oasis::PreProcessInFix(input))
        .and_then([&serializer](const std::unique_ptr<Oasis::Expression>& expr) -> std::expected<std::string, std::string> {
            return expr->Accept(serializer);
        })
        .transform([&](const std::string& str) -> void {
            fmt::println("  {}", str);
        })
        .transform_error([&](const std::string& err) -> std::string {
            static auto err_style = fg(fmt::color::indian_red);
            print(err_style, "  {}\n", err);
            return err;
        });
    }

    return EXIT_SUCCESS;
}
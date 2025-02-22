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
    linenoiseHistorySetMaxLen(16);

    Oasis::InFixSerializer serializer;

    const char *line;
    while((line = linenoise("> ")) != nullptr) {
        std::string input { line };
        delete[] line;

        linenoiseHistoryAdd(input.c_str());

        auto parseResult = Oasis::FromInFix(Oasis::PreProcessInFix(input));
        auto err_style = fg(fmt::color::indian_red);
        if (!parseResult) {
            print(err_style, "  Failed to parse: {}\n", parseResult.error());
            continue;
        }

        const auto simplifyResult = parseResult.value()->Simplify();
        if (!simplifyResult) {
            print(err_style, "  Failed to simplify\n");
            continue;
        }

        auto serializeResult = simplifyResult->Accept(serializer);
        if (!serializeResult) {
            print(err_style, "  Failed to serialize: {}\n", serializeResult.error());
            continue;
        }

        fmt::println("  {}", serializeResult.value());
    }

    return EXIT_SUCCESS;
}
//
// Created by codin on 11/8/25.
//

#include "PALMHelper.hpp"

namespace Oasis {

auto PALMExpressionToToken(const ExpressionType type, const PALMOpts& options) -> std::string_view
{
    // Find the type in the map
    auto it = expressionTypeToPALMTokenMap.find(type);
    if (it == expressionTypeToPALMTokenMap.end()) { // Not found
        return "";
    }

    // If the value is a string_view (one option), return it directly
    if (std::holds_alternative<std::string_view>(it->second)) {
        return std::get<std::string_view>(it->second);
    }

    // Check for variants
    if (std::holds_alternative<std::unordered_map<PALMOpts::ImgSym, std::string_view>>(it->second)) { // Imaginary symbol
        const auto& symMap = std::get<std::unordered_map<PALMOpts::ImgSym, std::string_view>>(it->second);
        auto symIt = symMap.find(options.imaginarySymbol);

        // Return the corresponding symbol if found
        if (symIt != symMap.end()) {
            return symIt->second;
        }

        // Default to 'i' if not found
        return symMap.at(PALMOpts::ImgSym::I);
    }

    // Fallback
    return "";
}

auto PALMDelimiterToToken(const PALMDelimiterType type, const PALMOpts& options) -> std::string_view
{
    // Find the type in the map
    auto it = palmDelimiterToTokenMap.find(type);
    if (it == palmDelimiterToTokenMap.end()) { // Not found
        return "";
    }

    // Return the corresponding token
    return std::get<std::string_view>(it->second);
}

}

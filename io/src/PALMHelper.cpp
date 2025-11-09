//
// Created by codin on 11/8/25.
//

#include "PALMHelper.hpp"

namespace Oasis {

auto PALMExpressionToToken(const ExpressionType type, const PALMOpts& options) -> std::string_view
{
    // Determine if there are multiple tokens for the type
    switch (type) {
    case ExpressionType::Imaginary:
        return tokenBimap.right.at(options.imaginarySymbol);
    default:
        return tokenBimap.right.at(type);
    }
}

auto PALMDelimiterToToken(const PALMDelimiterType type, const PALMOpts& /*options*/) -> std::string_view
{
   return tokenBimap.right.at(type);
}

}

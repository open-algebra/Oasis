//
// Created by Matthew McCall on 4/22/24.
//

#ifndef UTIL_HPP
#define UTIL_HPP

#include <utility>
#include <tinyxml2.h>

namespace Oasis::mml {

tinyxml2::XMLElement* CreatePlaceholder(tinyxml2::XMLDocument& doc);

template <typename T>
auto GetOpsAsMathMLPair(const T& binexp, tinyxml2::XMLDocument& doc) -> std::pair<tinyxml2::XMLElement*, tinyxml2::XMLElement*>
{
    tinyxml2::XMLElement* mostSig = binexp.HasMostSigOp() ? binexp.GetMostSigOp().ToMathMLElement(doc) : CreatePlaceholder(doc);
    tinyxml2::XMLElement* leastSig = binexp.HasLeastSigOp() ? binexp.GetLeastSigOp().ToMathMLElement(doc) : CreatePlaceholder(doc);

    return { mostSig, leastSig };
}

}

#endif //UTIL_HPP

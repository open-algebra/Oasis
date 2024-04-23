//
// Created by Matthew McCall on 4/22/24.
//

#include "Util.hpp"

namespace Oasis::mml {

tinyxml2::XMLElement* CreatePlaceholder(tinyxml2::XMLDocument& doc)
{
    tinyxml2::XMLElement* mspace = doc.NewElement("mspace");
    mspace->SetAttribute("style", "border: dashed gray 1.5pt;");
    mspace->SetAttribute("width", "1em");
    mspace->SetAttribute("height", "1em");
    return mspace;
}

}
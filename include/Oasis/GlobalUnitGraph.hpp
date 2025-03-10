//
// Created by Jary Chen on 3/10/25.
//

#ifndef GLOBALUNITGRAPH_HPP
#define GLOBALUNITGRAPH_HPP
#include "UnitGraph.hpp"

namespace Oasis {
    // Reference to singleton UnitGraph
    UnitGraph& getGlobalUnitGraph();
}

#endif //GLOBALUNITGRAPH_HPP

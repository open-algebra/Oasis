//
// Created by Jary Chen on 3/10/25.
//

#include "../include/Oasis/GlobalUnitGraph.hpp"

namespace Oasis {

UnitGraph& getGlobalUnitGraph() {
    static UnitGraph globalGraph;
    static bool initialized = false;
    if (!initialized) {
        initialized = true;

        // Mapping Here
        globalGraph.addConversion(Unit::Meter, Unit::Kilometer, 0.001);
    }
    return globalGraph;
}

} // namespace Oasis

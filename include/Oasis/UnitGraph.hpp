//
// Created by Jary Chen on 3/4/25.
//

#ifndef OASIS_UNIT_GRAPH_HPP
#define OASIS_UNIT_GRAPH_HPP

#include "Unit.hpp"
#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <optional>
#include <unordered_map>
#include <boost/graph/graph_traits.hpp>
#include <queue>

namespace Oasis {

struct EdgeProperties {
    double factor;
};

using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS,
                                    boost::no_property, EdgeProperties>;

class UnitGraph {
public:
    UnitGraph();

    void addConversion(Unit from, Unit to, double factor);

    std::optional<double> findConversionFactor(Unit from, Unit to) const;

private:
    Graph graph_;
    std::unordered_map<Unit, int> unitToVertex_;
    int nextVertex_;

    int getOrAddVertex(Unit unit);
};

} // namespace Oasis

#endif // OASIS_UNIT_GRAPH_HPP


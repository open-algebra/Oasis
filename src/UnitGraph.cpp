//
// Created by Jary Chen on 3/4/25.
//

#include "../include/Oasis/UnitGraph.hpp"

namespace Oasis {

UnitGraph::UnitGraph() : nextVertex_(0) {}

int UnitGraph::getOrAddVertex(Unit unit) {
    auto it = unitToVertex_.find(unit);
    if (it != unitToVertex_.end())
        return it->second;
    int vertex = nextVertex_;
    unitToVertex_[unit] = vertex;
    ++nextVertex_;

    // Check if the graph has enough vertices
    if (boost::num_vertices(graph_) <= vertex)
        boost::add_vertex(graph_);
    return vertex;
}

void UnitGraph::addConversion(Unit from, Unit to, double factor) {
    //Build Adj Edges
    int vFrom = getOrAddVertex(from);
    int vTo = getOrAddVertex(to);
    boost::add_edge(vFrom, vTo, EdgeProperties{factor}, graph_);
    boost::add_edge(vTo, vFrom, EdgeProperties{1.0 / factor}, graph_);
}

std::optional<double> UnitGraph::findConversionFactor(Unit from, Unit to) const {
    if (from == to)
        return 1.0;
    auto itFrom = unitToVertex_.find(from);
    auto itTo = unitToVertex_.find(to);
    if (itFrom == unitToVertex_.end() || itTo == unitToVertex_.end())
        return std::nullopt;

    int start = itFrom->second;
    int target = itTo->second;

    //Standard BFS
    std::queue<int> queue;
    std::unordered_map<int, double> cumulative;
    cumulative[start] = 1.0;
    queue.push(start);

    while (!queue.empty()) {
        int current = queue.front();
        queue.pop();

        if (current == target)
            return cumulative[current];

        auto edges = boost::out_edges(current, graph_);
        for (auto edgeIter = edges.first; edgeIter != edges.second; ++edgeIter) {
            int neighbor = boost::target(*edgeIter, graph_);
            double factor = graph_[*edgeIter].factor;
            double newFactor = cumulative[current] * factor;

            //cycle detection
            if (cumulative.find(neighbor) == cumulative.end()) {
                cumulative[neighbor] = newFactor;
                queue.push(neighbor);
            }
        }
    }
    return std::nullopt;
}

} // namespace Oasis

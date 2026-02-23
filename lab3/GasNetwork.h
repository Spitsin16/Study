#ifndef GASNETWORK_H
#define GASNETWORK_H

#include "PipeManager.h"
#include "CompressorStationManager.h"
#include <unordered_map>
#include <vector>
#include <string>

// Edge in the gas transport network: pipe connects two compressor stations
struct NetworkEdge {
    int pipeId;
    int fromKS;
    int toKS;
};

class GasNetwork {
    std::vector<NetworkEdge> edges;

    int chooseDiameter() const;
    int findFreePipe(const PipeManager& pm, double diameter) const;

public:
    void connectKS(PipeManager& pm, const CompressorStationManager& sm);
    void removeConnection(PipeManager& pm);
    void displayNetwork(const PipeManager& pm, const CompressorStationManager& sm) const;
    void topologicalSort(const CompressorStationManager& sm) const;
    void manageNetwork(PipeManager& pm, CompressorStationManager& sm);

    // build adjacency list from current pipe states
    std::unordered_map<int, std::vector<int>> buildAdjacencyList(const PipeManager& pm) const;

    // save/load
    std::string saveEdge(const NetworkEdge& e) const;
    void addEdgeFromLoad(const NetworkEdge& e);
    const std::vector<NetworkEdge>& getEdges() const { return edges; }
    void clear() { edges.clear(); }
};

#endif

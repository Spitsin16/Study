#include "GasNetwork.h"
#include "utils.h"
#include <iostream>
#include <queue>
#include <algorithm>
#include <set>

int GasNetwork::chooseDiameter() const {
    std::cout << "available diameters:\n"
              << "1. 500 mm\n"
              << "2. 700 mm\n"
              << "3. 1000 mm\n"
              << "4. 1400 mm\n";
    int choice = inputIntInRange("choose diameter: ", 1, 4);
    switch (choice) {
        case 1: return 500;
        case 2: return 700;
        case 3: return 1000;
        case 4: return 1400;
    }
    return 500;
}

int GasNetwork::findFreePipe(const PipeManager& pm, double diameter) const {
    for (const auto& pair : pm.getPipes()) {
        if (pair.second.isFree() && pair.second.getDiameter() == diameter) {
            return pair.first;
        }
    }
    return -1;
}

void GasNetwork::connectKS(PipeManager& pm, const CompressorStationManager& sm) {
    if (sm.getStations().size() < 2) {
        std::cout << "need at least 2 stations to create a connection.\n";
        return;
    }

    sm.displayStations(sm.getStations());

    int fromId = inputInt("enter input KS id: ");
    if (sm.getStations().find(fromId) == sm.getStations().end()) {
        std::cout << "station " << fromId << " not found.\n";
        return;
    }

    int toId = inputInt("enter output KS id: ");
    if (sm.getStations().find(toId) == sm.getStations().end()) {
        std::cout << "station " << toId << " not found.\n";
        return;
    }

    if (fromId == toId) {
        std::cout << "cannot connect station to itself.\n";
        return;
    }

    int diameter = chooseDiameter();
    log("connecting KS " + std::to_string(fromId) + " -> KS " + std::to_string(toId) +
        " with diameter " + std::to_string(diameter));

    // search for a free pipe with matching diameter
    int pipeId = findFreePipe(pm, diameter);

    if (pipeId != -1) {
        std::cout << "found free pipe id " << pipeId << " with diameter " << diameter << " mm.\n";
        log("using existing pipe id " + std::to_string(pipeId));
    } else {
        // create new pipe
        std::cout << "no free pipe with diameter " << diameter << " mm. creating new pipe.\n";
        int nextId = 1;
        while (pm.getPipes().count(nextId)) ++nextId;

        Pipe p;
        p.readForNetwork(nextId, diameter);
        pm.addPipeFromLoad(nextId, p);
        pipeId = nextId;
        log("created new pipe id " + std::to_string(pipeId) + " for network");
    }

    // connect pipe to stations
    {
        Pipe p = pm.getPipes().at(pipeId);
        p.connectToNetwork(fromId, toId);
        pm.addPipeFromLoad(pipeId, p);
    }

    NetworkEdge edge;
    edge.pipeId = pipeId;
    edge.fromKS = fromId;
    edge.toKS = toId;
    edges.push_back(edge);

    std::cout << "connected: KS " << fromId << " --[pipe " << pipeId << "]--> KS " << toId << "\n";
    log("network edge added: KS " + std::to_string(fromId) + " -> KS " + std::to_string(toId) +
        " via pipe " + std::to_string(pipeId));
}

void GasNetwork::removeConnection(PipeManager& pm) {
    if (edges.empty()) {
        std::cout << "no connections in network.\n";
        return;
    }

    // show current connections
    std::cout << "\ncurrent connections:\n";
    for (const auto& edge : edges) {
        std::cout << "  KS " << edge.fromKS << " --[pipe " << edge.pipeId << "]--> KS " << edge.toKS << "\n";
    }

    int pipeId = inputInt("enter pipe id to disconnect: ");

    auto it = std::find_if(edges.begin(), edges.end(),
        [pipeId](const NetworkEdge& e) { return e.pipeId == pipeId; });

    if (it == edges.end()) {
        std::cout << "pipe " << pipeId << " is not in the network.\n";
        return;
    }

    // disconnect pipe
    if (pm.getPipes().count(pipeId)) {
        Pipe p = pm.getPipes().at(pipeId);
        p.disconnectFromNetwork();
        pm.addPipeFromLoad(pipeId, p);
    }

    log("removed network edge: KS " + std::to_string(it->fromKS) + " -> KS " + std::to_string(it->toKS));
    edges.erase(it);
    std::cout << "connection removed.\n";
}

void GasNetwork::displayNetwork(const PipeManager& pm, const CompressorStationManager& sm) const {
    if (edges.empty()) {
        std::cout << "network is empty.\n";
        return;
    }
    std::cout << "\n=== gas transport network ===\n";
    for (const auto& edge : edges) {
        std::cout << "KS " << edge.fromKS << " --[pipe " << edge.pipeId
                  << ", d=" << (pm.getPipes().count(edge.pipeId) ?
                     std::to_string((int)pm.getPipes().at(edge.pipeId).getDiameter()) : "?")
                  << " mm]--> KS " << edge.toKS << "\n";
    }
    std::cout << "=============================\n";
}

std::unordered_map<int, std::vector<int>> GasNetwork::buildAdjacencyList(const PipeManager& pm) const {
    std::unordered_map<int, std::vector<int>> adj;
    for (const auto& edge : edges) {
        adj[edge.fromKS].push_back(edge.toKS);
        if (adj.find(edge.toKS) == adj.end()) {
            adj[edge.toKS] = {};
        }
    }
    return adj;
}

void GasNetwork::topologicalSort(const CompressorStationManager& sm) const {
    if (edges.empty()) {
        std::cout << "network is empty, nothing to sort.\n";
        return;
    }

    // collect all vertices
    std::set<int> vertices;
    std::unordered_map<int, std::vector<int>> adj;
    std::unordered_map<int, int> inDegree;

    for (const auto& edge : edges) {
        vertices.insert(edge.fromKS);
        vertices.insert(edge.toKS);
        adj[edge.fromKS].push_back(edge.toKS);
    }

    for (int v : vertices) {
        inDegree[v] = 0;
    }
    for (const auto& edge : edges) {
        inDegree[edge.toKS]++;
    }

    // Kahn's algorithm (BFS)
    std::queue<int> q;
    for (int v : vertices) {
        if (inDegree[v] == 0) {
            q.push(v);
        }
    }

    std::vector<int> result;
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        result.push_back(v);

        if (adj.find(v) != adj.end()) {
            for (int u : adj[v]) {
                inDegree[u]--;
                if (inDegree[u] == 0) {
                    q.push(u);
                }
            }
        }
    }

    if ((int)result.size() != (int)vertices.size()) {
        std::cout << "cycle detected in the network! topological sort is not possible.\n";
        log("topological sort failed: cycle detected");
        return;
    }

    std::cout << "\ntopological sort result:\n";
    for (int i = 0; i < (int)result.size(); i++) {
        int ksId = result[i];
        std::cout << "  " << (i + 1) << ". KS " << ksId;
        if (sm.getStations().count(ksId)) {
            std::cout << " (" << sm.getStations().at(ksId).getName() << ")";
        }
        std::cout << "\n";
    }
    log("topological sort completed: " + std::to_string(result.size()) + " stations sorted");
}

void GasNetwork::manageNetwork(PipeManager& pm, CompressorStationManager& sm) {
    while (true) {
        std::cout << "-----------------------------\n"
                  << "| gas transport network:    |\n"
                  << "| 1. connect KS             |\n"
                  << "| 2. view network           |\n"
                  << "| 3. topological sort       |\n"
                  << "| 4. remove connection      |\n"
                  << "| 0. back                   |\n"
                  << "-----------------------------\n";
        int choice = inputIntInRange("choose action: ", 0, 4);
        switch (choice) {
            case 1: connectKS(pm, sm); break;
            case 2: displayNetwork(pm, sm); break;
            case 3: topologicalSort(sm); break;
            case 4: removeConnection(pm); break;
            case 0: log("left network management menu"); return;
        }
    }
}

std::string GasNetwork::saveEdge(const NetworkEdge& e) const {
    return std::to_string(e.pipeId) + ";" +
           std::to_string(e.fromKS) + ";" +
           std::to_string(e.toKS);
}

void GasNetwork::addEdgeFromLoad(const NetworkEdge& e) {
    edges.push_back(e);
}

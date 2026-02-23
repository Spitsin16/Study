#ifndef NETWORK_H
#define NETWORK_H
#include "Pipe.h"
#include "CompressorStation.h"
#include <unordered_map>
#include <vector>

class Network {
public:
    static double pipeCap(const Pipe& p);
    static double pipeW(const Pipe& p);
    static double maxFlow(const std::unordered_map<int,Pipe>& pipes, int s, int t);
    static double shortPath(const std::unordered_map<int,Pipe>& pipes, int s, int t,
                            std::vector<int>& path);
    static void manage(const std::unordered_map<int,Pipe>& pipes,
                       const std::unordered_map<int,CompressorStation>& stations);
};

#endif
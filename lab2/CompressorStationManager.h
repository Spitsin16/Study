#ifndef COMPRESSORSTATIONMANAGER_H
#define COMPRESSORSTATIONMANAGER_H
#include "CompressorStation.h"
#include <unordered_map>
#include <string>

class CompressorStationManager {
    std::unordered_map<int, CompressorStation> stations;
    int nextId = 1;

    void toggleShop(CompressorStation& station);
    void addStation();
    void editStation();
    void removeStation();
    void displayAllStations() const;
    void searchStations();

public:
    void manageStations();
    void displayStations(const std::unordered_map<int, CompressorStation>& subset) const;
    void clear() { stations.clear(); nextId = 1; }

    const std::unordered_map<int, CompressorStation>& getStations() const { return stations; }
    void addStationFromLoad(int id, const CompressorStation& station);
    std::string saveCS(const CompressorStation& cs) const;
};

#endif
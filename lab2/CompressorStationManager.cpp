#include "CompressorStationManager.h"
#include "utils.h"
#include "filters.h"
#include <iostream>
#include <vector>
#include <algorithm>

static std::vector<int> sortedKeys(const std::unordered_map<int, CompressorStation>& m) {
    std::vector<int> keys;
    for (const auto& pair : m) keys.push_back(pair.first);
    std::sort(keys.begin(), keys.end());
    return keys;
}

void CompressorStationManager::toggleShop(CompressorStation& station) {
    std::cout << "1. start shop\n2. stop shop\n";
    int choice = inputIntInRange("choose: ", 1, 2);
    if (choice == 1) station.startShop();
    else             station.stopShop();
}

void CompressorStationManager::addStation() {
    while (stations.count(nextId)) ++nextId;
    CompressorStation cs;
    cs.read(nextId);
    stations[nextId] = cs;
    log("station added with id: " + std::to_string(nextId));
    std::cout << "station added, id: " << nextId << "\n";
    ++nextId;
}

void CompressorStationManager::displayStations(
    const std::unordered_map<int, CompressorStation>& subset) const
{
    if (subset.empty()) { std::cout << "no stations found.\n"; return; }
    std::vector<int> keys;
    for (const auto& pair : subset) keys.push_back(pair.first);
    std::sort(keys.begin(), keys.end());
    for (int id : keys) {
        std::cout << "id " << id;
        subset.at(id).display();
    }
}

void CompressorStationManager::displayAllStations() const {
    if (stations.empty()) {
        std::cout << "no stations.\n";
    } else {
        std::cout << "\nall stations:\n";
        for (int id : sortedKeys(stations)) {
            std::cout << "id " << id;
            stations.at(id).display();
        }
    }
    log("user viewed all stations (" + std::to_string(stations.size()) + " total)");
}

void CompressorStationManager::editStation() {
    if (stations.empty()) { std::cout << "no stations.\n"; return; }
    displayAllStations();
    std::cout << "enter station id to edit: ";
    std::string input;
    std::getline(std::cin, input);
    long id = numberOrDefault(input, -1);
    log("user requested edit station id: " + input);
    std::unordered_map<int, CompressorStation>::iterator it = stations.find(static_cast<int>(id));
    if (it != stations.end()) {
        toggleShop(it->second);
    } else {
        log("edit station: id " + input + " not found");
        std::cout << "station not found.\n";
    }
}

void CompressorStationManager::removeStation() {
    if (stations.empty()) { std::cout << "no stations.\n"; return; }
    displayAllStations();
    std::cout << "enter station id to delete: ";
    std::string input;
    std::getline(std::cin, input);
    long id = numberOrDefault(input, -1);
    log("user requested delete station id: " + input);
    if (stations.erase(static_cast<int>(id))) {
        log("station id " + std::to_string(id) + " deleted");
        std::cout << "station id " << id << " deleted.\n";
    } else {
        log("delete station: id " + input + " not found");
        std::cout << "station not found.\n";
    }
}

void CompressorStationManager::searchStations() {
    if (stations.empty()) { std::cout << "no stations.\n"; return; }
    std::cout << "1. by name\n2. by unused shops %\n0. back\n";
    int opt = inputIntInRange("choose filter: ", 0, 2);
    if (opt == 0) return;

    std::unordered_map<int, CompressorStation> found;
    if (opt == 1) {
        std::string name;
        std::cout << "enter name substring: ";
        INPUT_LINE(std::cin, name);
        log("search stations by name: \"" + name + "\"");
        found = find_by_filter(stations, filters::by_name<CompressorStation>, name);
    } else {
        double minPct = inputDouble("enter min unused %: ");
        log("search stations by unused% >= " + std::to_string(minPct));
        found = find_by_filter(stations, filters::station_by_unused, minPct);
    }
    log("search result: " + std::to_string(found.size()) + " station(s) found");
    displayStations(found);
}

void CompressorStationManager::manageStations() {
    while (true) {
        std::cout << "-----------------------------\n"
                  << "| station management:       |\n"
                  << "| 1. add                    |\n"
                  << "| 2. edit                   |\n"
                  << "| 3. delete                 |\n"
                  << "| 4. view all               |\n"
                  << "| 5. search                 |\n"
                  << "| 0. back                   |\n"
                  << "-----------------------------\n";
        int choice = inputIntInRange("choose action: ", 0, 5);
        switch (choice) {
            case 1: addStation(); break;
            case 2: editStation(); break;
            case 3: removeStation(); break;
            case 4: displayAllStations(); break;
            case 5: searchStations(); break;
            case 0: log("left station management menu"); return;
        }
    }
}

void CompressorStationManager::addStationFromLoad(int id, const CompressorStation& station) {
    CompressorStation cs = station;
    cs.setId(id);
    stations[id] = cs;
    if (id >= nextId) nextId = id + 1;
    log("station loaded with id: " + std::to_string(id));
}

std::string CompressorStationManager::saveCS(const CompressorStation& cs) const {
    return cs.getInfo();
}
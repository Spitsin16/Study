#include "CompressorStation.h"
#include "utils.h"
#include <iostream>

static void recalc(int total, int operational, double& unused, double& eff) {
    unused = total > 0 ? (double)(total - operational) / total * 100.0 : 0.0;
    eff    = 100.0 - unused;
}

void CompressorStation::startShop() {
    if (operationalShops < totalShops) {
        ++operationalShops;
        recalc(totalShops, operationalShops, unusedPercent, efficiency);
        log("station id " + std::to_string(id) +
            " shop started, operational now: " + std::to_string(operationalShops));
        std::cout << "shop started. operational: " << operationalShops << "\n";
    } else {
        log("station id " + std::to_string(id) + " start shop: all already running");
        std::cout << "all shops already running.\n";
    }
}

void CompressorStation::stopShop() {
    if (operationalShops > 0) {
        --operationalShops;
        recalc(totalShops, operationalShops, unusedPercent, efficiency);
        log("station id " + std::to_string(id) +
            " shop stopped, operational now: " + std::to_string(operationalShops));
        std::cout << "shop stopped. operational: " << operationalShops << "\n";
    } else {
        log("station id " + std::to_string(id) + " stop shop: none running");
        std::cout << "no shops to stop.\n";
    }
}

std::string CompressorStation::getInfo() const {
    return full_name + ";" +
           std::to_string(totalShops) + ";" +
           std::to_string(operationalShops) + ";" +
           std::to_string(unusedPercent) + ";" +
           std::to_string(efficiency);
}

void CompressorStation::read(int station_id) {
    id = station_id;
    std::cout << "enter station name: ";
    std::getline(std::cin, full_name);
    log("station name entered: " + full_name);
    totalShops = readPositive<int>("enter total shops: ", "error: must be positive.", 10000);
    log("total shops entered: " + std::to_string(totalShops));
    operationalShops = inputIntInRange(
        "enter operational shops (0 to " + std::to_string(totalShops) + "): ", 0, totalShops);
    log("operational shops entered: " + std::to_string(operationalShops));
    recalc(totalShops, operationalShops, unusedPercent, efficiency);
}

void CompressorStation::display() const {
    std::cout << " station: " << full_name
              << ", total: " << totalShops
              << ", operational: " << operationalShops
              << ", unused: " << unusedPercent << "%"
              << ", efficiency: " << efficiency << "%\n";
}

void CompressorStation::load(const std::string& data) {
    if (data.empty()) return;
    size_t p1 = data.find(';');
    size_t p2 = data.find(';', p1 + 1);
    size_t p3 = data.find(';', p2 + 1);
    size_t p4 = data.find(';', p3 + 1);
    if (p4 == std::string::npos) return;
    try {
        full_name        = data.substr(0, p1);
        totalShops       = std::stoi(data.substr(p1 + 1, p2 - p1 - 1));
        operationalShops = std::stoi(data.substr(p2 + 1, p3 - p2 - 1));
        unusedPercent    = std::stod(data.substr(p3 + 1, p4 - p3 - 1));
        efficiency       = std::stod(data.substr(p4 + 1));
    } catch (...) {
        log("error parsing station data: " + data);
    }
}
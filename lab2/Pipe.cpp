#include "Pipe.h"
#include "utils.h"
#include <iostream>

void Pipe::toggleRepair() {
    underRepair = !underRepair;
}

std::string Pipe::getInfo() const {
    return full_name + ";" +
           std::to_string(length) + ";" +
           std::to_string(diameter) + ";" +
           (underRepair ? "Yes" : "No") + ";";
}

void Pipe::read(int pipe_id) {
    id = pipe_id;
    std::cout << "enter pipe name: ";
    std::getline(std::cin, full_name);
    log("pipe name entered: " + full_name);
    length   = readPositive<double>("enter pipe length (km): ",   "error: must be positive.", 1e6);
    diameter = readPositive<double>("enter pipe diameter (mm): ", "error: must be positive.", 1e6);
    underRepair = false;
}

void Pipe::display() const {
    std::cout << " pipe: " << full_name
              << ", length: " << length << " km"
              << ", diameter: " << diameter << " mm"
              << ", repair: " << (underRepair ? "yes" : "no")
              << "\n";
}

void Pipe::load(const std::string& data) {
    if (data.empty()) return;
    size_t p1 = data.find(';');
    size_t p2 = data.find(';', p1 + 1);
    size_t p3 = data.find(';', p2 + 1);
    if (p1 == std::string::npos || p2 == std::string::npos || p3 == std::string::npos) return;
    try {
        full_name   = data.substr(0, p1);
        length      = std::stod(data.substr(p1 + 1, p2 - p1 - 1));
        diameter    = std::stod(data.substr(p2 + 1, p3 - p2 - 1));
        std::string rep = data.substr(p3 + 1);
        underRepair = (rep == "Yes" || rep == "1");
    } catch (...) {
        log("error parsing pipe data: " + data);
    }
}
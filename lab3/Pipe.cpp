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
           (underRepair ? "Yes" : "No") + ";" +
           std::to_string(fromKS) + ";" +
           std::to_string(toKS) + ";";
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

void Pipe::readForNetwork(int pipe_id, double diam) {
    id = pipe_id;
    full_name = "auto_pipe_" + std::to_string(pipe_id);
    length = readPositive<double>("enter pipe length (km): ", "error: must be positive.", 1e6);
    diameter = diam;
    underRepair = false;
    log("pipe created for network: id=" + std::to_string(pipe_id) + " diameter=" + std::to_string(diam));
}

void Pipe::display() const {
    std::cout << " pipe: " << full_name
              << ", length: " << length << " km"
              << ", diameter: " << diameter << " mm"
              << ", repair: " << (underRepair ? "yes" : "no");
    if (isConnected()) {
        std::cout << ", [KS " << fromKS << " -> KS " << toKS << "]";
    } else {
        std::cout << ", [not connected]";
    }
    std::cout << "\n";
}

void Pipe::load(const std::string& data) {
    if (data.empty()) return;
    size_t p1 = data.find(';');
    size_t p2 = data.find(';', p1 + 1);
    size_t p3 = data.find(';', p2 + 1);
    size_t p4 = data.find(';', p3 + 1);
    size_t p5 = data.find(';', p4 + 1);
    if (p3 == std::string::npos) return;
    try {
        full_name   = data.substr(0, p1);
        length      = std::stod(data.substr(p1 + 1, p2 - p1 - 1));
        diameter    = std::stod(data.substr(p2 + 1, p3 - p2 - 1));
        std::string rep = data.substr(p3 + 1, p4 != std::string::npos ? p4 - p3 - 1 : std::string::npos);
        underRepair = (rep == "Yes" || rep == "1");
        if (p4 != std::string::npos && p5 != std::string::npos) {
            fromKS = std::stoi(data.substr(p4 + 1, p5 - p4 - 1));
            toKS   = std::stoi(data.substr(p5 + 1));
        }
    } catch (...) {
        log("error parsing pipe data: " + data);
    }
}

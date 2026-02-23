#include "Pipe.h"
#include "utils.h"
#include <iostream>

void Pipe::toggleRepair() { underRepair = !underRepair; }

std::string Pipe::getInfo() const {
    return full_name + ";" +
           std::to_string(length) + ";" +
           std::to_string(diameter) + ";" +
           (underRepair ? "Yes" : "No") + ";" +
           std::to_string(src_id) + ";" +
           std::to_string(dst_id) + ";";
}

void Pipe::read(int pipe_id) {
    id = pipe_id;
    std::cout << "pipe name: ";
    std::getline(std::cin, full_name);
    log("pipe name: " + full_name);
    length   = readPositive<double>("length (km): ",   "must be positive.", 1e6);
    diameter = readPositive<double>("diameter (mm): ", "must be positive.", 1e6);
    underRepair = false;
    std::string s;
    std::cout << "src ks id (0=none): ";
    std::getline(std::cin, s);
    src_id = (int)numberOrDefault(s, 0);
    std::cout << "dst ks id (0=none): ";
    std::getline(std::cin, s);
    dst_id = (int)numberOrDefault(s, 0);
    log("pipe src=" + std::to_string(src_id) + " dst=" + std::to_string(dst_id));
}

void Pipe::display() const {
    std::cout << " pipe: " << full_name
              << ", length: " << length << " km"
              << ", diam: " << diameter << " mm"
              << ", repair: " << (underRepair ? "yes" : "no");
    if (src_id > 0 || dst_id > 0)
        std::cout << ", ks" << src_id << "->ks" << dst_id;
    std::cout << "\n";
}

void Pipe::load(const std::string& data) {
    if (data.empty()) return;
    size_t p1 = data.find(';');
    size_t p2 = data.find(';', p1 + 1);
    size_t p3 = data.find(';', p2 + 1);
    size_t p4 = (p3 != std::string::npos) ? data.find(';', p3 + 1) : std::string::npos;
    size_t p5 = (p4 != std::string::npos) ? data.find(';', p4 + 1) : std::string::npos;
    if (p1 == std::string::npos || p2 == std::string::npos || p3 == std::string::npos) return;
    try {
        full_name = data.substr(0, p1);
        length    = std::stod(data.substr(p1 + 1, p2 - p1 - 1));
        diameter  = std::stod(data.substr(p2 + 1, p3 - p2 - 1));
        std::string rep;
        if (p4 != std::string::npos)
            rep = data.substr(p3 + 1, p4 - p3 - 1);
        else {
            rep = data.substr(p3 + 1);
            if (!rep.empty() && rep.back() == ';') rep.pop_back();
        }
        underRepair = (rep == "Yes" || rep == "1");
        if (p4 != std::string::npos && p5 != std::string::npos) {
            src_id = std::stoi(data.substr(p4 + 1, p5 - p4 - 1));
            std::string rest = data.substr(p5 + 1);
            if (!rest.empty() && rest.back() == ';') rest.pop_back();
            if (!rest.empty()) dst_id = std::stoi(rest);
        }
    } catch (...) {
        log("error parsing pipe: " + data);
    }
}
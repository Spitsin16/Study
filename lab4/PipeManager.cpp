#include "PipeManager.h"
#include "utils.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>

static std::vector<int> parseIds(const std::string& input, const std::vector<int>& allowed) {
    if (input == "*") return allowed;
    std::vector<int> result;
    std::stringstream ss(input);
    std::string token;
    while (std::getline(ss, token, ',')) {
        token.erase(0, token.find_first_not_of(" \t"));
        token.erase(token.find_last_not_of(" \t") + 1);
        try {
            int id = std::stoi(token);
            if (std::find(allowed.begin(), allowed.end(), id) != allowed.end())
                result.push_back(id);
        } catch (...) {}
    }
    return result;
}

// возвращает ключи map отсортированными
static std::vector<int> sortedKeys(const std::unordered_map<int, Pipe>& m) {
    std::vector<int> keys;
    for (const auto& pair : m) keys.push_back(pair.first);
    std::sort(keys.begin(), keys.end());
    return keys;
}

void PipeManager::displayPipes(const std::unordered_map<int, Pipe>& subset) const {
    if (subset.empty()) { std::cout << "no pipes found.\n"; return; }
    for (int id : sortedKeys(subset)) {
        std::cout << "id " << id;
        subset.at(id).display();
    }
}

void PipeManager::toggleRepair(Pipe& pipe) {
    pipe.toggleRepair();
    log("pipe id " + std::to_string(pipe.getId()) +
        " repair toggled -> " + (pipe.isInRepairs() ? "yes" : "no"));
    std::cout << "repair status -> " << (pipe.isInRepairs() ? "yes" : "no") << "\n";
}

std::vector<int> PipeManager::selectIds(const std::unordered_map<int, Pipe>& found) const {
    std::vector<int> allowed = sortedKeys(found);
    std::cout << "enter ids separated by commas or * for all: ";
    std::string input;
    std::getline(std::cin, input);
    log("batch selection input: \"" + input + "\"");
    return parseIds(input, allowed);
}

void PipeManager::editByIds(const std::vector<int>& ids) {
    for (int id : ids) {
        std::unordered_map<int, Pipe>::iterator it = pipes.find(id);
        if (it != pipes.end()) {
            toggleRepair(it->second);
        } else {
            log("batch edit: pipe id " + std::to_string(id) + " not found");
        }
    }
}

void PipeManager::deleteByIds(const std::vector<int>& ids) {
    for (int id : ids) {
        if (pipes.erase(id)) {
            log("pipe id " + std::to_string(id) + " deleted");
            std::cout << "pipe id " << id << " deleted.\n";
        } else {
            log("batch delete: pipe id " + std::to_string(id) + " not found");
        }
    }
}

void PipeManager::addPipe() {
    while (pipes.count(nextId)) ++nextId;
    Pipe p;
    p.read(nextId);
    pipes[nextId] = p;
    log("pipe added with id: " + std::to_string(nextId));
    std::cout << "pipe added, id: " << nextId << "\n";
    ++nextId;
}

void PipeManager::editPipes() {
    if (pipes.empty()) { std::cout << "no pipes.\n"; return; }
    displayAllPipes();
    log("user opened edit pipes menu");
    std::cout << "1. edit by id(s)\n2. edit all\n";
    int choice = inputIntInRange("choose: ", 1, 2);
    if (choice == 1) {
        editByIds(selectIds(pipes));
    } else {
        for (auto& pair : pipes) toggleRepair(pair.second);
        log("all pipes repair status toggled");
    }
}

void PipeManager::deletePipes() {
    if (pipes.empty()) { std::cout << "no pipes.\n"; return; }
    displayAllPipes();
    log("user opened delete pipes menu");
    std::cout << "1. delete by id(s)\n2. delete all\n";
    int choice = inputIntInRange("choose: ", 1, 2);
    if (choice == 1) {
        deleteByIds(selectIds(pipes));
    } else {
        pipes.clear();
        log("all pipes deleted");
        std::cout << "all pipes deleted.\n";
    }
}

void PipeManager::displayAllPipes() const {
    if (pipes.empty()) {
        std::cout << "no pipes.\n";
    } else {
        std::cout << "\nall pipes:\n";
        for (int id : sortedKeys(pipes)) {
            std::cout << "id " << id;
            pipes.at(id).display();
        }
    }
    log("user viewed all pipes (" + std::to_string(pipes.size()) + " total)");
}

void PipeManager::searchPipes() {
    if (pipes.empty()) { std::cout << "no pipes.\n"; return; }
    std::cout << "1. by name\n2. by repair status\n3. by diameter\n0. back\n";
    int opt = inputIntInRange("choose filter: ", 0, 3);
    if (opt == 0) return;

    std::unordered_map<int, Pipe> found;
    if (opt == 1) {
        std::string name;
        std::cout << "enter name substring: ";
        INPUT_LINE(std::cin, name);
        log("search pipes by name: \"" + name + "\"");
        found = find_by_filter(pipes, filters::by_name<Pipe>, name);
    } else if (opt == 2) {
        bool rep = inputIntInRange("in repair? (1=yes, 0=no): ", 0, 1) == 1;
        log("search pipes by repair status: " + std::string(rep ? "yes" : "no"));
        found = find_by_filter(pipes, filters::pipe_by_repair, rep);
    } else if (opt == 3) {
        double d = readPositive<double>("enter diameter: ", "error: must be positive.", 1e7);
        log("search pipes by diameter: " + std::to_string(d));
        found = find_by_filter(pipes, filters::pipe_by_diameter, d);
    }

    log("search result: " + std::to_string(found.size()) + " pipe(s) found");
    displayPipes(found);
    if (found.empty()) return;

    std::cout << "1. batch edit found\n2. batch delete found\n0. back\n";
    int action = inputIntInRange("choose action: ", 0, 2);
    if (action == 1) {
        editByIds(selectIds(found));
    } else if (action == 2) {
        deleteByIds(selectIds(found));
    }
}

void PipeManager::managePipes() {
    while (true) {
        std::cout << "-----------------------------\n"
                  << "| pipe management:          |\n"
                  << "| 1. add                    |\n"
                  << "| 2. edit                   |\n"
                  << "| 3. delete                 |\n"
                  << "| 4. view all               |\n"
                  << "| 5. search & batch edit    |\n"
                  << "| 0. back                   |\n"
                  << "-----------------------------\n";
        int choice = inputIntInRange("choose action: ", 0, 5);
        switch (choice) {
            case 1: addPipe(); break;
            case 2: editPipes(); break;
            case 3: deletePipes(); break;
            case 4: displayAllPipes(); break;
            case 5: searchPipes(); break;
            case 0: log("left pipe management menu"); return;
        }
    }
}

void PipeManager::addPipeFromLoad(int id, const Pipe& pipe) {
    Pipe p = pipe;
    p.setId(id);
    pipes[id] = p;
    if (id >= nextId) nextId = id + 1;
    log("pipe loaded with id: " + std::to_string(id));
}

std::string PipeManager::savePipe(const Pipe& pipe) const {
    return pipe.getInfo();
}
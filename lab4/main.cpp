#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <ctime>
#include <vector>
#include <algorithm>
#include "PipeManager.h"
#include "CompressorStationManager.h"
#include "Network.h"
#include "utils.h"

using namespace std;

void saveToFile(const PipeManager& pm, const CompressorStationManager& sm) {
    string filename;
    cout << "filename to save: ";
    INPUT_LINE(cin, filename);
    log("save filename: \"" + filename + "\"");
    if (filename.empty()) { cout << "cancelled.\n"; return; }

    ofstream file(filename + ".txt");
    if (!file) { cerr << "failed to create file.\n"; return; }

    vector<int> skeys;
    for (const auto& pair : sm.getStations()) skeys.push_back(pair.first);
    sort(skeys.begin(), skeys.end());
    for (int id : skeys)
        file << "KS " << id << ": " << sm.saveCS(sm.getStations().at(id)) << '\n';

    vector<int> pkeys;
    for (const auto& pair : pm.getPipes()) pkeys.push_back(pair.first);
    sort(pkeys.begin(), pkeys.end());
    for (int id : pkeys)
        file << "pipe " << id << ": " << pm.savePipe(pm.getPipes().at(id)) << '\n';

    file.close();
    log("saved to " + filename + ".txt (" +
        to_string(skeys.size()) + " stations, " +
        to_string(pkeys.size()) + " pipes)");
    cout << "saved to " << filename << ".txt\n";
}

void loadFromFile(PipeManager& pm, CompressorStationManager& sm) {
    string filename;
    cout << "filename to load: ";
    INPUT_LINE(cin, filename);
    log("load filename: \"" + filename + "\"");
    if (filename.empty()) { cout << "cancelled.\n"; return; }

    ifstream file(filename + ".txt");
    if (!file) { cerr << "file not found: " << filename << ".txt\n"; return; }

    cout << "1. clear and load\n2. append\n";
    int mode = inputIntInRange("choose: ", 1, 2);
    log("load mode: " + to_string(mode));
    if (mode == 1) { pm.clear(); sm.clear(); }

    int lp = 0, ls = 0;
    string line;
    while (INPUT_LINE(file, line)) {
        if (line.empty()) continue;
        try {
            size_t pos = line.find(':');
            if (pos == string::npos) continue;
            size_t sp = line.find(' ');
            int id = stoi(line.substr(sp + 1, pos - sp - 1));
            string data = line.substr(pos + 2);
            if (line.substr(0, 2) == "KS") {
                CompressorStation cs; cs.load(data);
                sm.addStationFromLoad(id, cs);
                ++ls;
            } else if (line.substr(0, 4) == "pipe") {
                Pipe p; p.load(data);
                pm.addPipeFromLoad(id, p);
                ++lp;
            }
        } catch (...) {
            log("failed to parse line: " + line);
        }
    }
    file.close();
    log("loaded: " + to_string(ls) + " stations, " + to_string(lp) + " pipes");
    cout << "loaded from " << filename << ".txt\n";
}

int main() {
    {
        time_t now = time(nullptr);
        tm* tb = localtime(&now);
        ostringstream oss;
        oss << put_time(tb, "%d_%m_%Y_%H_%M_%S");
        g_logfile.open("log_" + oss.str() + ".txt");
    }
    redirect_output_wrapper cerr_redir(cerr);
    if (g_logfile) cerr_redir.redirect(g_logfile);

    log("program started");
    PipeManager pipeManager;
    CompressorStationManager stationManager;

    while (true) {
        cout << "-----------------------------\n"
             << "| 1. pipes                  |\n"
             << "| 2. compressor stations    |\n"
             << "| 3. view all               |\n"
             << "| 4. save                   |\n"
             << "| 5. load                   |\n"
             << "| 6. network                |\n"
             << "| 0. exit                   |\n"
             << "-----------------------------\n";
        int choice = inputIntInRange("choose: ", 0, 6);
        switch (choice) {
            case 1: pipeManager.managePipes(); break;
            case 2: stationManager.manageStations(); break;
            case 3:
                pipeManager.displayPipes(pipeManager.getPipes());
                stationManager.displayStations(stationManager.getStations());
                break;
            case 4: saveToFile(pipeManager, stationManager); break;
            case 5: loadFromFile(pipeManager, stationManager); break;
            case 6:
                log("entered network menu");
                Network::manage(pipeManager.getPipes(), stationManager.getStations());
                break;
            case 0:
                log("exit");
                return 0;
        }
    }
}
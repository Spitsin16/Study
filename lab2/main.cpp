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
#include "utils.h"

using namespace std;

// сохраняет объекты отсортированными по id
void saveToFile(const PipeManager& pm, const CompressorStationManager& sm) {
    string filename;
    cout << "enter filename to save: ";
    INPUT_LINE(cin, filename);
    log("user entered save filename: \"" + filename + "\"");
    if (filename.empty()) { cout << "save cancelled.\n"; return; }

    ofstream file(filename + ".txt");
    if (!file) {
        cerr << "failed to create file.\n";
        log("save failed: could not create " + filename + ".txt");
        return;
    }

    // сортируем по id перед записью
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
    log("data saved to file: " + filename + ".txt (" +
        to_string(skeys.size()) + " stations, " +
        to_string(pkeys.size()) + " pipes)");
    cout << "saved to " << filename << ".txt\n";
}

void loadFromFile(PipeManager& pm, CompressorStationManager& sm) {
    string filename;
    cout << "enter filename to load: ";
    INPUT_LINE(cin, filename);
    log("user entered load filename: \"" + filename + "\"");
    if (filename.empty()) { cout << "load cancelled.\n"; return; }

    ifstream file(filename + ".txt");
    if (!file) {
        cerr << "file not found: " << filename << ".txt\n";
        log("load failed: file not found: " + filename + ".txt");
        return;
    }

    // спрашиваем режим загрузки
    cout << "1. clear current data and load\n2. append to current data\n";
    int mode = inputIntInRange("choose mode: ", 1, 2);
    log("load mode selected: " + to_string(mode) + (mode == 1 ? " (clear+load)" : " (append)"));
    if (mode == 1) {
        pm.clear();
        sm.clear();
        log("current data cleared before load");
    }

    int loaded_pipes = 0, loaded_stations = 0;
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
                ++loaded_stations;
            } else if (line.substr(0, 4) == "pipe") {
                Pipe p; p.load(data);
                pm.addPipeFromLoad(id, p);
                ++loaded_pipes;
            }
        } catch (...) {
            log("failed to parse line: " + line);
        }
    }
    file.close();
    log("load complete: " + to_string(loaded_stations) + " station(s), " +
        to_string(loaded_pipes) + " pipe(s) from " + filename + ".txt");
    cout << "loaded from " << filename << ".txt\n";
}

int main() {
    {
        time_t now = time(nullptr);
        tm* tm_buf = localtime(&now);
        ostringstream oss;
        oss << put_time(tm_buf, "%d_%m_%Y_%H_%M_%S");
        g_logfile.open("log_" + oss.str() + ".txt");
    }
    redirect_output_wrapper cerr_redir(cerr);
    if (g_logfile) cerr_redir.redirect(g_logfile);

    log("program started");
    PipeManager pipeManager;
    CompressorStationManager stationManager;

    while (true) {
        cout << "-----------------------------\n"
             << "| main menu:                |\n"
             << "| 1. pipes                  |\n"
             << "| 2. compressor stations    |\n"
             << "| 3. view all               |\n"
             << "| 4. save                   |\n"
             << "| 5. load                   |\n"
             << "| 0. exit                   |\n"
             << "-----------------------------\n";
        int choice = inputIntInRange("choose action: ", 0, 5);
        switch (choice) {
            case 1:
                log("entered pipe management menu");
                pipeManager.managePipes();
                break;
            case 2:
                log("entered station management menu");
                stationManager.manageStations();
                break;
            case 3:
                log("user requested view all");
                pipeManager.displayPipes(pipeManager.getPipes());
                stationManager.displayStations(stationManager.getStations());
                break;
            case 4:
                log("user initiated save");
                saveToFile(pipeManager, stationManager);
                break;
            case 5:
                log("user initiated load");
                loadFromFile(pipeManager, stationManager);
                break;
            case 0:
                log("program exited by user");
                cout << "exiting.\n";
                return 0;
        }
    }
}
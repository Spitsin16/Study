#ifndef PIPEMANAGER_H
#define PIPEMANAGER_H
#include "Pipe.h"
#include "filters.h"
#include <unordered_map>
#include <vector>
#include <string>

class PipeManager {
    std::unordered_map<int, Pipe> pipes;
    int nextId = 1;

    void toggleRepair(Pipe& pipe);
    void editByIds(const std::vector<int>& ids);
    void deleteByIds(const std::vector<int>& ids);
    std::vector<int> selectIds(const std::unordered_map<int, Pipe>& found) const;

    void addPipe();
    void editPipes();
    void deletePipes();
    void displayAllPipes() const;
    void searchPipes();

public:
    void managePipes();
    void displayPipes(const std::unordered_map<int, Pipe>& subset) const;
    void clear() { pipes.clear(); nextId = 1; }

    const std::unordered_map<int, Pipe>& getPipes() const { return pipes; }
    void addPipeFromLoad(int id, const Pipe& pipe);
    std::string savePipe(const Pipe& pipe) const;
};

#endif
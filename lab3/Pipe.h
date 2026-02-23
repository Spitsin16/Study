#ifndef PIPE_H
#define PIPE_H
#include <string>

class Pipe {
    std::string full_name;
    int id = 0;
    double length = 0.0;
    double diameter = 0.0;
    bool underRepair = false;
    // network fields
    int fromKS = -1;   // id of input compressor station (-1 = not connected)
    int toKS = -1;     // id of output compressor station (-1 = not connected)
public:
    void read(int pipe_id);
    void readForNetwork(int pipe_id, double diam);
    void display() const;
    void load(const std::string& data);
    std::string getInfo() const;

    void toggleRepair();
    void setId(int pipe_id) { id = pipe_id; }
    int getId() const { return id; }
    const std::string& getName() const { return full_name; }
    bool isInRepairs() const { return underRepair; }
    double getDiameter() const { return diameter; }
    double getLength() const { return length; }

    // network methods
    void connectToNetwork(int from, int to) { fromKS = from; toKS = to; }
    void disconnectFromNetwork() { fromKS = -1; toKS = -1; }
    bool isConnected() const { return fromKS != -1 && toKS != -1; }
    bool isFree() const { return fromKS == -1 && toKS == -1; }
    int getFromKS() const { return fromKS; }
    int getToKS() const { return toKS; }
};

#endif

#ifndef COMPRESSORSTATION_H
#define COMPRESSORSTATION_H
#include <string>

class CompressorStation {
    std::string full_name;
    int id = 0;
    int totalShops = 0;
    int operationalShops = 0;
    double unusedPercent = 0.0;
    double efficiency = 0.0;
public:
    void read(int station_id);
    void display() const;
    void load(const std::string& data);
    std::string getInfo() const;

    void startShop();
    void stopShop();

    void setId(int station_id) { id = station_id; }
    int getId() const { return id; }
    const std::string& getName() const { return full_name; }
    int getAllCompressorShop() const { return totalShops; }
    int getActiveCompressorShop() const { return operationalShops; }
    double getUnusedPercent() const { return unusedPercent; }
};

#endif
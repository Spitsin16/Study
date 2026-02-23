#ifndef PIPE_H
#define PIPE_H
#include <string>

class Pipe {
    std::string full_name;
    int id = 0;
    double length = 0.0;
    double diameter = 0.0;
    bool underRepair = false;
public:
    void read(int pipe_id);
    void display() const;
    void load(const std::string& data);
    std::string getInfo() const;

    void toggleRepair();
    void setId(int pipe_id) { id = pipe_id; }
    int getId() const { return id; }
    const std::string& getName() const { return full_name; }
    bool isInRepairs() const { return underRepair; }
    double getDiameter() const { return diameter; }
};

#endif
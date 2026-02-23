#include "utils.h"
#include <sstream>
#include <ctime>
#include <limits>
#include <algorithm>
#include <cctype>

std::ofstream g_logfile;

void log(const std::string& operation) {
    std::time_t t = std::time(nullptr);
    std::string ts = std::ctime(&t);
    if (!ts.empty() && ts.back() == '\n') ts.pop_back();
    std::cerr << ts << " - " << operation << std::endl;
}

template <typename T>
T readPositive(const std::string& prompt, const std::string& error_message, T upper_limit) {
    T value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        bool valid = !std::cin.fail() && value > 0 && value < upper_limit;
        if (!valid) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            log("invalid input for: " + prompt);
            std::cout << error_message << "\n";
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            log("input [" + prompt + "] = " + std::to_string(value));
            return value;
        }
    }
}

template double readPositive<double>(const std::string&, const std::string&, double);
template int    readPositive<int>(const std::string&, const std::string&, int);

double inputDouble(const std::string& prompt) {
    double value;
    std::string line;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, line);
        std::stringstream ss(line);
        if (ss >> value && ss.eof()) {
            log("input [" + prompt + "] = " + std::to_string(value));
            return value;
        }
        log("invalid double input for: " + prompt + ", got: \"" + line + "\"");
        std::cout << "error: enter a valid number.\n";
    }
}

int inputInt(const std::string& prompt) {
    int value;
    std::string line;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, line);
        std::stringstream ss(line);
        if (ss >> value && ss.eof()) {
            log("input [" + prompt + "] = " + std::to_string(value));
            return value;
        }
        log("invalid int input for: " + prompt + ", got: \"" + line + "\"");
        std::cout << "error: enter a valid integer.\n";
    }
}

long numberOrDefault(const std::string& input, long default_value) {
    if (input.empty()) return default_value;
    for (unsigned char c : input)
        if (!std::isdigit(c)) return default_value;
    char* end;
    long v = std::strtol(input.c_str(), &end, 10);
    return (*end == '\0') ? v : default_value;
}

int inputIntInRange(const std::string& prompt, int lo, int hi) {
    int value;
    std::string line;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, line);
        std::stringstream ss(line);
        if (ss >> value && ss.eof() && value >= lo && value <= hi) {
            log("input [" + prompt + "] = " + std::to_string(value));
            return value;
        }
        log("invalid input [" + prompt + "]: \"" + line + "\" (expected " +
            std::to_string(lo) + ".." + std::to_string(hi) + ")");
        std::cout << "error: enter an integer from " << lo << " to " << hi << ".\n";
    }
}

std::istream& INPUT_LINE(std::istream& in, std::string& str) {
    std::getline(in >> std::ws, str);
    return in;
}
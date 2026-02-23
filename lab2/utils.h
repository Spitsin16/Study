#ifndef UTILS_H
#define UTILS_H
#include <ostream>
#include <streambuf>
#include <string>
#include <iostream>
#include <fstream>

extern std::ofstream g_logfile;

void log(const std::string& operation);

template <typename T>
T readPositive(const std::string& prompt, const std::string& error_message, T upper_limit);

long numberOrDefault(const std::string& input, long default_value);
int inputIntInRange(const std::string& prompt, int lower_bound, int upper_bound);
int inputInt(const std::string& prompt);
double inputDouble(const std::string& prompt);
std::istream& INPUT_LINE(std::istream& in, std::string& str);

class redirect_output_wrapper {
    std::ostream& stream;
    std::streambuf* const old_buf;
public:
    redirect_output_wrapper(std::ostream& src)
        : stream(src), old_buf(src.rdbuf()) {}
    ~redirect_output_wrapper() {
        stream.rdbuf(old_buf);
    }
    void redirect(std::ostream& dest) {
        stream.rdbuf(dest.rdbuf());
    }
};

#endif
#ifndef FILTERS_H
#define FILTERS_H
#include <string>
#include <unordered_map>
#include "Pipe.h"
#include "CompressorStation.h"

template <typename T, typename Filter, typename Param>
std::unordered_map<int, T> find_by_filter(
    const std::unordered_map<int, T>& objects, Filter filter, Param param)
{
    std::unordered_map<int, T> result;
    for (typename std::unordered_map<int, T>::const_iterator it = objects.begin();
         it != objects.end(); ++it)
    {
        if (filter(it->second, param))
            result[it->first] = it->second;
    }
    return result;
}

namespace filters {
    template<typename T>
    bool by_name(const T& obj, const std::string& name) {
        return obj.getName().find(name) != std::string::npos;
    }

    inline bool pipe_by_repair(const Pipe& p, bool rep) {
        return p.isInRepairs() == rep;
    }

    inline bool pipe_by_diameter(const Pipe& p, double d) {
        return p.getDiameter() == d;
    }

    inline bool station_by_unused(const CompressorStation& s, double minPct) {
        if (s.getAllCompressorShop() == 0) return false;
        return s.getUnusedPercent() >= minPct;
    }
}

#endif
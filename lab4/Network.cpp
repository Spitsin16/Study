#include "Network.h"
#include "utils.h"
#include <cmath>
#include <map>
#include <set>
#include <queue>
#include <limits>
#include <algorithm>
#include <iostream>

static const double INF = std::numeric_limits<double>::infinity();

double Network::pipeCap(const Pipe& p) {
    if (p.isInRepairs()) return 0.0;
    double d = p.getDiameter() / 1000.0;
    double l = p.getLength() * 1000.0;
    if (l <= 0.0 || d <= 0.0) return 0.0;
    return 1e7 * std::sqrt(std::pow(d, 5.0) / l);
}

double Network::pipeW(const Pipe& p) {
    return p.isInRepairs() ? INF : p.getLength();
}

double Network::maxFlow(const std::unordered_map<int,Pipe>& pipes, int s, int t) {
    std::map<int, std::map<int,double>> cap;
    for (std::unordered_map<int,Pipe>::const_iterator it = pipes.begin(); it != pipes.end(); ++it) {
        const Pipe& p = it->second;
        if (p.getSrc() <= 0 || p.getDst() <= 0) continue;
        int u = p.getSrc(), v = p.getDst();
        cap[u][v] += pipeCap(p);
        if (!cap[v].count(u)) cap[v][u] = 0.0;
    }
    if (!cap.count(s) || !cap.count(t)) return 0.0;

    double flow = 0.0;
    while (true) {
        std::map<int,int> prev;
        prev[s] = s;
        std::queue<int> q;
        q.push(s);
        while (!q.empty() && !prev.count(t)) {
            int u = q.front(); q.pop();
            for (std::map<int,double>::iterator jt = cap[u].begin(); jt != cap[u].end(); ++jt) {
                int v = jt->first;
                double c = jt->second;
                if (!prev.count(v) && c > 1e-9) {
                    prev[v] = u;
                    q.push(v);
                }
            }
        }
        if (!prev.count(t)) break;
        double aug = INF;
        for (int v = t; v != s; ) {
            int u = prev[v];
            aug = std::min(aug, cap[u][v]);
            v = u;
        }
        for (int v = t; v != s; ) {
            int u = prev[v];
            cap[u][v] -= aug;
            cap[v][u] += aug;
            v = u;
        }
        flow += aug;
    }
    return flow;
}

double Network::shortPath(const std::unordered_map<int,Pipe>& pipes, int s, int t,
                          std::vector<int>& path) {
    std::map<int, std::vector<std::pair<int,double>>> adj;
    std::set<int> nodes;
    for (std::unordered_map<int,Pipe>::const_iterator it = pipes.begin(); it != pipes.end(); ++it) {
        const Pipe& p = it->second;
        if (p.getSrc() <= 0 || p.getDst() <= 0) continue;
        nodes.insert(p.getSrc());
        nodes.insert(p.getDst());
        double w = pipeW(p);
        if (w < INF)
            adj[p.getSrc()].push_back(std::make_pair(p.getDst(), w));
    }
    if (!nodes.count(s) || !nodes.count(t)) return INF;

    std::map<int,double> dist;
    std::map<int,int> prev;
    for (std::set<int>::iterator it = nodes.begin(); it != nodes.end(); ++it)
        dist[*it] = INF;
    dist[s] = 0.0;

    typedef std::pair<double,int> pdi;
    std::priority_queue<pdi, std::vector<pdi>, std::greater<pdi>> pq;
    pq.push(std::make_pair(0.0, s));

    while (!pq.empty()) {
        double d = pq.top().first;
        int u   = pq.top().second;
        pq.pop();
        if (d > dist[u] + 1e-9) continue;
        std::vector<std::pair<int,double>>& nbrs = adj[u];
        for (size_t i = 0; i < nbrs.size(); ++i) {
            int v = nbrs[i].first;
            double w = nbrs[i].second;
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                prev[v] = u;
                pq.push(std::make_pair(dist[v], v));
            }
        }
    }

    double res = dist.count(t) ? dist[t] : INF;
    path.clear();
    if (res < INF) {
        for (int v = t; v != s; v = prev[v])
            path.push_back(v);
        path.push_back(s);
        std::reverse(path.begin(), path.end());
    }
    return res;
}

void Network::manage(const std::unordered_map<int,Pipe>& pipes,
                     const std::unordered_map<int,CompressorStation>& stations) {
    while (true) {
        std::cout << "network:\n"
                  << "1. connections\n"
                  << "2. max flow\n"
                  << "3. shortest path\n"
                  << "0. back\n";
        int c = inputIntInRange("choose: ", 0, 3);
        if (c == 0) return;

        if (c == 1) {
            bool any = false;
            for (std::unordered_map<int,Pipe>::const_iterator it = pipes.begin(); it != pipes.end(); ++it) {
                const Pipe& p = it->second;
                if (p.getSrc() > 0 && p.getDst() > 0) {
                    std::cout << "pipe id " << it->first
                              << ": ks" << p.getSrc() << " -> ks" << p.getDst()
                              << ", cap=" << pipeCap(p)
                              << ", w=";
                    double w = pipeW(p);
                    if (w >= INF) std::cout << "inf";
                    else          std::cout << w << " km";
                    std::cout << "\n";
                    any = true;
                }
            }
            if (!any) std::cout << "no connections.\n";
            continue;
        }

        std::string s;
        std::cout << "src ks id: ";
        std::getline(std::cin, s);
        int src = (int)numberOrDefault(s, 0);
        std::cout << "dst ks id: ";
        std::getline(std::cin, s);
        int dst = (int)numberOrDefault(s, 0);
        if (src <= 0 || dst <= 0) { std::cout << "invalid id.\n"; continue; }

        if (c == 2) {
            double f = maxFlow(pipes, src, dst);
            std::cout << "max flow ks" << src << "->ks" << dst << ": " << f << "\n";
            log("max flow ks" + std::to_string(src) + "->ks" + std::to_string(dst)
                + " = " + std::to_string(f));
        } else {
            std::vector<int> path;
            double d = shortPath(pipes, src, dst, path);
            if (d >= INF) {
                std::cout << "no path.\n";
            } else {
                std::cout << "path: ";
                for (int i = 0; i < (int)path.size(); ++i) {
                    if (i) std::cout << " -> ";
                    std::cout << "ks" << path[i];
                }
                std::cout << ", dist=" << d << " km\n";
            }
            log("shortest path ks" + std::to_string(src) + "->ks" + std::to_string(dst)
                + " dist=" + std::to_string(d));
        }
    }
}
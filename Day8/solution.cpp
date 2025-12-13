#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <tuple>
#include <unordered_map>
#include <algorithm>

#include "../helpers/helper.h"

class Solution {
public:
    template <typename T>
    T part1(const std::vector<std::string>& input) {
        struct Point { long long x, y, z; };
        std::vector<Point> points;
        points.reserve(input.size());

        for (const auto& line : input) {
            if (line.empty()) continue;
            size_t c1 = line.find(',');
            size_t c2 = line.find(',', c1 + 1);
            if (c1 == std::string::npos || c2 == std::string::npos) continue;

            long long x = std::stoll(line.substr(0, c1));
            long long y = std::stoll(line.substr(c1 + 1, c2 - c1 - 1));
            long long z = std::stoll(line.substr(c2 + 1));
            points.push_back({x, y, z});
        }

        int n = (int)points.size();
        if (n == 0) return (T)0;

        struct DSU {
            std::vector<int> parent, sz;
            DSU(int n) : parent(n), sz(n, 1) { for (int i = 0; i < n; i++) parent[i] = i; }
            int find(int a) { return parent[a] == a ? a : (parent[a] = find(parent[a])); }
            bool unite(int a, int b) {
                a = find(a); b = find(b);
                if (a == b) return false;
                if (sz[a] < sz[b]) std::swap(a, b);
                parent[b] = a;
                sz[a] += sz[b];
                return true;
            }
        };

        auto dist2 = [&](int i, int j) -> long long {
            long long dx = points[i].x - points[j].x;
            long long dy = points[i].y - points[j].y;
            long long dz = points[i].z - points[j].z;
            return dx*dx + dy*dy + dz*dz;
        };

        const int K = 1000;
        using Edge = std::tuple<long long, int, int>; // (dist^2, i, j)

        auto cmpMax = [](const Edge& a, const Edge& b) {
            return std::get<0>(a) < std::get<0>(b); // max-heap by dist^2
        };
        std::priority_queue<Edge, std::vector<Edge>, decltype(cmpMax)> pq(cmpMax);

        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                long long d = dist2(i, j);
                if ((int)pq.size() < K) {
                    pq.emplace(d, i, j);
                } else if (d < std::get<0>(pq.top())) {
                    pq.pop();
                    pq.emplace(d, i, j);
                }
            }
        }

        std::vector<Edge> edges;
        edges.reserve(pq.size());
        while (!pq.empty()) {
            edges.push_back(pq.top());
            pq.pop();
        }
        std::sort(edges.begin(), edges.end(),
                  [](const Edge& a, const Edge& b) { return std::get<0>(a) < std::get<0>(b); });

        DSU dsu(n);
        for (auto& [d, i, j] : edges) dsu.unite(i, j);

        std::unordered_map<int, long long> comp;
        comp.reserve(n * 2);
        for (int i = 0; i < n; i++) comp[dsu.find(i)]++;

        std::vector<long long> sizes;
        sizes.reserve(comp.size());
        for (auto& kv : comp) sizes.push_back(kv.second);

        std::sort(sizes.begin(), sizes.end(), std::greater<long long>());

        long long ans = 1;
        for (int i = 0; i < 3 && i < (int)sizes.size(); i++) ans *= sizes[i];
        return (T)ans;
    }

    template <typename T>
    T part2(const std::vector<std::string>& input) {
        struct Point { long long x, y, z; };
        std::vector<Point> points;
        points.reserve(input.size());

        for (const auto& line : input) {
            if (line.empty()) continue;
            size_t c1 = line.find(',');
            size_t c2 = line.find(',', c1 + 1);
            if (c1 == std::string::npos || c2 == std::string::npos) continue;

            long long x = std::stoll(line.substr(0, c1));
            long long y = std::stoll(line.substr(c1 + 1, c2 - c1 - 1));
            long long z = std::stoll(line.substr(c2 + 1));
            points.push_back({x, y, z});
        }

        int n = (int)points.size();
        if (n < 2) return (T)0;

        struct DSU {
            std::vector<int> parent, sz;
            DSU(int n) : parent(n), sz(n, 1) { for (int i = 0; i < n; i++) parent[i] = i; }
            int find(int a) { return parent[a] == a ? a : (parent[a] = find(parent[a])); }
            bool unite(int a, int b) {
                a = find(a); b = find(b);
                if (a == b) return false;
                if (sz[a] < sz[b]) std::swap(a, b);
                parent[b] = a;
                sz[a] += sz[b];
                return true;
            }
        };

        auto dist2 = [&](int i, int j) -> long long {
            long long dx = points[i].x - points[j].x;
            long long dy = points[i].y - points[j].y;
            long long dz = points[i].z - points[j].z;
            return dx*dx + dy*dy + dz*dz;
        };

        // Build *all* edges, then Kruskal in (dist^2, i, j) order (deterministic).
        using Edge = std::tuple<long long, int, int>;
        std::vector<Edge> edges;
        edges.reserve((size_t)n * (n - 1) / 2);

        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                edges.emplace_back(dist2(i, j), i, j);
            }
        }

        std::sort(edges.begin(), edges.end(),
                  [](const Edge& a, const Edge& b) {
                      if (std::get<0>(a) != std::get<0>(b)) return std::get<0>(a) < std::get<0>(b);
                      if (std::get<1>(a) != std::get<1>(b)) return std::get<1>(a) < std::get<1>(b);
                      return std::get<2>(a) < std::get<2>(b);
                  });

        DSU dsu(n);
        int components = n;

        for (const auto& [d, i, j] : edges) {
            if (dsu.unite(i, j)) {
                components--;
                if (components == 1) {
                    // This is the final edge needed to connect everything.
                    long long product = points[i].x * points[j].x;
                    return (T)product;
                }
            }
        }

        return (T)0; // should not happen for n>=2
    }
};

int main() {
    Solution sol;

    std::ifstream inputFile("input.txt");
    if (!inputFile.is_open()) {
        Log::error("The input file is missing.");
        return 1;
    }

    std::string line;
    std::vector<std::string> input;

    while (std::getline(inputFile, line)) {
        input.push_back(strip(line));
    }

    // ------------ Part 1 ------------
    Timer::start();
    auto answer1 = sol.part1<long long>(input);
    Log::result("Part 1 Answer: " + std::to_string(answer1));
    Timer::print("Part 1 Execution Time");

    // ------------ Part 2 ------------
    Timer::start();
    auto answer2 = sol.part2<long long>(input);
    Log::result("Part 2 Answer: " + std::to_string(answer2));
    Timer::print("Part 2 Execution Time");

    return 0;
}

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <functional>
#include "../helpers/helper.h"

class Solution {
public:
    template <typename T>
    T part1(const std::vector<std::string>& input) {
        // Build adjacency list: device -> list of downstream devices
        std::unordered_map<std::string, std::vector<std::string>> adj;
        adj.reserve(input.size() * 2);

        for (const std::string& raw : input) {
            if (raw.empty()) continue;

            // Expected: "name: a b c" (possibly no outputs)
            auto pos = raw.find(':');
            if (pos == std::string::npos) continue;

            std::string from = strip(raw.substr(0, pos));
            std::string rest = strip(raw.substr(pos + 1));

            std::vector<std::string> outs;
            if (!rest.empty()) {
                std::istringstream iss(rest);
                std::string to;
                while (iss >> to) outs.push_back(to);
            }

            adj[from] = std::move(outs);
        }

        const std::string START = "you";
        const std::string TARGET = "out";

        // Memoized DFS count of paths in a directed graph (assumes no cycles on valid inputs).
        // If a cycle is encountered, we treat it as contributing 0 paths to avoid infinite recursion.
        std::unordered_map<std::string, long long> memo;
        memo.reserve(adj.size() * 2);

        std::unordered_set<std::string> onStack;
        onStack.reserve(adj.size() * 2);

        std::function<long long(const std::string&)> dfs = [&](const std::string& u) -> long long {
            if (u == TARGET) return 1LL;

            auto itMemo = memo.find(u);
            if (itMemo != memo.end()) return itMemo->second;

            if (onStack.find(u) != onStack.end()) {
                // Cycle detected; puzzle inputs are expected to be acyclic.
                return 0LL;
            }

            onStack.insert(u);

            long long total = 0;
            auto it = adj.find(u);
            if (it != adj.end()) {
                for (const std::string& v : it->second) {
                    total += dfs(v);
                }
            }

            onStack.erase(u);
            memo[u] = total;
            return total;
        };

        long long ans = dfs(START);
        return static_cast<T>(ans);
    }

    template <typename T>
    T part2(const std::vector<std::string>& input) {
        // Build adjacency list
        std::unordered_map<std::string, std::vector<std::string>> adj;
        adj.reserve(input.size() * 2);

        for (const std::string& raw : input) {
            if (raw.empty()) continue;

            auto pos = raw.find(':');
            if (pos == std::string::npos) continue;

            std::string from = strip(raw.substr(0, pos));
            std::string rest = strip(raw.substr(pos + 1));

            std::vector<std::string> outs;
            if (!rest.empty()) {
                std::istringstream iss(rest);
                std::string to;
                while (iss >> to) outs.push_back(to);
            }

            adj[from] = std::move(outs);
        }

        const std::string START = "svr";
        const std::string TARGET = "out";
        const std::string NEED1 = "dac";
        const std::string NEED2 = "fft";

        auto bitFor = [&](const std::string& node) -> int {
            int m = 0;
            if (node == NEED1) m |= 1; // dac
            if (node == NEED2) m |= 2; // fft
            return m;
        };

        // dp[node][mask] stored as "node|mask" -> count
        struct Key {
            std::string node;
            int mask;
            bool operator==(const Key& other) const { return mask == other.mask && node == other.node; }
        };
        struct KeyHash {
            std::size_t operator()(const Key& k) const noexcept {
                std::size_t h1 = std::hash<std::string>{}(k.node);
                std::size_t h2 = std::hash<int>{}(k.mask);
                // hash combine
                return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));
            }
        };

        std::unordered_map<Key, long long, KeyHash> memo;
        memo.reserve(adj.size() * 8);

        std::unordered_set<Key, KeyHash> onStack;
        onStack.reserve(adj.size() * 8);

        std::function<long long(const std::string&, int)> dfs = [&](const std::string& u, int mask) -> long long {
            mask |= bitFor(u);

            if (u == TARGET) {
                return (mask == 3) ? 1LL : 0LL; // must have visited both dac and fft
            }

            Key k{u, mask};
            auto itMemo = memo.find(k);
            if (itMemo != memo.end()) return itMemo->second;

            if (onStack.find(k) != onStack.end()) {
                // Cycle detected; treat as 0 to avoid infinite recursion.
                return 0LL;
            }
            onStack.insert(k);

            long long total = 0;
            auto it = adj.find(u);
            if (it != adj.end()) {
                for (const std::string& v : it->second) {
                    total += dfs(v, mask);
                }
            }

            onStack.erase(k);
            memo[k] = total;
            return total;
        };

        long long ans = dfs(START, 0);
        return static_cast<T>(ans);
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
    auto answer1 = sol.part1<long long>(input);   // or int, string, etc.
    Log::result("Part 1 Answer: " + std::to_string(answer1));
    Timer::print("Part 1 Execution Time");

    // ------------ Part 2 ------------
    Timer::start();
    auto answer2 = sol.part2<long long>(input);   // or int, string, etc.
    Log::result("Part 2 Answer: " + std::to_string(answer2));
    Timer::print("Part 2 Execution Time");

    return 0;
}

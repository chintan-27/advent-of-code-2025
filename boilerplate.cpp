#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../helpers/helper.h"

class Solution {
public:
    // Part 1 (return type flexible)
    template <typename T>
    T part1(const std::vector<std::string>& input) {
        // TODO: return whatever type Part 1 needs
        // Example: return (T)123;
        return T{};
    }

    // Part 2 (return type flexible)
    template <typename T>
    T part2(const std::vector<std::string>& input) {
        // TODO: return whatever type Part 2 needs
        // Example: return (T)456;
        return T{};
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

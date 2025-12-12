#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "../helpers/helper.h"

class Solution
{
public:

    long long part1(const std::vector<std::string> &raw)
    {
        int H = (int)raw.size();
        int W = (int)raw[0].size();

        int sx = -1, sy = -1;
        for (int y = 0; y < H; y++)
            for (int x = 0; x < W; x++)
                if (raw[y][x] == 'S')
                    sx = x, sy = y;

        std::vector<char> cur(W, 0), nxt(W, 0);
        if (sy + 1 < H)
            cur[sx] = 1;

        long long splits = 0;

        for (int y = sy + 1; y < H; y++)
        {
            std::fill(nxt.begin(), nxt.end(), 0);

            for (int x = 0; x < W; x++)
            {
                if (!cur[x])
                    continue;

                if (raw[y][x] == '^')
                {
                    splits++;
                    if (y + 1 < H)
                    {
                        if (x - 1 >= 0)
                            nxt[x - 1] = 1;
                        if (x + 1 < W)
                            nxt[x + 1] = 1;
                    }
                }
                else
                {
                    if (y + 1 < H)
                        nxt[x] = 1;
                }
            }

            cur.swap(nxt);
        }

        return splits;
    }

    long long part2(const std::vector<std::string> &raw)
    {
        int H = (int)raw.size();
        int W = (int)raw[0].size();

        int sx = -1, sy = -1;
        for (int y = 0; y < H; y++)
            for (int x = 0; x < W; x++)
                if (raw[y][x] == 'S')
                    sx = x, sy = y;

        std::vector<long long> cur(W), nxt(W);
        if (sy + 1 < H)
            cur[sx] = 1;

        long long exits = 0;

        for (int y = sy + 1; y < H; y++)
        {
            std::fill(nxt.begin(), nxt.end(), 0);

            for (int x = 0; x < W; x++)
            {
                if (cur[x] == 0)
                    continue;

                if (raw[y][x] == '^')
                {
                    if (y + 1 >= H)
                    {
                        if (x - 1 >= 0)
                            exits += cur[x];
                        if (x + 1 < W)
                            exits += cur[x];
                    }
                    else
                    {
                        if (x - 1 >= 0)
                            nxt[x - 1] += cur[x];
                        if (x + 1 < W)
                            nxt[x + 1] += cur[x];
                    }
                }
                else
                {
                    if (y + 1 >= H)
                        exits += cur[x];
                    else
                        nxt[x] += cur[x];
                }
            }

            cur.swap(nxt);
        }

        return exits;
    }
};

int main()
{
    Solution sol;

    std::ifstream inputFile("input.txt");
    if (!inputFile.is_open())
    {
        Log::error("The input file is missing.");
        return 1;
    }

    std::string line;
    std::vector<std::string> raw;

    while (std::getline(inputFile, line))
        raw.push_back(line);

    size_t width = 0;
    for (auto &s : raw)
        width = std::max(width, s.size());
    for (auto &s : raw)
        if (s.size() < width)
            s += std::string(width - s.size(), ' ');

    Timer::start();
    auto answer1 = sol.part1(raw);
    Log::result("Part 1 Answer: " + std::to_string(answer1));
    Timer::print("Part 1 Execution Time");

    Timer::start();
    long long answer2 = sol.part2(raw);
    std::ostringstream oss;
    oss << answer2;
    Log::result("Part 2 Answer: " + oss.str());
    Timer::print("Part 2 Execution Time");

    return 0;
}

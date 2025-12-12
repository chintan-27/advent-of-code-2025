#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "../helpers/helper.h"

class Solution
{
public:
    long long part1(const std::vector<std::vector<std::string>> &input,
                    const std::vector<std::string> &ops)
    {
        long long ans = 0;

        for (size_t i = 0; i < ops.size(); i++)
        {
            long long val = (ops[i] == "*") ? 1LL : 0LL;

            for (size_t r = 0; r < input.size(); r++)
            {
                long long x = std::stoll(input[r][i]);
                if (ops[i] == "*")
                    val *= x;
                else
                    val += x;
            }

            ans += val;
        }

        return ans;
    }

    long long part2(const std::vector<std::string> &raw,
                    const std::vector<std::pair<int, int>> &segs,
                    int opRow)
    {
        long long total = 0;

        for (auto [l, r] : segs)
        {
            int w = r - l;

            char op = '\0';
            for (int j = 0; j < w; j++)
            {
                char ch = raw[opRow][l + j];
                if (ch != ' ')
                {
                    op = ch;
                    break;
                }
            }

            std::vector<long long> nums;

            for (int col = w - 1; col >= 0; col--)
            {
                std::string digits;
                for (int rr = 0; rr < opRow; rr++)
                {
                    char ch = raw[rr][l + col];
                    if (ch >= '0' && ch <= '9')
                        digits.push_back(ch);
                }

                if (!digits.empty())
                    nums.push_back(std::stoll(digits));
            }

            long long val = (op == '*') ? 1LL : 0LL;
            if (op == '*')
            {
                for (auto x : nums)
                    val *= x;
            }
            else
            {
                for (auto x : nums)
                    val += x;
            }

            total += val;
        }

        return total;
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

    int rows = raw.size();
    int opRow = rows - 1;

    std::vector<bool> isSep(width, true);
    for (size_t c = 0; c < width; c++)
    {
        for (int r = 0; r < rows; r++)
        {
            if (raw[r][c] != ' ')
            {
                isSep[c] = false;
                break;
            }
        }
    }

    std::vector<std::pair<int, int>> segs;
    int c = 0;
    while (c < (int)width)
    {
        while (c < (int)width && isSep[c])
            c++;
        if (c >= (int)width)
            break;

        int l = c;
        while (c < (int)width && !isSep[c])
            c++;
        int r = c;

        segs.push_back({l, r});
    }

    std::vector<std::vector<std::string>> input(opRow);
    std::vector<std::string> ops;

    for (auto [l, r] : segs)
    {
        for (int rIdx = 0; rIdx < opRow; rIdx++)
            input[rIdx].push_back(strip(raw[rIdx].substr(l, r - l)));

        ops.push_back(strip(raw[opRow].substr(l, r - l)));
    }

    Timer::start();
    auto answer1 = sol.part1(input, ops);
    Log::result("Part 1 Answer: " + std::to_string(answer1));
    Timer::print("Part 1 Execution Time");

    Timer::start();
    auto answer2 = sol.part2(raw, segs, opRow);
    Log::result("Part 2 Answer: " + std::to_string(answer2));
    Timer::print("Part 2 Execution Time");

    return 0;
}

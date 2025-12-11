#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../helpers/helper.h"

class Solution
{
public:
    // Part 1 (return type flexible)
    template <typename T = std::pair<int, std::vector<std::string>>>
    T part1(const std::vector<std::string> &input)
    {
        int lines = input.size(), ans = 0;
        std::vector<std::string> temp(input);
        for (int i = 0; i < lines; i++)
        {
            for (int j = 0; j < input[0].length(); j++)
            {
                if (input[i][j] == '.')
                    continue;
                int count = 0;
                if (i > 0)
                {
                    if (input[i - 1][j] == '@')
                        count++;
                    if (j > 0)
                    {
                        if (input[i - 1][j - 1] == '@')
                            count++;
                    }
                    if (j < input[0].length())
                    {
                        if (input[i - 1][j + 1] == '@')
                            count++;
                    }
                }

                if (i < input.size())
                {
                    if (input[i + 1][j] == '@')
                        count++;
                    if (j > 0)
                    {
                        if (input[i + 1][j - 1] == '@')
                            count++;
                    }
                    if (j < input[0].length())
                    {
                        if (input[i + 1][j + 1] == '@')
                            count++;
                    }
                }

                if (j > 0)
                    if (input[i][j - 1] == '@')
                        count++;

                if (j < input[0].length())
                    if (input[i][j + 1] == '@')
                        count++;

                if (count < 4)
                {
                    ans++;
                    temp[i][j] = '.';
                }
            }
        }

        return std::make_pair(ans, temp);
    }

    // Part 2 (return type flexible)
    template <typename T>
    T part2(const std::vector<std::string> &input)
    {
        auto [answer, temp] = part1(input);
        int ans = 0;

        while (true)
        {
            ans += answer;
            if (answer == 0)
                break;

            auto [nextAnswer, nextTemp] = part1(temp);
            answer = nextAnswer;
            temp = nextTemp;
        }

        return T{ans};
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
    std::vector<std::string> input;

    while (std::getline(inputFile, line))
    {
        input.push_back(strip(line));
    }

    // ------------ Part 1 ------------
    Timer::start();
    auto [answer1, temp] = sol.part1(input); // or int, string, etc.
    Log::result("Part 1 Answer: " + std::to_string(answer1));
    Timer::print("Part 1 Execution Time");

    // ------------ Part 2 ------------
    // Timer::start();
    auto answer2 = sol.part2<int>(input); // or int, string, etc.
    Log::result("Part 2 Answer: " + std::to_string(answer2));
    Timer::print("Part 2 Execution Time");

    return 0;
}

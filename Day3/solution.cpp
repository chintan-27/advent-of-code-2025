#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../helpers/helper.h"

class Solution
{
public:
    int Lobby(std::vector<std::string> batteries)
    {
        int ans = 0;
        for (std::string battery : batteries)
        {
            int index = 0, mx = 0;
            for (int i = 0; i < battery.length(); i++)
            {
                int val = battery[i] - '0';
                if (val > mx)
                {
                    mx = val;
                    index = i;
                }
            }

            if (index == battery.length() - 1)
            {
                int mx2 = 0;
                for (int i = 0; i < battery.length() - 1; i++)
                {
                    int val2 = battery[i] - '0';
                    if (val2 > mx2)
                    {
                        mx2 = val2;
                        index = i;
                    }
                }
                ans += (mx2 * 10) + mx;
            }
            else
            {
                int mx2 = 0;
                for (int i = index + 1; i < battery.length(); i++)
                {
                    int val2 = battery[i] - '0';
                    if (val2 > mx2)
                    {
                        mx2 = val2;
                        index = i;
                    }
                }
                ans += (mx * 10) + mx2;
            }
        }
        return ans;
    }

    long long LobbyPart2(const std::vector<std::string> &batteries)
    {
        const int K = 12; // number of batteries to turn on in each bank
        long long ans = 0;

        for (const std::string &battery : batteries)
        {
            int n = battery.size();

            int start = 0;
            std::string chosen;
            chosen.reserve(K);

            for (int pos = 0; pos < K; ++pos)
            {
                int remaining = K - pos;
                int end = n - remaining;

                char best = '0';
                int bestIndex = start;
                for (int i = start; i <= end; ++i)
                {
                    if (battery[i] > best)
                    {
                        best = battery[i];
                        bestIndex = i;
                    }
                }

                chosen.push_back(best);
                start = bestIndex + 1;
            }

            long long val = 0;
            for (char c : chosen)
            {
                val = val * 10 + (c - '0');
            }

            ans += val;
        }

        return ans;
    }
};

int main()
{
    Timer::start();
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

    auto result = sol.LobbyPart2(input);
    Log::result("Answer: " + std::to_string(result));

    Timer::print("Execution Time");
    return 0;
}

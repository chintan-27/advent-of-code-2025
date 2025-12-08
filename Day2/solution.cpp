#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../helpers/helper.h"

class Solution
{
public:
    long long gift_shop(std::vector<std::vector<std::string>> ranges)
    {
        long long ans = 0.0;
        // SOLUTION PART 1
        // for (auto range: ranges)
        // {
        //     if ((range[0].length() % 2 == 0) || (range[1].length() % 2 == 0))
        //     {
        //         long long start = stoll(range[0]), end = stoll(range[1]);
        //         for (; start <= end; start += 1)
        //         {
        //             std::string temp = std::to_string(start);
        //             size_t midpoint = temp.length() / 2;
        //             std::string firstHalf = temp.substr(0, midpoint);
        //             std::string secondHalf = temp.substr(midpoint);
        //             if(firstHalf == secondHalf){
        //                 ans += stoll(temp);
        //             }
        //         }
        //     }
        // }

        // SOLUTION PART 2
        for (auto range : ranges)
        {
            long long start = stoll(range[0]), end = stoll(range[1]);
            for (; start <= end; start += 1)
            {
                std::string temp = std::to_string(start);
                int n = temp.length();

                for (int i = 1; i <= n / 2; ++i)
                {
                    if (n % i != 0)
                        continue;

                    bool allEqual = true;
                    std::string pattern = temp.substr(0, i);

                    for (int pos = i; pos < n; pos += i)
                    {
                        if (temp.compare(pos, i, pattern) != 0)
                        {
                            allEqual = false;
                            break;
                        }
                    }

                    if (allEqual)
                    {
                        ans += start;
                        break;
                    }
                }
            }
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
    std::vector<std::vector<std::string>> input;

    while (std::getline(inputFile, line))
    {
        for (auto range : split(strip(line), ','))
        {
            input.emplace_back(split(strip(range), '-'));
        }
    }

    auto result = sol.gift_shop(input);
    Log::result("Answer: " + std::to_string(result));

    Timer::print("Execution Time");
    return 0;
}

// DAY 1: SECRET ENTRANCE
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include "../helpers/helper.h"

class Solution
{
public:
    int secretEntrance(std::vector<std::string> rotations)
    {
        int position = 50;
        int ans = 0;
        for (auto rotation : rotations)
        {
            int rotate = 0;
            try
            { 
                rotate = std::stoi(rotation.substr(1));
                // rotate %= 100; -> PART 1

                
            }
            catch (const std::invalid_argument &e) {std::cerr << " [ERROR]: Converting to number of rotation -> Invalid argument: " << e.what() << std::endl;}
            catch (const std::out_of_range &e) {std::cerr << "[ERROR]: Converting to number of rotation -> Out of range: " << e.what() << std::endl;}

            // SOLUTION FOR PART 1
            // if (rotation[0] == 'L')
            // {
            //     position = position - rotate;
            //     if (position < 0) position = 100 + position;
            // }
            // else if (rotation[0] == 'R') position = (position + rotate) % 100;
            // else std::cerr << "[ERROR]: Invalid Rotation encountered -> " << rotation << std::endl;
            // if (position == 0) ans++; 
            
            // SOLUTION FOR PART 2
            int full = rotate / 100;
            rotate %= 100;
            ans += full;
            if (rotation[0] == 'L')
            {
                if (rotate > position && position != 0) ans++;
                position = position - rotate;
                if (position < 0) position = 100 + position;
            }
            else if (rotation[0] == 'R') {
                if (rotate > 100 - position && position != 0) ans++;
                position = (position + rotate) % 100;
            }
            else std::cerr << "[ERROR]: Invalid Rotation encountered -> " << rotation << std::endl;
            if (position == 0) ans++; 


        }
        return ans;
    }
};

int main()
{
    Solution sol;

    std::ifstream inputFile("input.txt");
    if (!inputFile.is_open())
    {
        std::cerr << "[ERROR]: The input file is missing." << std::endl;
        return 1;
    }

    std::string line;
    std::vector<std::string> rotations;
    while (std::getline(inputFile, line))
    {
        rotations.emplace_back(strip(line));
    }
    int result = sol.secretEntrance(rotations);
    std::cout << "[RESULT]: " << result << std::endl;
    return 0;
}

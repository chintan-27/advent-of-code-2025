#pragma once
#include <string>
#include <cctype>
#include <iostream>
#include <chrono>
#include <sstream>
#include <vector>

//
// ===============================
// STRING UTILITIES
// ===============================
//
inline std::string strip(const std::string &inpt)
{
    if (inpt.empty()) return "";

    auto start_it = inpt.begin();
    while (start_it != inpt.end() && std::isspace((unsigned char)*start_it))
        ++start_it;

    auto end_it = inpt.end();
    do {
        --end_it;
    } while (end_it != start_it && std::isspace((unsigned char)*end_it));

    return std::string(start_it, end_it + 1);
}

inline std::vector<std::string> split(const std::string &input, char delim)
{
    std::vector<std::string> parts;
    std::string current;

    for (char c : input)
    {
        if (c == delim)
        {
            parts.push_back(current);
            current.clear();
        }
        else
        {
            current.push_back(c);
        }
    }

    // push the last segment
    parts.push_back(current);

    return parts;
}
//
// ===============================
// LOGGING UTILITIES
// ===============================
//
namespace Log
{
    // Toggle debug logs
    inline bool DEBUG = true;

    // Colors
    inline const std::string RED     = "\033[31m";
    inline const std::string GREEN   = "\033[32m";
    inline const std::string YELLOW  = "\033[33m";
    inline const std::string BLUE    = "\033[34m";
    inline const std::string MAGENTA = "\033[35m";
    inline const std::string CYAN    = "\033[36m";
    inline const std::string RESET   = "\033[0m";

    inline void error(const std::string &msg)
    {
        std::cerr << RED << "[ERROR] " << msg << RESET << std::endl;
    }

    inline void debug(const std::string &msg)
    {
        if (DEBUG)
            std::cout << CYAN << "[DEBUG] " << msg << RESET << std::endl;
    }

    inline void warn(const std::string &msg)
    {
        std::cout << YELLOW << "[WARN] " << msg << RESET << std::endl;
    }

    inline void info(const std::string &msg)
    {
        std::cout << BLUE << "[INFO] " << msg << RESET << std::endl;
    }

    inline void result(const std::string &msg)
    {
        std::cout << GREEN << "[RESULT] " << msg << RESET << std::endl;
    }
}

//
// ===============================
// ASSERTIONS
// ===============================
//
inline void assertOrError(bool condition, const std::string &msg)
{
    if (!condition)
    {
        Log::error("Assertion failed: " + msg);
        std::exit(1);
    }
}

//
// ===============================
// TIMER UTILITIES
// ===============================
//
namespace Timer
{
    inline std::chrono::high_resolution_clock::time_point start_time;

    inline void start()
    {
        start_time = std::chrono::high_resolution_clock::now();
    }

    inline double stop()
    {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> ms = end - start_time;
        return ms.count();
    }

    inline void print(const std::string &label = "Execution Time")
    {
        double ms = stop();
        std::cout << Log::MAGENTA << "[" << label << "] "
                  << ms << " ms" << Log::RESET << std::endl;
    }
}

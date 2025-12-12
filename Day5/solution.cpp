#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../helpers/helper.h"

class IntervalTree
{
private:
    struct Interval
    {
        long long low, high;
    };

    struct Node
    {
        Interval *i;
        long long max;
        Node *left, *right;
    };

    Node *tree = nullptr;

    Node *newNode(const Interval &i)
    {
        Node *temp = new Node;
        temp->i = new Interval(i);
        temp->max = i.high;
        temp->left = temp->right = nullptr;
        return temp;
    }

    // recursive helper
    Node *insert(Node *root, const Interval &i)
    {
        if (root == nullptr)
            return newNode(i);

        long long l = root->i->low;
        if (i.low < l)
            root->left = insert(root->left, i);
        else
            root->right = insert(root->right, i);
        if (root->max < i.high)
            root->max = i.high;

        return root;
    }
    bool exists(Node *root, long long x) const
    {
        if (root == nullptr)
            return false;

        if (x >= root->i->low && x <= root->i->high)
            return true;

        if (root->left != nullptr && root->left->max >= x)
            return exists(root->left, x);

        return exists(root->right, x);
    }
    void inorder(Node *root,
                 long long &curL,
                 long long &curR,
                 long long &total,
                 bool &hasCurrent) const
    {
        if (!root)
            return;

        inorder(root->left, curL, curR, total, hasCurrent);

        long long L = root->i->low;
        long long R = root->i->high;

        if (!hasCurrent)
        {
            curL = L;
            curR = R;
            hasCurrent = true;
        }
        else if (L <= curR + 1) // overlap or touching
        {
            if (R > curR)
                curR = R;
        }
        else
        {
            total += (curR - curL + 1);
            curL = L;
            curR = R;
        }

        inorder(root->right, curL, curR, total, hasCurrent);
    }

public:
    void insert(long long low, long long high)
    {
        Interval i{low, high};
        tree = insert(tree, i);
    }
    bool exists(long long x) const
    {
        return exists(tree, x);
    }
    long long totalCoveredLength() const
    {
        long long total = 0;
        long long curL = 0, curR = 0;
        bool hasCurrent = false;

        inorder(tree, curL, curR, total, hasCurrent);

        if (hasCurrent)
            total += (curR - curL + 1);

        return total;
    }
};

class Solution
{
public:
    // Part 1 (return type flexible)
    int part1(const std::vector<std::pair<long long, long long>> ranges, const std::vector<long long> items)
    {
        int ans = 0;
        IntervalTree it;
        for (auto range : ranges)
        {
            it.insert(range.first, range.second);
        }
        for (long long item : items)
        {
            if (it.exists(item))
                ans++;
        }
        return ans;
    }

    // Part 2 (return type flexible)
    long long part2(const std::vector<std::pair<long long, long long>> &ranges)
    {
        IntervalTree it;
        for (auto &r : ranges)
            it.insert(r.first, r.second);

        return it.totalCoveredLength();
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
    std::vector<std::pair<long long, long long>> ranges;
    std::vector<long long> items;

    while (std::getline(inputFile, line))
    {
        line = strip(line);
        if (split(line, '-').size() > 1)
        {
            std::vector<std::string> rng = split(line, '-');
            ranges.push_back(std::make_pair(std::stoll(rng[0]), std::stoll(rng[1])));
        }
        else if (line != "")
        {
            items.push_back(std::stoll(line));
        }
    }

    // ------------ Part 1 ------------
    Timer::start();
    auto answer1 = sol.part1(ranges, items); // or int, string, etc.
    Log::result("Part 1 Answer: " + std::to_string(answer1));
    Timer::print("Part 1 Execution Time");

    // ------------ Part 2 ------------
    Timer::start();
    auto answer2 = sol.part2(ranges);
    Log::result("Part 2 Answer: " + std::to_string(answer2));
    Timer::print("Part 2 Execution Time");

    return 0;
}

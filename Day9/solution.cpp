#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <deque>
#include <cstdlib>

#include "../helpers/helper.h"

class Solution
{
public:
    template <typename T>
    T part1(const std::vector<std::string> &input)
    {
        struct Pt
        {
            long long x, y;
        };
        std::vector<Pt> red;
        red.reserve(input.size());

        for (const auto &line : input)
        {
            if (line.empty())
                continue;
            size_t c = line.find(',');
            if (c == std::string::npos)
                continue;
            long long x = std::stoll(line.substr(0, c));
            long long y = std::stoll(line.substr(c + 1));
            red.push_back({x, y});
        }

        long long best = 0;
        for (int i = 0; i < (int)red.size(); i++)
        {
            for (int j = i + 1; j < (int)red.size(); j++)
            {
                long long dx = std::llabs(red[i].x - red[j].x);
                long long dy = std::llabs(red[i].y - red[j].y);
                long long area = (dx + 1) * (dy + 1); // inclusive tiles
                best = std::max(best, area);
            }
        }
        return (T)best;
    }

    template <typename T>
    T part2(const std::vector<std::string> &input)
    {
        struct Pt
        {
            long long x, y;
        };
        std::vector<Pt> red;
        red.reserve(input.size());

        for (const auto &line : input)
        {
            if (line.empty())
                continue;
            size_t c = line.find(',');
            if (c == std::string::npos)
                continue;
            long long x = std::stoll(line.substr(0, c));
            long long y = std::stoll(line.substr(c + 1));
            red.push_back({x, y});
        }

        const int R = (int)red.size();
        if (R < 2)
            return (T)0;

        // ---- Coordinate compression on tile boundaries ----
        // Each tile at (x,y) is unit square [x,x+1) x [y,y+1)
        std::vector<long long> xs, ys;
        xs.reserve(R * 2 + 4);
        ys.reserve(R * 2 + 4);

        long long minX = red[0].x, maxX = red[0].x;
        long long minY = red[0].y, maxY = red[0].y;
        for (auto &p : red)
        {
            minX = std::min(minX, p.x);
            maxX = std::max(maxX, p.x);
            minY = std::min(minY, p.y);
            maxY = std::max(maxY, p.y);
            xs.push_back(p.x);
            xs.push_back(p.x + 1);
            ys.push_back(p.y);
            ys.push_back(p.y + 1);
        }

        // padding boundary so flood-fill can start outside safely
        xs.push_back(minX - 1);
        xs.push_back(maxX + 2);
        ys.push_back(minY - 1);
        ys.push_back(maxY + 2);

        std::sort(xs.begin(), xs.end());
        xs.erase(std::unique(xs.begin(), xs.end()), xs.end());
        std::sort(ys.begin(), ys.end());
        ys.erase(std::unique(ys.begin(), ys.end()), ys.end());

        auto idx_of = [](const std::vector<long long> &v, long long val) -> int
        {
            return (int)(std::lower_bound(v.begin(), v.end(), val) - v.begin());
        };

        // Cell grid is between boundaries: size (H = ys.size()-1, W = xs.size()-1)
        const int W = (int)xs.size() - 1;
        const int H = (int)ys.size() - 1;

        std::vector<int> wlen(W), hlen(H);
        for (int i = 0; i < W; i++)
            wlen[i] = (int)(xs[i + 1] - xs[i]); // number of tile-columns
        for (int j = 0; j < H; j++)
            hlen[j] = (int)(ys[j + 1] - ys[j]); // number of tile-rows

        auto cellArea = [&](int cx, int cy) -> long long
        {
            return 1LL * wlen[cx] * hlen[cy];
        };

        auto id = [&](int x, int y) -> int
        { return y * W + x; };
        auto inb = [&](int x, int y) -> bool
        { return x >= 0 && x < W && y >= 0 && y < H; };

        // boundary blocks movement; then we fill interior
        std::vector<unsigned char> blocked(W * H, 0); // boundary cells
        std::vector<unsigned char> inside(W * H, 0);  // interior+boundary after fill

        // Map each red tile (x,y) to its cell index (cx,cy) where boundary contains x and y
        std::vector<std::pair<int, int>> redCell(R);
        for (int i = 0; i < R; i++)
        {
            int cx = idx_of(xs, red[i].x);
            int cy = idx_of(ys, red[i].y);
            // cx,cy index a boundary; the tile cell is exactly that cell in compressed grid
            // because we ensured x and x+1 exist in xs, same for y.
            redCell[i] = {cx, cy};
        }

        // ---- Rasterize boundary segments onto compressed cell grid ----
        auto mark_horiz = [&](long long y, long long x1, long long x2)
        {
            if (x2 < x1)
                std::swap(x1, x2);
            int cy = idx_of(ys, y);
            int cx1 = idx_of(xs, x1);
            int cx2 = idx_of(xs, x2);
            // segment covers tiles from x1..x2-1 at row y => cells cx1..cx2-1 in that cy
            for (int cx = cx1; cx < cx2; cx++)
            {
                if (inb(cx, cy))
                    blocked[id(cx, cy)] = 1;
            }
        };

        auto mark_vert = [&](long long x, long long y1, long long y2)
        {
            if (y2 < y1)
                std::swap(y1, y2);
            int cx = idx_of(xs, x);
            int cy1 = idx_of(ys, y1);
            int cy2 = idx_of(ys, y2);
            for (int cy = cy1; cy < cy2; cy++)
            {
                if (inb(cx, cy))
                    blocked[id(cx, cy)] = 1;
            }
        };

        for (int i = 0; i < R; i++)
        {
            Pt a = red[i];
            Pt b = red[(i + 1) % R];
            if (a.x == b.x)
                mark_vert(a.x, a.y, b.y);
            else
                mark_horiz(a.y, a.x, b.x);
        }

        // ---- Flood-fill "outside" through NON-blocked cells, on compressed grid ----
        std::vector<unsigned char> vis(W * H, 0);
        std::deque<std::pair<int, int>> dq;

        // start from a guaranteed outside cell: use cell containing (minX-1, minY-1)
        int sx = idx_of(xs, minX - 1);
        int sy = idx_of(ys, minY - 1);
        if (!inb(sx, sy))
        {
            sx = 0;
            sy = 0;
        }

        if (!blocked[id(sx, sy)])
        {
            vis[id(sx, sy)] = 1;
            dq.push_back({sx, sy});
        }

        const int dx4[4] = {1, -1, 0, 0};
        const int dy4[4] = {0, 0, 1, -1};

        while (!dq.empty())
        {
            auto [x, y] = dq.front();
            dq.pop_front();
            for (int k = 0; k < 4; k++)
            {
                int nx = x + dx4[k], ny = y + dy4[k];
                if (!inb(nx, ny))
                    continue;
                int nid = id(nx, ny);
                if (vis[nid])
                    continue;
                if (blocked[nid])
                    continue; // boundary blocks flood
                vis[nid] = 1;
                dq.push_back({nx, ny});
            }
        }

        // inside = boundary OR not outside
        for (int y = 0; y < H; y++)
        {
            for (int x = 0; x < W; x++)
            {
                int idx = id(x, y);
                if (blocked[idx] || !vis[idx])
                    inside[idx] = 1;
            }
        }

        // ---- Prefix sums of total area and allowed(inside) area ----
        // ps[y+1][x+1]
        std::vector<std::vector<long long>> psTotal(H + 1, std::vector<long long>(W + 1, 0));
        std::vector<std::vector<long long>> psAllow(H + 1, std::vector<long long>(W + 1, 0));

        for (int y = 0; y < H; y++)
        {
            long long rowT = 0, rowA = 0;
            for (int x = 0; x < W; x++)
            {
                long long a = cellArea(x, y);
                rowT += a;
                if (inside[id(x, y)])
                    rowA += a;
                psTotal[y + 1][x + 1] = psTotal[y][x + 1] + rowT;
                psAllow[y + 1][x + 1] = psAllow[y][x + 1] + rowA;
            }
        }

        auto rectSum = [&](const std::vector<std::vector<long long>> &ps,
                           int x1, int y1, int x2, int y2) -> long long
        {
            // inclusive cells [x1..x2], [y1..y2]
            return ps[y2 + 1][x2 + 1] - ps[y1][x2 + 1] - ps[y2 + 1][x1] + ps[y1][x1];
        };

        // ---- Check red-corner rectangles ----
        long long best = 0;

        for (int i = 0; i < R; i++)
        {
            auto [xA, yA] = redCell[i];
            for (int j = i + 1; j < R; j++)
            {
                auto [xB, yB] = redCell[j];

                int x1 = std::min(xA, xB), x2 = std::max(xA, xB);
                int y1 = std::min(yA, yB), y2 = std::max(yA, yB);

                long long total = rectSum(psTotal, x1, y1, x2, y2);
                if (total <= best)
                    continue;

                long long allow = rectSum(psAllow, x1, y1, x2, y2);
                if (allow == total)
                    best = total;
            }
        }

        return (T)best;
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
    auto answer1 = sol.part1<long long>(input);
    Log::result("Part 1 Answer: " + std::to_string(answer1));
    Timer::print("Part 1 Execution Time");

    // ------------ Part 2 ------------
    Timer::start();
    auto answer2 = sol.part2<long long>(input);
    Log::result("Part 2 Answer: " + std::to_string(answer2));
    Timer::print("Part 2 Execution Time");

    return 0;
}

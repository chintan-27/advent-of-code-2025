#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <unordered_map>
#include <algorithm>
#include <cstdint>

#include "../helpers/helper.h"

class Solution {
    static constexpr int SH = 3;
    static constexpr int SW = 3;

    struct ShapeVariant {
        // Cells (x,y) within 3x3 that are '#'
        std::vector<std::pair<int,int>> cells;
    };

    static std::vector<int> extractInts(const std::string& s) {
        static const std::regex re(R"(\d+)");
        std::vector<int> out;
        for (auto it = std::sregex_iterator(s.begin(), s.end(), re);
             it != std::sregex_iterator(); ++it) {
            out.push_back(std::stoi((*it).str()));
        }
        return out;
    }

    static std::vector<std::string> splitByEmptyLines(const std::vector<std::string>& lines) {
        std::vector<std::string> blocks;
        std::string cur;
        for (const auto& ln : lines) {
            if (ln.empty()) {
                if (!cur.empty()) {
                    blocks.push_back(cur);
                    cur.clear();
                }
            } else {
                cur += ln;
                cur.push_back('\n');
            }
        }
        if (!cur.empty()) blocks.push_back(cur);
        return blocks;
    }

    static std::vector<std::string> splitLinesKeepNonEmpty(const std::string& block) {
        std::vector<std::string> out;
        std::string cur;
        for (char c : block) {
            if (c == '\n') {
                if (!cur.empty()) out.push_back(cur);
                cur.clear();
            } else {
                cur.push_back(c);
            }
        }
        if (!cur.empty()) out.push_back(cur);
        return out;
    }

    static std::vector<std::string> parseShapeGrid(const std::string& block) {
        // block looks like:
        // "0:\n###\n##.\n##.\n"
        auto lines = splitLinesKeepNonEmpty(block);
        // skip "k:" line
        std::vector<std::string> grid;
        for (size_t i = 1; i < lines.size(); i++) grid.push_back(lines[i]);
        return grid;
    }

    static int countTiles(const std::vector<std::string>& grid) {
        int cnt = 0;
        for (auto& r : grid) for (char c : r) if (c == '#') cnt++;
        return cnt;
    }

    static std::vector<std::pair<int,int>> cellsFromGrid(const std::vector<std::string>& grid) {
        std::vector<std::pair<int,int>> cells;
        for (int y = 0; y < SH; y++) {
            for (int x = 0; x < SW; x++) {
                if (grid[y][x] == '#') cells.push_back({x,y});
            }
        }
        return cells;
    }

    static std::vector<std::pair<int,int>> transformCells(
        const std::vector<std::pair<int,int>>& cells,
        int rot, bool flipX
    ) {
        // coordinates in 0..2
        // apply optional flip about vertical axis, then rotate rot*90
        std::vector<std::pair<int,int>> out;
        out.reserve(cells.size());
        for (auto [x,y] : cells) {
            int tx = flipX ? (2 - x) : x;
            int ty = y;

            int rx = tx, ry = ty;
            // rotate around origin then re-map within 0..2 using known 3x3 rotations
            // rot=0: (x,y)
            // rot=1: (2-y, x)
            // rot=2: (2-x, 2-y)
            // rot=3: (y, 2-x)
            if (rot == 1) { rx = 2 - ty; ry = tx; }
            else if (rot == 2) { rx = 2 - tx; ry = 2 - ty; }
            else if (rot == 3) { rx = ty; ry = 2 - tx; }

            out.push_back({rx, ry});
        }
        // Normalize to keep within 0..2 (already is), but remove duplicates by sorting.
        std::sort(out.begin(), out.end());
        out.erase(std::unique(out.begin(), out.end()), out.end());
        return out;
    }

    static std::vector<ShapeVariant> uniqueVariants(const std::vector<std::pair<int,int>>& base) {
        std::vector<ShapeVariant> vars;
        std::vector<std::vector<std::pair<int,int>>> seen;

        for (bool flipX : {false, true}) {
            for (int rot = 0; rot < 4; rot++) {
                auto c = transformCells(base, rot, flipX);
                // check uniqueness
                if (std::find(seen.begin(), seen.end(), c) == seen.end()) {
                    seen.push_back(c);
                    vars.push_back({c});
                }
            }
        }
        return vars;
    }

    // Exact packing using bitmasks (works best when area <= 64; otherwise fall back to uint64_t vector)
    // We'll implement generic using vector<uint64_t> bitset.
    struct Bitset {
        int nbits;
        std::vector<uint64_t> w;
        explicit Bitset(int n=0): nbits(n), w((n+63)/64, 0ULL) {}
        inline bool test(int i) const { return (w[i>>6] >> (i&63)) & 1ULL; }
        inline void set(int i) { w[i>>6] |= (1ULL << (i&63)); }
        inline void reset(int i) { w[i>>6] &= ~(1ULL << (i&63)); }

        inline bool intersects(const Bitset& other) const {
            for (size_t k = 0; k < w.size(); k++) if (w[k] & other.w[k]) return true;
            return false;
        }
        inline void OR(const Bitset& other) {
            for (size_t k = 0; k < w.size(); k++) w[k] |= other.w[k];
        }
        inline void ANDNOT(const Bitset& other) {
            for (size_t k = 0; k < w.size(); k++) w[k] &= ~other.w[k];
        }
        inline int firstZero() const {
            // returns index of first 0 bit, or nbits if full
            int idx = 0;
            for (size_t k = 0; k < w.size(); k++) {
                uint64_t inv = ~w[k];
                if (inv) {
                    int bit = __builtin_ctzll(inv);
                    idx = int(k*64 + bit);
                    if (idx < nbits) return idx;
                }
            }
            return nbits;
        }
    };

    struct PlacementOption {
        Bitset mask; // occupied squares for this placement
    };

    static std::vector<PlacementOption> enumeratePlacements(
        int W, int H, const ShapeVariant& var
    ) {
        std::vector<PlacementOption> opts;
        int area = W*H;

        // Determine bounding box of shape cells (within 3x3)
        int minx=10, miny=10, maxx=-10, maxy=-10;
        for (auto [x,y] : var.cells) {
            minx = std::min(minx, x);
            miny = std::min(miny, y);
            maxx = std::max(maxx, x);
            maxy = std::max(maxy, y);
        }
        int bw = maxx - minx + 1;
        int bh = maxy - miny + 1;

        for (int oy = 0; oy + bh <= H; oy++) {
            for (int ox = 0; ox + bw <= W; ox++) {
                Bitset m(area);
                bool ok = true;
                for (auto [x,y] : var.cells) {
                    int gx = ox + (x - minx);
                    int gy = oy + (y - miny);
                    if (gx < 0 || gx >= W || gy < 0 || gy >= H) { ok = false; break; }
                    int idx = gy * W + gx;
                    m.set(idx);
                }
                if (ok) opts.push_back({m});
            }
        }
        return opts;
    }

    static bool canPackExact(
        int W, int H,
        const std::vector<std::vector<PlacementOption>>& allOptionsPerPiece
    ) {
        const int area = W*H;
        const int n = (int)allOptionsPerPiece.size();

        // order pieces by fewest options first
        std::vector<int> order(n);
        for (int i = 0; i < n; i++) order[i] = i;
        std::sort(order.begin(), order.end(), [&](int a, int b){
            return allOptionsPerPiece[a].size() < allOptionsPerPiece[b].size();
        });

        Bitset used(area);

        // Simple DFS with "must cover first empty cell" heuristic:
        // pick next piece in order, but restrict placements to those that cover the first empty cell sometimes
        // We'll do: at each depth, compute first empty; if remaining tiles equal area-used, still ok.
        std::function<bool(int)> dfs = [&](int depth) -> bool {
            if (depth == n) return true;

            int pieceId = order[depth];

            // optional heuristic: if there's an empty cell, try placements that cover it first
            int firstEmpty = used.firstZero();
            const auto& opts = allOptionsPerPiece[pieceId];

            // Try covering firstEmpty first (if possible)
            for (int pass = 0; pass < 2; pass++) {
                for (const auto& op : opts) {
                    if (op.mask.intersects(used)) continue;
                    bool covers = (firstEmpty < area) ? op.mask.test(firstEmpty) : true;
                    if ((pass == 0 && !covers) || (pass == 1 && covers)) continue;

                    used.OR(op.mask);
                    if (dfs(depth + 1)) return true;
                    used.ANDNOT(op.mask);
                }
            }
            return false;
        };

        return dfs(0);
    }

public:
    template <typename T>
    T part1(const std::vector<std::string>& input) {
        auto blocks = splitByEmptyLines(input);
        if (blocks.size() < 2) return (T)0;

        // All blocks except last are shapes; last block contains region lines.
        std::vector<std::string> shapeBlocks(blocks.begin(), blocks.end() - 1);
        std::string regionBlock = blocks.back();

        // Parse shapes and count tiles + variants
        std::vector<int> shapeTiles;
        std::vector<std::vector<ShapeVariant>> shapeVariants;

        for (const auto& sb : shapeBlocks) {
            auto grid = parseShapeGrid(sb);
            // Expect 3 lines of 3 chars
            if ((int)grid.size() != SH) continue;

            shapeTiles.push_back(countTiles(grid));

            auto baseCells = cellsFromGrid(grid);
            shapeVariants.push_back(uniqueVariants(baseCells));
        }

        // Parse regions
        auto regionLines = splitLinesKeepNonEmpty(regionBlock);

        long long total = 0;

        for (const auto& rl : regionLines) {
            auto nums = extractInts(rl);
            if (nums.size() < 2) continue;
            int W = nums[0];
            int H = nums[1];

            std::vector<int> qty;
            for (size_t i = 2; i < nums.size(); i++) qty.push_back(nums[i]);

            // Pad qty if needed (in case shapes are more than listed, or vice versa)
            if (qty.size() < shapeTiles.size()) qty.resize(shapeTiles.size(), 0);

            long long numPresents = 0;
            for (int q : qty) numPresents += q;

            // Guaranteed fit: each present fits in its own 3x3 cell
            long long cellCount = (W / SW) * 1LL * (H / SH);
            if (numPresents <= cellCount) {
                total++;
                continue;
            }

            // Tile lower bound must fit
            long long neededTiles = 0;
            for (size_t i = 0; i < shapeTiles.size() && i < qty.size(); i++) {
                neededTiles += 1LL * shapeTiles[i] * qty[i];
            }
            long long area = 1LL * W * H;
            if (neededTiles > area) {
                continue;
            }

            // Exact packer fallback (complete correctness)
            // Build explicit list of pieces: each entry refers to a shape index
            std::vector<int> pieces;
            for (size_t s = 0; s < qty.size() && s < shapeVariants.size(); s++) {
                for (int k = 0; k < qty[s]; k++) pieces.push_back((int)s);
            }

            // Precompute placement options per piece (union of options across variants)
            std::vector<std::vector<PlacementOption>> allOptionsPerPiece;
            allOptionsPerPiece.reserve(pieces.size());

            for (int sidx : pieces) {
                std::vector<PlacementOption> optsUnion;
                for (const auto& var : shapeVariants[sidx]) {
                    auto opts = enumeratePlacements(W, H, var);
                    // append
                    optsUnion.insert(optsUnion.end(), opts.begin(), opts.end());
                }
                // If a piece has no placements at all, impossible
                if (optsUnion.empty()) {
                    optsUnion.clear();
                }
                allOptionsPerPiece.push_back(std::move(optsUnion));
            }

            bool ok = true;
            for (auto& opts : allOptionsPerPiece) {
                if (opts.empty()) { ok = false; break; }
            }
            if (!ok) continue;

            if (canPackExact(W, H, allOptionsPerPiece)) total++;
        }

        return (T)total;
    }

    template <typename T>
    T part2(const std::vector<std::string>& input) {
        // Day 12 Part 2 not provided in your prompt.
        // Put your Part 2 logic here once you share the statement.
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

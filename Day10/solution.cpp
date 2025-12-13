#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <climits>
#include <unordered_map>

#include "../helpers/helper.h"

class Solution {
public:
    static std::string extract_bracket(const std::string& s) {
        auto l = s.find('[');
        auto r = s.find(']', l + 1);
        if (l == std::string::npos || r == std::string::npos) return "";
        return s.substr(l + 1, r - l - 1);
    }

    static std::vector<std::vector<int>> extract_parens(const std::string& s) {
        std::vector<std::vector<int>> res;
        for (size_t i = 0; i < s.size();) {
            if (s[i] != '(') { i++; continue; }
            size_t j = s.find(')', i + 1);
            if (j == std::string::npos) break;
            std::string t = s.substr(i + 1, j - i - 1);
            std::vector<int> v;
            int cur = 0;
            bool in = false;
            for (char c : t) {
                if (std::isdigit((unsigned char)c)) {
                    cur = cur * 10 + (c - '0');
                    in = true;
                } else {
                    if (in) {
                        v.push_back(cur);
                        cur = 0;
                        in = false;
                    }
                }
            }
            if (in) v.push_back(cur);
            res.push_back(v);
            i = j + 1;
        }
        return res;
    }

    static std::vector<int> extract_braces_nums(const std::string& s) {
        auto l = s.find('{');
        auto r = s.find('}', l + 1);
        if (l == std::string::npos || r == std::string::npos) return {};
        std::string t = s.substr(l + 1, r - l - 1);

        std::vector<int> v;
        int cur = 0;
        bool in = false;
        for (char c : t) {
            if (std::isdigit((unsigned char)c)) {
                cur = cur * 10 + (c - '0');
                in = true;
            } else {
                if (in) { v.push_back(cur); cur = 0; in = false; }
            }
        }
        if (in) v.push_back(cur);
        return v;
    }

    static int popcount(unsigned long long x) {
#if defined(__GNUG__) || defined(__clang__)
        return __builtin_popcountll(x);
#else
        int c = 0;
        while (x) { x &= x - 1; c++; }
        return c;
#endif
    }

    // ------------------------- Part 1 -------------------------

    static int solve_machine_part1(const std::string& line) {
        std::string pat = extract_bracket(line);
        if (pat.empty()) return 0;
        int n = (int)pat.size();
        auto buttons = extract_parens(line);
        int m = (int)buttons.size();
        if (n > 63 || m > 63) return 0;

        unsigned long long target = 0;
        for (int i = 0; i < n; i++) if (pat[i] == '#') target |= 1ULL << i;

        std::vector<unsigned long long> A(m);
        for (int i = 0; i < m; i++) {
            unsigned long long v = 0;
            for (int x : buttons[i]) if (0 <= x && x < n) v ^= 1ULL << x;
            A[i] = v;
        }

        std::vector<unsigned long long> basis(n, 0), combo(n, 0);
        for (int i = 0; i < m; i++) {
            unsigned long long v = A[i], c = 1ULL << i;
            for (int b = n - 1; b >= 0; b--) {
                if (((v >> b) & 1) == 0) continue;
                if (basis[b] == 0) {
                    basis[b] = v;
                    combo[b] = c;
                    break;
                }
                v ^= basis[b];
                c ^= combo[b];
            }
        }

        unsigned long long t = target, sol = 0;
        for (int b = n - 1; b >= 0; b--) {
            if (((t >> b) & 1) == 0) continue;
            if (basis[b] == 0) return INT_MAX / 4;
            t ^= basis[b];
            sol ^= combo[b];
        }

        std::vector<unsigned long long> nulls;
        for (int i = 0; i < m; i++) {
            unsigned long long v = A[i], c = 1ULL << i;
            for (int b = n - 1; b >= 0; b--) {
                if (((v >> b) & 1) == 0) continue;
                if (basis[b] == 0) break;
                v ^= basis[b];
                c ^= combo[b];
            }
            if (v == 0 && c != 0) nulls.push_back(c);
        }

        std::sort(nulls.begin(), nulls.end());
        nulls.erase(std::unique(nulls.begin(), nulls.end()), nulls.end());

        int best = popcount(sol);
        int k = (int)nulls.size();
        if (k > 26) return best;

        for (int mask = 1; mask < (1 << k); mask++) {
            unsigned long long add = 0;
            for (int i = 0; i < k; i++) if (mask & (1 << i)) add ^= nulls[i];
            best = std::min(best, popcount(sol ^ add));
        }

        return best;
    }

    // ------------------------- Part 2 -------------------------

    struct SubsetInfo {
        unsigned long long maskBits;                 // which buttons are pressed odd number of times
        unsigned long long parityMask;               // bit j = (sum over selected buttons affecting counter j) mod 2
        std::vector<unsigned short> counts;          // exact counts per counter contributed by this subset (0..m)
        int presses;                                 // popcount(maskBits)
    };

    static std::string key_from_j(const std::vector<int>& j) {
        std::string k;
        k.reserve(j.size() * 6);
        for (int x : j) {
            k.append(std::to_string(x));
            k.push_back(',');
        }
        return k;
    }

    static bool all_zero(const std::vector<int>& j) {
        for (int x : j) if (x != 0) return false;
        return true;
    }

    static int fewest_joltage_presses_machine(const std::vector<std::vector<int>>& buttons,
                                              const std::vector<int>& target_j) {
        int n = (int)target_j.size();
        int m = (int)buttons.size();
        if (n == 0) return 0;
        if (m > 63) {
            // This implementation enumerates all subsets of buttons for the parity step, so it needs m<=63 (and practical m<=25ish).
            // If AoC input had huge m, you'd need a different approach.
            return INT_MAX / 4;
        }

        // Build per-button bitmask over counters (for parity) and also adjacency list counts contribution.
        std::vector<unsigned long long> btnMask(m, 0);
        for (int i = 0; i < m; i++) {
            unsigned long long v = 0;
            for (int x : buttons[i]) if (0 <= x && x < n) v |= (1ULL << x);
            btnMask[i] = v;
        }

        // Precompute subset information: for every subset of buttons, compute:
        // - parityMask (XOR of btnMask)
        // - counts per counter (how many selected buttons touch that counter)
        // - presses = popcount(subset)
        //
        // This is O(2^m * n). Works for typical AoC sizes where m is small/moderate.
        const int64_t total = 1LL << m;
        std::vector<SubsetInfo> subsets;
        subsets.reserve((size_t)total);

        // dynamic build: subset s = s without lsb + lsb
        std::vector<unsigned long long> parity(total, 0);
        std::vector<int> pressCount(total, 0);
        std::vector<std::vector<unsigned short>> counts(total);

        counts[0] = std::vector<unsigned short>(n, 0);
        for (int64_t s = 1; s < total; s++) {
            int lsb = __builtin_ctzll((unsigned long long)s);
            int64_t prev = s & (s - 1);
            parity[s] = parity[prev] ^ btnMask[lsb];
            pressCount[s] = pressCount[prev] + 1;
            counts[s] = counts[prev];
            unsigned long long bm = btnMask[lsb];
            while (bm) {
                int bit = __builtin_ctzll(bm);
                counts[s][bit] += 1;
                bm &= (bm - 1);
            }
        }

        for (int64_t s = 0; s < total; s++) {
            SubsetInfo info;
            info.maskBits = (unsigned long long)s;
            info.parityMask = parity[s];
            info.counts = std::move(counts[s]);
            info.presses = pressCount[s];
            subsets.push_back(std::move(info));
        }

        // Memoized recursion by halving trick
        std::unordered_map<std::string, int> memo;
        memo.reserve(1 << 16);

        std::function<int(const std::vector<int>&)> dfs = [&](const std::vector<int>& j) -> int {
            if (all_zero(j)) return 0;
            std::string key = key_from_j(j);
            auto it = memo.find(key);
            if (it != memo.end()) return it->second;

            // Build parity mask of current target (j % 2)
            unsigned long long wantParity = 0;
            for (int i = 0; i < n; i++) {
                if (j[i] & 1) wantParity |= (1ULL << i);
            }

            int best = INT_MAX / 4;

            // Try all subsets whose parity matches wantParity
            for (const auto& sub : subsets) {
                if (sub.parityMask != wantParity) continue;

                // After choosing odd-press buttons sub, remaining must be even and nonnegative:
                // new[i] = (j[i] - sub.counts[i]) / 2, requiring j[i] >= sub.counts[i] and same parity
                std::vector<int> nxt(n, 0);
                bool ok = true;
                for (int i = 0; i < n; i++) {
                    int rem = j[i] - (int)sub.counts[i];
                    if (rem < 0) { ok = false; break; }
                    // parity already matches by construction, but keep this safe:
                    if ((rem & 1) != 0) { ok = false; break; }
                    nxt[i] = rem / 2;
                }
                if (!ok) continue;

                int rec = dfs(nxt);
                if (rec >= INT_MAX / 8) continue;
                int cost = sub.presses + 2 * rec;
                if (cost < best) best = cost;
            }

            memo[key] = best;
            return best;
        };

        return dfs(target_j);
    }

    static int solve_machine_part2(const std::string& line) {
        auto buttons = extract_parens(line);
        auto j = extract_braces_nums(line);
        if (j.empty()) return 0;
        return fewest_joltage_presses_machine(buttons, j);
    }

    // ------------------------- Runner -------------------------

    template <typename T>
    T part1(const std::vector<std::string>& input) {
        long long ans = 0;
        for (const auto& line : input) if (!line.empty()) ans += solve_machine_part1(line);
        return (T)ans;
    }

    template <typename T>
    T part2(const std::vector<std::string>& input) {
        long long ans = 0;
        for (const auto& line : input) if (!line.empty()) ans += solve_machine_part2(line);
        return (T)ans;
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

    Timer::start();
    auto answer1 = sol.part1<long long>(input);
    Log::result("Part 1 Answer: " + std::to_string(answer1));
    Timer::print("Part 1 Execution Time");

    Timer::start();
    auto answer2 = sol.part2<long long>(input);
    Log::result("Part 2 Answer: " + std::to_string(answer2));
    Timer::print("Part 2 Execution Time");

    return 0;
}

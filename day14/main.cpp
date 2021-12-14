#include <aoc.h>

static constexpr const auto Size = size_t(26);
using Map = std::array<std::array<char, Size>, Size>;
using PairCounts = std::array<std::array<size_t, Size>, Size>;

static inline size_t get_result(PairCounts const& pairs, std::string const& seed) {
    auto counts = std::array<size_t, Size>{};
    for (auto c1 = 0; c1 < pairs.size(); c1++) {
        auto& adj = pairs.at(c1);
        for (auto c2 = 0; c2 < adj.size(); c2++) {
            counts.at(c1) += adj.at(c2);
            counts.at(c2) += adj.at(c2);
        }
    }
    counts.at(seed.front() - 'A')++;
    counts.at(seed.back() - 'A')++;

    auto max = std::numeric_limits<size_t>::min();
    auto min = std::numeric_limits<size_t>::max();
    for (auto c : counts) {
        max = std::max(max, c);
        if (c) min = std::min(min, c);
    }
    return (max - min) / 2;
}

static inline void do_round(PairCounts& pairs, Map const& map) {
    auto tmp = PairCounts{};
    for (auto c1 = 0; c1 < pairs.size(); c1++) {
        for (auto c2 = 0; c2 < pairs.at(c1).size(); c2++) {
            if (!pairs.at(c1).at(c2)) continue;
            auto insert = map.at(c1).at(c2);
            auto count = pairs.at(c1).at(c2);
            tmp.at(c1).at(insert - 'A') += count;
            tmp.at(insert - 'A').at(c2) += count;
        }
    }
    pairs = tmp;
}

int main() {
    static const auto split_re = std::regex("\\s+->\\s+");
    auto map = Map{};
    auto pairs = PairCounts{};

    auto line = std::string{};
    if (!std::getline(std::cin, line)) return 1;
    auto tpl = std::string(aoc::trim(line));

    if (!std::getline(std::cin, line)) return 1;
    while (std::getline(std::cin, line)) {
        auto ar = aoc::str_split(aoc::trim(line), split_re);
        if (ar.size() != 2) return 1;
        auto pair = ar.at(0);
        auto insert = ar.at(1);
        if (pair.size() != 2) return 1;
        if (insert.size() != 1) return 1;
        map.at(pair.at(0) - 'A').at(pair.at(1) - 'A') = insert.at(0);
    }

    for (size_t i = 0; i < tpl.size() - 1; i++) {
        auto c1 = tpl.at(i) - 'A';
        auto c2 = tpl.at(i + 1) - 'A';
        pairs.at(c1).at(c2)++;
    }

    auto start = std::chrono::steady_clock::now();
    for (size_t round = 0; round < 10; round++) do_round(pairs, map);
    auto end = std::chrono::steady_clock::now();
    auto part1 = get_result(pairs, tpl);
    auto ns1 = (end - start).count();
    fmt::print("{} ({})\n", part1, ns1);

    start = std::chrono::steady_clock::now();
    for (size_t round = 10; round < 40; round++) do_round(pairs, map);
    auto part2 = get_result(pairs, tpl);
    end = std::chrono::steady_clock::now();
    auto ns2 = (end - start).count();
    fmt::print("{} ({})\n", part2, ns2);

    fmt::print("{}\n", ns1 + ns2);

    return 0;
}

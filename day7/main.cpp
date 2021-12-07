#include <aoc.h>

int main() {
    auto positions = std::vector<int>{};
    auto min = std::numeric_limits<int>::max();
    auto max = std::numeric_limits<int>::min();
    auto line = std::string{};
    if (!std::getline(std::cin, line)) return 1;

    positions.reserve(10000);
    for (auto& p : aoc::str_split(aoc::trim(aoc::to_sv(line)), ',')) {
        positions.push_back(0);
        if (!aoc::from_chars(aoc::trim(p), positions.back())) return 1;
        min = std::min(min, positions.back());
        max = std::max(max, positions.back());
    }

    auto part1 = std::numeric_limits<uint64_t>::max();
    auto part2 = std::numeric_limits<uint64_t>::max();
    for (auto pos = min; pos <= max; pos++) {
        auto p1 = aoc::accumulate(positions, uint64_t(0), [pos](uint64_t s, int p) -> uint64_t {
            return s + std::abs(p - pos);
        });
        auto p2 = aoc::accumulate(positions, uint64_t(0), [pos](uint64_t s, int p) -> uint64_t {
            auto v = std::abs(p - pos);
            return s + v * (v + 1) / 2;
        });
        part1 = std::min(part1, p1);
        part2 = std::min(part2, p2);
    }

    fmt::print("{}\n", part1);
    fmt::print("{}\n", part2);

    return 0;
}

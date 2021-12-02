#include <aoc.h>

int main() {
    std::string line{};
    int64_t x{}, y{}, aim{}, depth{};
    int64_t delta{};

    while (std::getline(std::cin, line)) {
        auto sv = aoc::trim(aoc::to_sv(line));

        if (aoc::substr(sv, 0, 7) == "forward") {
            aoc::from_chars(aoc::substr(sv, 8), delta);
            x += delta;
            depth += aim * delta;
        } else if (aoc::substr(sv, 0, 4) == "down") {
            aoc::from_chars(aoc::substr(sv, 5), delta);
            y += delta;
            aim += delta;
        } else if (aoc::substr(sv, 0, 2) == "up") {
            aoc::from_chars(aoc::substr(sv, 3), delta);
            y -= delta;
            aim -= delta;
        }
    }

    auto part1 = x * y;
    auto part2 = x * depth;

    fmt::print("{}\n", part1);
    fmt::print("{}\n", part2);

    return 0;
}

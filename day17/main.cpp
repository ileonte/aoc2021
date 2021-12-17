#include <aoc.h>

struct Point {
    int x{};
    int y{};
};
struct Rectangle {
    Point top_left{};
    Point bottom_right{};
};
struct Velocity {
    int dx{};
    int dy{};
};

static inline bool inside_rectangle(Point const& p, Rectangle const& r) {
    return (p.x >= r.top_left.x) && (p.y <= r.top_left.y) && (p.x <= r.bottom_right.x) && (p.y >= r.bottom_right.y);
}

static inline bool simulate(Velocity const& v0, Rectangle const& target) {
    auto v = v0;
    auto p = Point{0, 0};
    while ((p.x <= target.bottom_right.x) && (p.y >= target.bottom_right.y)) {
        p.x += v.dx;
        p.y += v.dy;
        if (inside_rectangle(p, target)) return true;
        v.dx = std::max(0, v.dx - 1);
        v.dy -= 1;
    }
    return false;
}

int main() {
    static const auto number_re = std::regex("-?\\d+");

    auto line = std::string{};
    if (!std::getline(std::cin, line)) return 1;

    auto numbers = std::vector<int>{};
    auto r = std::cmatch{};
    auto start = line.c_str();
    auto stop = start + line.size();
    while (std::regex_search(start, stop, r, number_re)) {
        auto v = int{};
        auto sv = aoc::to_sv(r[0]);
        if (!aoc::from_chars(sv, v)) return 1;
        numbers.push_back(v);
        start = r[0].first + r[0].length();
    }
    if (numbers.size() != 4) return 1;

    auto max_x = std::max(numbers[0], numbers[1]);
    auto min_x = std::min(numbers[0], numbers[1]);
    auto max_y = std::max(numbers[2], numbers[3]);
    auto min_y = std::min(numbers[2], numbers[3]);

    auto v_max = std::abs(min_y) - 1;
    auto part1 = v_max * (v_max + 1) / 2;
    fmt::print("{}\n", part1);

    auto target = Rectangle{{min_x, max_y}, {max_x, min_y}};
    auto part2 = size_t{};
    for (int dy = min_y; dy <= v_max; dy++) {
        for (int dx = 1; dx <= max_x; dx++)  {
            part2 += simulate({dx, dy}, target);
        }
    }
    fmt::print("{}\n", part2);

    return 0;
}

#include <aoc.h>

template <typename IntType>
static inline IntType sign(IntType v) {
    return v ? v / std::abs(v) : 0;
}

struct Point {
    int16_t x{};
    int16_t y{};

    bool operator==(Point p) const {
        return (x == p.x) && (y == p.y);
    }

    Point next_towards(Point p) const {
        auto sx = int16_t(sign(p.x - x) + x);
        auto sy = int16_t(sign(p.y - y) + y);
        return {sx, sy};
    }
};
template<>
struct std::hash<Point> {
    size_t operator()(Point const& p) const noexcept {
        return (size_t(p.x) << 16) | size_t(p.y);
    }
};
std::ostream& operator<<(std::ostream& out, Point const& p) {
    out << '{' << p.x << ", " << p.y << '}';
    return out;
}

struct Segment {
    Point p1{};
    Point p2{};

    static bool read(std::istream& in, Segment& ret) {
        static const auto split_re = std::regex("\\s*->\\s*");
        std::string line{};
        if (!std::getline(in, line)) return false;
        auto points_descs = aoc::str_split(aoc::trim(aoc::to_sv(line)), split_re);
        if (points_descs.size() != 2) return false;

        auto coords_desc = aoc::str_split(points_descs[0], ',');
        if (coords_desc.size() != 2) return false;
        if (!aoc::from_chars(coords_desc[0], ret.p1.x)) return false;
        if (!aoc::from_chars(coords_desc[1], ret.p1.y)) return false;

        coords_desc = aoc::str_split(points_descs[1], ',');
        if (coords_desc.size() != 2) return false;
        if (!aoc::from_chars(coords_desc[0], ret.p2.x)) return false;
        if (!aoc::from_chars(coords_desc[1], ret.p2.y)) return false;

        return true;
    }

    bool is_parallel_to_x() const {
        return p1.x == p2.x;
    }
    bool is_parallel_to_y() const {
        return p1.y == p2.y;
    }
    bool is_axis_aligned() const {
        return is_parallel_to_x() || is_parallel_to_y();
    }

    void generate_points_list(std::vector<Point>& ret) const {
        ret.clear();
        for (auto p = p1; ; p = p.next_towards(p2)) {
            ret.push_back(p);
            if (p == p2) break;
        }
    }
};

static inline void map_point(std::unordered_map<Point, uint32_t>& heat_map, Point p) {
    auto it = heat_map.find(p);
    if (it != heat_map.end())
        it->second++;
    else
        heat_map[p] = 1;
}

int main() {
    Segment seg{};
    std::unordered_map<Point, uint32_t> heat_map{}, parallel_heat_map{};
    std::vector<Point> points{};

    points.reserve(1000);
    heat_map.reserve(1000000);
    parallel_heat_map.reserve(1000000);

    while (Segment::read(std::cin, seg)) {
        seg.generate_points_list(points);
        for (auto& p : points) {
            map_point(heat_map, p);
            if (seg.is_axis_aligned())
                map_point(parallel_heat_map, p);
        }
    }

    auto part1 = aoc::accumulate(parallel_heat_map, size_t(0), [](size_t s, std::pair<Point, uint32_t> const& p) -> size_t {
        return (p.second > 1) ? s + 1 : s;
    });
    auto part2 = aoc::accumulate(heat_map, size_t(0), [](size_t s, std::pair<Point, uint32_t> const& p) -> size_t {
        return (p.second > 1) ? s + 1 : s;
    });

    fmt::print("{}\n", part1);
    fmt::print("{}\n", part2);

    return 0;
}

#include <aoc.h>

static constexpr const auto marker_fold_x = std::string_view("fold along x=");
static constexpr const auto marker_fold_y = std::string_view("fold along y=");

using Points = std::unordered_set<uint32_t>;

static inline uint32_t pack(uint16_t x, uint16_t y) {
    return (uint32_t(x) << 16) | uint32_t(y);
}

static inline std::pair<uint16_t, uint16_t> unpack(uint32_t p) {
    return {uint16_t(p >> 16), uint16_t(p & 0x0000ffff)};
}

static inline void fold_x(Points& points, uint16_t fold) {
    auto to_remove = std::vector<uint32_t>{};
    auto to_add = std::vector<uint32_t>{};

    to_remove.reserve(1024);
    to_add.reserve(1024);

    for (auto existing_point : points) {
        auto [existing_x, existing_y] = unpack(existing_point);
        if (existing_x <= fold) continue;
        auto new_x = fold - (existing_x - fold);
        auto new_point = pack(new_x, existing_y);
        to_remove.push_back(existing_point);
        to_add.push_back(new_point);
    }

    for (auto p : to_remove) points.erase(p);
    for (auto p : to_add) points.insert(p);
}

static inline void fold_y(Points& points, uint16_t fold) {
    auto to_remove = std::vector<uint32_t>{};
    auto to_add = std::vector<uint32_t>{};

    to_remove.reserve(1024);
    to_add.reserve(1024);

    for (auto existing_point : points) {
        auto [existing_x, existing_y] = unpack(existing_point);
        if (existing_y <= fold) continue;
        auto new_y = fold - (existing_y - fold);
        auto new_point = pack(existing_x, new_y);
        to_remove.push_back(existing_point);
        to_add.push_back(new_point);
    }

    for (auto p : to_remove) points.erase(p);
    for (auto p : to_add) points.insert(p);
}

static inline void draw_points(Points const& points) {
    auto min_x = std::numeric_limits<uint16_t>::max();
    auto min_y = std::numeric_limits<uint16_t>::max();
    auto max_x = std::numeric_limits<uint16_t>::min();
    auto max_y = std::numeric_limits<uint16_t>::min();

    for (auto p : points) {
        auto [x, y] = unpack(p);
        min_x = std::min(min_x, x);
        max_x = std::max(max_x, x);
        min_y = std::min(min_y, y);
        max_y = std::max(max_y, y);
    }

    auto w = size_t((max_x - min_x) + 1);
    auto h = size_t((max_y - min_y) + 1);
    auto canvas = std::vector<char>(w * h, ' ');
    for (auto p : points) {
        auto [x, y] = unpack(p);
        canvas.at((y - min_y) * w + (x - min_x)) = '#';
    }

    for (size_t y = 0; y < h; y++) {
        auto sv = std::string_view(canvas.data() + y * w, w);
        fmt::print("{}\n", sv);
    }
}

int main() {
    auto points = std::unordered_set<uint32_t>{};
    auto line = std::string{};

    auto part1 = size_t{};
    while (std::getline(std::cin, line)) {
        auto sv = aoc::trim(aoc::to_sv(line));
        if (sv.empty()) continue;
        if (sv.starts_with(marker_fold_x)) {
            auto v = uint16_t{};
            if (!aoc::from_chars(aoc::substr(sv, marker_fold_x.size()), v)) return 1;
            fold_x(points, v);
            if (!part1) part1 = points.size();
        } else if (sv.starts_with(marker_fold_y)) {
            auto v = uint16_t{};
            if (!aoc::from_chars(aoc::substr(sv, marker_fold_x.size()), v)) return 1;
            fold_y(points, v);
            if (!part1) part1 = points.size();
        } else {
            auto ar = aoc::str_split(sv, ',');
            if (ar.size() != 2) return 1;
            auto x = uint16_t{};
            auto y = uint16_t{};
            if (!aoc::from_chars(aoc::trim(ar.at(0)), x)) return 1;
            if (!aoc::from_chars(aoc::trim(ar.at(1)), y)) return 1;
            points.insert(pack(x, y));
        }
    }

    fmt::print("{}\n", part1);
    draw_points(points);

    return 0;
}

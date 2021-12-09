#include <aoc.h>

static inline unsigned char sample(std::vector<char> const& map, size_t width, int x, int y) {
    if ((x >= 0) && (y >= 0) && (size_t(x) < width) && (size_t(y) < (map.size() / width)))
        return map.at(y * width + x);
    return 255;
}

static inline size_t flood_fill(std::vector<char>& map, size_t width, int x, int y) {
    auto v = sample(map, width, x, y);
    if (!v || v >= '9') return 0;
    map.at(y * width + x) = '9';
    return 1
        + flood_fill(map, width, x, y - 1)
        + flood_fill(map, width, x, y + 1)
        + flood_fill(map, width, x - 1, y)
        + flood_fill(map, width, x + 1, y);
}

int main() {
    auto line = std::string{};
    auto map = std::vector<char>{};

    std::getline(std::cin, line);
    auto sv = aoc::trim(line);
    auto width = sv.size();

    map.insert(map.end(), sv.begin(), sv.end());
    while (std::getline(std::cin, line)) {
        sv = aoc::trim(line);
        map.insert(map.end(), sv.begin(), sv.end());
    }

    auto sizes = std::vector<size_t>{};
    sizes.reserve(1000);

    auto part1 = size_t{};
    for (int y = 0; y < int(map.size() / width); y++) {
        for (int x = 0; x < int(width); x++) {
            auto top = sample(map, width, x, y - 1);
            auto bot = sample(map, width, x, y + 1);
            auto left = sample(map, width, x - 1, y);
            auto right = sample(map, width, x + 1, y);
            auto v = sample(map, width, x, y);

            if ((v < top) && (v < bot) && (v < left) && (v < right)) {
                part1 += (size_t(v - '0') + 1);
                sizes.push_back(flood_fill(map, width, x, y));
            }
        }
    }

    std::sort(sizes.begin(), sizes.end(), std::greater<size_t>{});

    fmt::print("{}\n", part1);
    fmt::print("{}\n", sizes[0] * sizes[1] * sizes[2]);

    return 0;
}

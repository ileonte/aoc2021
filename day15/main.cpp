#include <aoc.h>

struct Position {
    uint32_t data{};

    static inline Position pack(uint16_t x, uint16_t y) {
        return Position {
            .data = (uint32_t(x) << 16) | uint32_t(y)
        };
    }

    inline std::pair<uint16_t, uint16_t> unpack() const {
        return { data >> 16, data & 0x0000ffff };
    }

    inline void neighbors(uint16_t mx, uint16_t my, std::vector<Position>& ret) const {
        ret.clear();
        auto [x, y] = unpack();
        if (y < my - 1) ret.push_back(pack(x, y + 1));
        if (x < mx - 1) ret.push_back(pack(x + 1, y));
        if (y) ret.push_back(pack(x, y - 1));
        if (x) ret.push_back(pack(x - 1, y));
    }

    inline bool operator==(Position const& other) const {
        return data == other.data;
    }
};

struct Map {
    std::vector<char> data{};
    size_t width{};
    size_t height{};

    static bool read(std::istream& in, Map& ret) {
        ret.data.clear();

        auto line = std::string{};
        if (!std::getline(in, line)) return false;
        auto sv = aoc::trim(line);
        auto w = sv.size();
        auto h = sv.size();
        ret.data.resize(w * h);
        for (size_t l = 0; l < h; l++) {
            auto lptr = ret.data.data() + l * w;
            for (size_t i = 0; i < w; i++) {
                lptr[i] = sv[i];
            }
            std::getline(in, line);
            sv = aoc::trim(line);
        }

        ret.width = w;
        ret.height = h;

        return true;
    }

    void print() const {
        for (size_t y = 0; y < height; y++) {
            fmt::print("'");
            for (size_t x = 0; x < width / 10; x++) {
                auto sv = std::string_view(data.data() + y * width + x * 10, 10);
                fmt::print("{} ", sv);
            }
            fmt::print("'\n");
        }
    }

    void enlarge(size_t factor) {
        auto nw = width * factor;
        auto nh = height * factor;
        auto nd = std::vector<char>(nw * nh, ' ');

        for (size_t y = 0; y < height; y++) {
            for (size_t x = 0; x < width; x++) {
                auto& src = data[y * width + x];

                for (size_t f = 0; f < factor; f++) {
                    auto& dst = nd[y * nw + f * width + x];
                    auto val = src + f;
                    if (val > '9') val -= 9;
                    dst = val;
                }
            }
        }

        for (size_t y = 0; y < height; y++) {
            for (size_t f = 1; f < factor; f++) {
                for (size_t x = 0; x < nw; x++) {
                    auto val = nd[y * nw + x] + f;
                    if (val > '9') val -= 9;
                    nd[(f * height + y) * nw + x] = val;
                }
            }
        }

        data = nd;
        width = nw;
        height = nh;
    }

    size_t shortest_path_cost() {
        auto costs = std::vector<size_t>(width * height, std::numeric_limits<size_t>::max());
        auto neigh = std::vector<Position>{};
        auto candidates = std::queue<Position>{};

        neigh.reserve(8);

        costs[0] = 0;
        candidates.push(Position::pack(0, 0));
        while (!candidates.empty()) {
            auto cpos = candidates.front();
            auto [cx, cy] = cpos.unpack();
            candidates.pop();

            cpos.neighbors(width, height, neigh);
            for (auto p : neigh) {
                auto [px, py] = p.unpack();
                auto current_n_cost = costs[py * width + px];
                auto new_n_cost = costs[cy * width + cx] + position_cost(p);
                if (current_n_cost > new_n_cost) {
                    costs[py * width + px] = new_n_cost;
                    candidates.push(p);
                }
            }
        }

        return costs.back();
    }

private:
    inline size_t position_cost(Position p) const {
        auto [x, y] = p.unpack();
        return (data[y * width + x]  - '0');
    }
};

int main() {
    Map map{};

    if (!Map::read(std::cin, map)) return 1;

    auto start = std::chrono::steady_clock::now();
    auto part1 = map.shortest_path_cost();
    auto end = std::chrono::steady_clock::now();
    auto ns1 = (end - start).count();
    fmt::print("{} ({})\n", part1, ns1);

    map.enlarge(5);
    start = std::chrono::steady_clock::now();
    auto part2 = map.shortest_path_cost();
    end = std::chrono::steady_clock::now();
    auto ns2 = (end - start).count();
    fmt::print("{} ({})\n", part2, ns2);

    fmt::print("{}\n", ns1 + ns2);

    return 0;
}

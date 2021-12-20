#include <aoc.h>

struct Delta {
    int dx{};
    int dy{};
};

struct Position {
    int x{};
    int y{};

    inline Position operator+(Delta d) {
        return {x + d.dx, y + d.dy};
    }
    inline bool operator==(Position p) const {
        return (x == p.x) && (y == p.y);
    }
    inline bool operator<(Position const& other) const {
        if (y < other.y) return true;
        if ((y == other.y) && (x < other.x)) return true;
        return false;
    }
};
namespace std {
    template<>
    struct hash<Position> {
        size_t operator()(Position const& p) const {
            return size_t(size_t(uint32_t(p.x)) << 32) | size_t(uint32_t(p.y));
        }
    };
}
std::ostream& operator<<(std::ostream& out, Position const& p) {
    return out << "{" << p.x << "," << p.y << "}";
}

using Board = std::unordered_set<Position>;

inline size_t bit_value(Board const& board, Position p, int minx, int miny, int maxx, int maxy, size_t void_value) {
    if ((p.x < minx) || (p.x > maxx) || (p.y < miny) || (p.y > maxy))
        return void_value ? 1 : 0;
    return board.contains(p) ? 1 : 0;
}

inline size_t get_index(Board const& board, Position center, int minx, int miny, int maxx, int maxy, size_t void_value) {
    auto ret = size_t{};

    auto p1 = center + Delta{-1, -1};
    auto p2 = center + Delta{ 0, -1};
    auto p3 = center + Delta{ 1, -1};
    auto p4 = center + Delta{-1,  0};
    auto p5 = center + Delta{ 1,  0};
    auto p6 = center + Delta{-1,  1};
    auto p7 = center + Delta{ 0,  1};
    auto p8 = center + Delta{ 1,  1};

    ret = (ret << 1) | bit_value(board, p1, minx, miny, maxx, maxy, void_value);
    ret = (ret << 1) | bit_value(board, p2, minx, miny, maxx, maxy, void_value);
    ret = (ret << 1) | bit_value(board, p3, minx, miny, maxx, maxy, void_value);
    ret = (ret << 1) | bit_value(board, p4, minx, miny, maxx, maxy, void_value);
    ret = (ret << 1) | bit_value(board, center, minx, miny, maxx, maxy, void_value);
    ret = (ret << 1) | bit_value(board, p5, minx, miny, maxx, maxy, void_value);
    ret = (ret << 1) | bit_value(board, p6, minx, miny, maxx, maxy, void_value);
    ret = (ret << 1) | bit_value(board, p7, minx, miny, maxx, maxy, void_value);
    ret = (ret << 1) | bit_value(board, p8, minx, miny, maxx, maxy, void_value);

    return ret;
}

static inline void run_algorightm(Board& board, std::string const& algorithm, int& minx, int& miny, int& maxx, int& maxy, size_t& void_value) {
    auto new_board = Board{};
    auto new_minx = minx;
    auto new_miny = miny;
    auto new_maxx = maxx;
    auto new_maxy = maxy;

    for (int y = miny - 1; y <= maxy + 1; y++) {
        for (int x = minx - 1; x <= maxx + 1; x++) {
            auto p = Position{x, y};
            auto i = get_index(board, p, minx, miny, maxx, maxy, void_value);
            auto c = algorithm[i];
            if (c == '#') {
                new_minx = std::min(new_minx, x);
                new_miny = std::min(new_miny, y);
                new_maxx = std::max(new_maxx, x);
                new_maxy = std::max(new_maxy, y);
                new_board.insert(p);
            }
        }
    }

    void_value = (algorithm[get_index(board, {-2, -2}, 0, 0, 0, 0, void_value)] == '#');
    board = std::move(new_board);
    minx = new_minx;
    miny = new_miny;
    maxx = new_maxx;
    maxy = new_maxy;
}

static inline void print(Board const& board) {
    auto minx = std::numeric_limits<int>::max();
    auto miny = std::numeric_limits<int>::max();
    auto maxx = std::numeric_limits<int>::min();
    auto maxy = std::numeric_limits<int>::min();
    for (auto& p : board) {
        minx = std::min(minx, p.x);
        miny = std::min(miny, p.y);
        maxx = std::max(maxx, p.x);
        maxy = std::max(maxy, p.y);
    }

    auto w = maxx - minx + 1;
    auto h = maxy - miny + 1;
    auto d = std::vector<char>(w * h, '.');
    for (auto p : board) {
        p = p + Delta{-minx, -miny};
        d[p.y * w + p.x] = '#';
    }
    for (int l = 0; l < h; l++) {
        auto sv = std::string_view(d.data() + l * w, w);
        fmt::print("{}\n", sv);
    }
    fmt::print("\n");
}

int main() {
    auto algorithm = std::string{};
    auto board = Board{};

    auto line = std::string{};
    while (std::getline(std::cin, line)) {
        auto sv = aoc::trim(line);
        if (sv.empty()) break;
        algorithm.append(sv);
    }

    auto lno = 0;
    auto line_width = size_t{};
    while (std::getline(std::cin, line)) {
        auto sv = aoc::trim(line);
        if (line_width) {
            if (sv.size() != line_width) return 1;
        }
        line_width = sv.size();
        for (int cno = 0; cno < int(sv.size()); cno++) {
            if (sv[cno] == '#') board.insert(Position {cno, lno});
        }
        lno++;
    }

    auto minx = 0;
    auto miny = 0;
    auto maxx = lno - 1;
    auto maxy = int(line_width) - 1;
    auto void_value = size_t{};

    for (size_t i = 0; i < 50; i++) {
        run_algorightm(board, algorithm, minx, miny, maxx, maxy, void_value);
        if (i == 1) fmt::print("{}\n", board.size());
    }

    fmt::print("{}\n", board.size());

    return 0;
}

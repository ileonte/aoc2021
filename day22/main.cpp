#include <aoc.h>

struct Segment1D {
    constexpr Segment1D() = default;
    constexpr Segment1D(Segment1D const&) = default;
    constexpr Segment1D(int n1, int n2) : c1{std::min(n1, n2)}, c2{std::max(n1, n2)} {}

    inline bool empty() const { return (c1 == INT_MAX) || (c2 == INT_MIN); }

    friend inline std::ostream& operator<<(std::ostream& out, Segment1D const& s) {
        return out << "{" << s.c1 << ", " << s.c2 << "}";
    }

    inline Segment1D& translate(int t) {
        c1 += t;
        c2 += t;
        return *this;
    }

    inline size_t len() const {
        return (c2 - c1) + 1;
    }

    inline Segment1D intersection(Segment1D const& other) const {
        auto c1 = std::max(this->c1, other.c1);
        auto c2 = std::min(this->c2, other.c2);
        return (c1 <= c2) ? Segment1D{c1, c2} : Segment1D{};
    }

private:
    int c1{INT_MAX}, c2{INT_MIN};
};

struct AABox {
    AABox() = default;
    AABox(AABox const&) = default;
    AABox(Segment1D const& sx, Segment1D const& sy, Segment1D const& sz) : x{sx}, y{sy}, z{sz} {}

    inline bool empty() const { return x.empty() || y.empty() || z.empty(); }

    friend inline std::ostream& operator<<(std::ostream& out, AABox const& b) {
        fmt::print(out, "{{{} {} {}}}", b.x, b.y, b.z);
        return out;
    }

    inline AABox intersection(AABox const& other) const {
        return {
            x.intersection(other.x),
            y.intersection(other.y),
            z.intersection(other.z),
        };
    }

    inline void carve_out(AABox const& other) {
        if (auto hi = intersection(other); !hi.empty()) {
            for (auto& h : holes) h.carve_out(hi);
            holes.push_back(hi);
        }
    }

    inline size_t volume() const {
        auto box_volume = x.len() * y.len() * z.len();
        auto hole_volume = aoc::accumulate(holes, size_t(0), [](size_t s, AABox const& b) {
            return s + b.volume();
        });
        return box_volume - hole_volume;
    }

private:
    Segment1D x{}, y{}, z{};
    std::vector<AABox> holes{};
};

struct Instruction {
    AABox box{};
    bool on{};

    static inline bool from_string(std::string_view sv, Instruction& ret) {
        static const auto re = std::regex(
            "^((?:on)|(?:off))[^-\\d]+(-?\\d+)[^-\\d]+(-?\\d+)[^-\\d]+(-?\\d+)[^-\\d]+(-?\\d+)[^-\\d]+(-?\\d+)[^-\\d]+(-?\\d+)$",
            std::regex_constants::ECMAScript);

        auto m = std::cmatch{};
        if (!std::regex_match(sv.begin(), sv.end(), m, re)) return false;

        auto type = aoc::to_sv(m[1]);
        if (type.length() < 2) return false;

        auto minx{0}, maxx{0}, miny{0}, maxy{0}, minz{0}, maxz{0};
        if (!aoc::from_chars(aoc::to_sv(m[2]), minx)) return false;
        if (!aoc::from_chars(aoc::to_sv(m[3]), maxx)) return false;
        if (!aoc::from_chars(aoc::to_sv(m[4]), miny)) return false;
        if (!aoc::from_chars(aoc::to_sv(m[5]), maxy)) return false;
        if (!aoc::from_chars(aoc::to_sv(m[6]), minz)) return false;
        if (!aoc::from_chars(aoc::to_sv(m[7]), maxz)) return false;

        ret.on = (type[1] == 'n');
        ret.box = {{minx, maxx}, {miny, maxy}, {minz, maxz}};
        return true;
    }
};

static inline size_t run(std::vector<Instruction> const& instructions, AABox bounding_box = AABox{}) {
    auto ret = size_t{};
    auto boxes = std::vector<AABox>{};
    boxes.reserve(instructions.size());

    for (auto& i : instructions) {
        if (auto nb = bounding_box.empty() ? i.box : bounding_box.intersection(i.box); !nb.empty()) {
            for (auto& b : boxes) b.carve_out(nb);
            if (i.on) boxes.push_back(nb);
        }
    }

    return aoc::accumulate(boxes, size_t{}, [](size_t s, AABox const& b) { return s + b.volume(); });
}

int main() {
    auto instructions = std::vector<Instruction>{};
    instructions.reserve(1000);

    auto line = std::string{};
    while (std::getline(std::cin, line)) {
        auto i = Instruction{};
        if (!Instruction::from_string(aoc::trim(line), i)) return 1;
        instructions.emplace_back(std::move(i));
    }

    auto part1 = run(instructions, {{-50, 50}, {-50, 50}, {-50, 50}});
    fmt::print("{}\n", part1);

    auto part2 = run(instructions);
    fmt::print("{}\n", part2);

    return 0;
}

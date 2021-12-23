#include <aoc_glm.h>

struct Orientation {
    glm::ivec3 xa{};
    glm::ivec3 ya{};
    glm::ivec3 za{};
    glm::mat<3, 3, int> rotation{};
};
namespace fmt {
    template <>
    struct formatter<Orientation> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(Orientation const& o, FormatContext &ctx) {
            return format_to(ctx.out(), "{{{}, {}, {}, {}}}", o.xa, o.ya, o.za, o.rotation);
        }
    };
}

static constexpr const auto directions = std::array<glm::vec3, 6>{{
    {1, 0, 0}, {-1, 0, 0},
    {0, 1, 0}, {0, -1, 0},
    {0, 0, 1}, {0, 0, -1},
}};
// static constexpr const auto orientations = std::array<Orientation, 24>{{
//     {{1,0,0}, {0,1,0}, {{1,0,0},{0,1,0},{0,0,1}}},
//     {{1,0,0}, {0,0,-1}, {{1,0,0},{0,0,1},{0,-1,0}}},
//     {{1,0,0}, {0,-1,0}, {{1,0,0},{0,-1,0},{0,0,-1}}},
//     {{1,0,0}, {0,0,1}, {{1,0,0},{0,0,-1},{0,1,0}}},
//     {{-1,0,0}, {0,1,0}, {{1,0,0},{0,1,0},{0,0,1}}},
//     {{-1,0,0}, {0,0,1}, {{1,0,0},{0,0,-1},{0,1,0}}},
//     {{-1,0,0}, {0,-1,0}, {{1,0,0},{0,-1,0},{0,0,-1}}},
//     {{-1,0,0}, {0,0,-1}, {{1,0,0},{0,0,1},{0,-1,0}}},
//     {{0,1,0}, {0,1,0}, {{1,0,0},{0,1,0},{0,0,1}}},
//     {{0,1,0}, {0,0,0}, {{0,0,-1},{0,1,0},{1,0,0}}},
//     {{0,1,0}, {0,1,0}, {{-1,0,0},{0,1,0},{0,0,-1}}},
//     {{0,1,0}, {0,1,0}, {{0,0,1},{0,1,0},{-1,0,0}}},
//     {{0,-1,0}, {0,1,0}, {{1,0,0},{0,1,0},{0,0,1}}},
//     {{0,-1,0}, {0,0,0}, {{0,0,1},{0,1,0},{-1,0,0}}},
//     {{0,-1,0}, {0,1,0}, {{-1,0,0},{0,1,0},{0,0,-1}}},
//     {{0,-1,0}, {0,1,0}, {{0,0,-1},{0,1,0},{1,0,0}}},
//     {{0,0,1}, {0,1,0}, {{1,0,0},{0,1,0},{0,0,1}}},
//     {{0,0,1}, {1,0,0}, {{0,1,0},{-1,0,0},{0,0,1}}},
//     {{0,0,1}, {0,-1,0}, {{-1,0,0},{0,-1,0},{0,0,1}}},
//     {{0,0,1}, {-1,0,0}, {{0,-1,0},{1,0,0},{0,0,1}}},
//     {{0,0,-1}, {0,1,0}, {{1,0,0},{0,1,0},{0,0,1}}},
//     {{0,0,-1}, {-1,0,0}, {{0,-1,-0},{1,0,0},{0,0,1}}},
//     {{0,0,-1}, {0,-1,0}, {{-1,0,0},{0,-1,0},{0,0,1}}},
//     {{0,0,-1}, {1,0,0}, {{0,1,0},{-1,0,0},{0,0,1}}},
// }};
static auto orientations = std::vector<Orientation>{};

static inline void gen_orientations() {
    for (auto& fwd : directions) {
        for (auto& up : directions) {
            if ((fwd == up) || (fwd == -up)) continue;
            auto za = glm::ivec3(glm::cross(fwd, up));

            auto rot = glm::mat<3, 3, int>(
                fwd.x, up.x, za.x,
                fwd.y, up.y, za.y,
                fwd.z, up.z, za.z
            );
            orientations.push_back(Orientation {
                .xa = fwd,
                .ya = up,
                .za = za,
                .rotation = rot
            });
        }
    }
}

struct Scanner {
    size_t id{};
    glm::ivec3 position{};
    std::vector<glm::ivec3> relative_beacons{};
    std::vector<glm::ivec3> beacons{};
    size_t next_orientation_id{};

    static inline bool from_stream(std::istream& in, size_t id, Scanner& ret) {
        auto line = std::string{};
        while (std::getline(in, line)) {
            auto sv = aoc::trim(line);
            if (sv.empty()) break;
            auto v = glm::ivec3{};
            auto parts = aoc::str_split(sv, ',');
            if (parts.size() != 3) return false;
            if (!aoc::from_chars(parts[0], v.x)) return false;
            if (!aoc::from_chars(parts[1], v.y)) return false;
            if (!aoc::from_chars(parts[2], v.z)) return false;
            ret.relative_beacons.push_back(v);
        }

        ret.id = id;

        return true;
    }

    bool next_orientation() {
        if (next_orientation_id >= orientations.size()) {
            next_orientation_id = 0;
            return false;
        }

        beacons.reserve(relative_beacons.size());
        beacons.clear();

        auto& o = orientation();
        for (auto& b : relative_beacons)
            beacons.push_back(b * o.rotation);

        next_orientation_id++;
        return true;
    }

    size_t orientation_id() const {
        return next_orientation_id ? next_orientation_id - 1 : orientations.size() - 1;
    }
    Orientation const& orientation() const {
        return orientations[orientation_id()];
    }

    bool matches(Scanner const& other) const {
        auto m = std::vector<glm::ivec3>{};
        m.reserve(12);

        for (auto& ours : beacons) {
            for (auto& theirs : other.beacons) {
                if (ours != theirs) continue;
                m.push_back(ours);
                break;
            }
            if (m.size() == 12) {
                fmt::print("{}\n", m);
                break;
            }
        }

        return m.size() == 12;
    }
};

int main() {
    auto scanners = std::vector<Scanner>{};

    auto line = std::string{};
    while (std::getline(std::cin, line)) {
        auto scanner = Scanner{};
        scanner.beacons.reserve(26);
        if (!Scanner::from_stream(std::cin, scanners.size(), scanner)) return 1;
        scanners.push_back(std::move(scanner));
    }

    gen_orientations();

    auto p = glm::ivec3(1, 2, 3);
    for (auto& o : orientations)
        fmt::print("{}\n", p * o.rotation);

    // auto& s1 = scanners[0];
    // auto& s2 = scanners[1];

    // s1.next_orientation();
    // while (s2.next_orientation()) {
    //     fmt::print("{}\n", s2.beacons);
    //     if (s1.matches(s2)) {
    //         fmt::print("MATCH at orientation {}!\n", s2.orientation_id());
    //         break;
    //     }
    // }

    return 0;
}

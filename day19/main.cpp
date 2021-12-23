#include <aoc_glm.h>

using Orientation = glm::mat<3, 3, int>;
static constexpr auto orientations = []() -> std::array<Orientation, 24> {
    constexpr const auto directions = std::array<glm::vec3, 6>{{
        {1, 0, 0}, {-1, 0, 0},
        {0, 1, 0}, {0, -1, 0},
        {0, 0, 1}, {0, 0, -1},
    }};

    auto ret = std::array<Orientation, 24>{};
    auto idx = size_t{};
    for (auto& xa : directions) {
        for (auto& ya : directions) {
            if ((xa == ya) || (xa == -ya)) continue;
            auto za = glm::cross(xa, ya);
            ret[idx++] = {xa, ya, za};
        }
    }
    return ret;
}();

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
        ret.beacons.reserve(ret.relative_beacons.size());

        return true;
    }

    bool next_orientation() {
        if (next_orientation_id >= orientations.size()) {
            next_orientation_id = 0;
            return false;
        }

        beacons.clear();

        auto& o = orientations[next_orientation_id++];
        for (auto& b : relative_beacons)
            beacons.push_back(b * o);
        return true;
    }

    bool matches(Scanner& other) const {
        for (auto& ours : beacons) {
            for (auto& theirs : other.beacons) {
                if (count_matches(other, theirs - ours) >= 12) {
                    other.position = position + ours - theirs;
                    return true;
                }
            }
        }
        return false;
    }

private:
    size_t count_matches(Scanner const& other, glm::ivec3 translation) const {
        auto ret = size_t{};
        for (auto& p : other.beacons) {
            auto t = p - translation;
            if (std::find(beacons.begin(), beacons.end(), t) != beacons.end()) {
                ret++;
                if (ret >= 12) break;
            }
        }
        return ret;
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

    auto unsolved = std::deque<Scanner*>{};
    for (size_t i = 1; i < scanners.size(); i++) unsolved.push_back(&scanners[i]);

    auto solved = std::vector<Scanner*>{};
    solved.push_back(&scanners[0]);

    solved[0]->next_orientation();
    while (!unsolved.empty()) {
        auto current = unsolved.front();
        unsolved.pop_front();

        auto matched = false;
        while (!matched && current->next_orientation()) {
            for (auto anchor : solved) {
                if (!anchor->matches(*current)) continue;
                matched = true;
                break;
            }
        }
        if (!matched) unsolved.push_back(current);
        else solved.push_back(current);
    }

    auto unique = std::vector<glm::ivec3>{};
    for (auto s : solved) {
        for (auto& p : s->beacons) {
            auto t = p + s->position;
            if (std::find(unique.begin(), unique.end(), t) == unique.end())
                unique.push_back(t);
        }
    }
    fmt::print("{}\n", unique.size());

    auto part2 = size_t{};
    for (auto s1 : solved) {
        auto p1 = s1->position;
        for (auto s2 : solved) {
            auto p2 = s2->position;
            part2 = std::max(part2, size_t(std::abs(p2.x - p1.x) + std::abs(p2.y - p1.y) + std::abs(p2.z - p1.z)));
        }
    }
    fmt::print("{}\n", part2);

    return 0;
}

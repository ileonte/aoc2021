#include <aoc.h>

#if !defined(NON_STUPID_SOLUTION)

/* this is the initial thing I wrote, which is pretty dumb */

struct Fish {
    size_t age{};
    size_t count{};
};
std::ostream& operator<<(std::ostream& out, Fish const& f) {
    return out << '{' << f.age << ", " << f.count << '}';
}

static inline void collapse(std::vector<Fish>& fish) {
    std::vector<Fish> shadow_fish{};
    shadow_fish.reserve(fish.size());
    for (auto& f : fish) {
        auto it = std::find_if(std::begin(shadow_fish), std::end(shadow_fish), [&f](Fish const& sf) -> bool {
            return f.age == sf.age;
        });
        if (it != std::end(shadow_fish)) {
            it->count += f.count;
        } else {
            shadow_fish.push_back(f);
        }
    }
    fish = std::move(shadow_fish);
}

static inline void simulate(std::vector<Fish>& fish) {
    auto nf = Fish {
        .age = 8,
        .count = 0,
    };
    bool spawned{false};
    for (auto& f : fish) {
        if (!f.age) {
            spawned = true;
            nf.count += f.count;
            f.age = 6;
        } else {
            f.age -= 1;
        }
    }
    if (spawned) fish.push_back(nf);

    collapse(fish);
}

int main() {
    std::vector<Fish> fish{};
    std::string line{};
    if (!std::getline(std::cin, line)) return 1;
    for (auto p : aoc::str_split(aoc::trim(aoc::to_sv(line)), ',')) {
        size_t v{};
        if (!aoc::from_chars(p, v)) return 1;
        auto it = std::find_if(std::begin(fish), std::end(fish), [v](Fish const& f) -> bool { return f.age == v; });
        if (it != std::end(fish)) {
            it->count++;
        } else {
            fish.push_back(Fish {
                .age = v,
                .count = 1,
            });
        }
    }

    auto ns = aoc::time_call([&fish]() {
        for (size_t i = 0; i < 80; i++)
            simulate(fish);
        auto part1 = aoc::accumulate(fish, size_t(0), [](size_t s, Fish const& f) -> size_t {
            return s + f.count;
        });
        fmt::print("{}\n", part1);

        for (size_t i = 80; i < 256; i++)
            simulate(fish);
        auto part2 = aoc::accumulate(fish, size_t(0), [](size_t s, Fish const& f) -> size_t {
            return s + f.count;
        });
        fmt::print("{}\n", part2);
    });

    fmt::print("{}\n", ns.count());

    return 0;
}

#else

/* actually good solution */

size_t counts[9]{};

static inline void simulate2() {
    auto zc = counts[0];

    for (size_t i = 1; i < 9; i++) {
        counts[i - 1] += counts[i];
        counts[i] = 0;
    }

    counts[8] = zc;
    counts[6] += zc;
    counts[0] -= zc;
}

int main() {
    std::string line{};
    if (!std::getline(std::cin, line)) return 1;
    for (auto p : aoc::str_split(aoc::trim(aoc::to_sv(line)), ',')) {
        size_t v{};
        if (!aoc::from_chars(p, v)) return 1;
        counts[v]++;
    }

    auto ns = aoc::time_call([]() {
        for (size_t i = 0; i < 80; i++) simulate2();
        auto part1 = aoc::accumulate(counts, size_t(0), [](size_t s, size_t c) -> size_t { return s + c; });
        fmt::print("{}\n", part1);

        for (size_t i = 80; i < 256; i++) simulate2();
        auto part2 = aoc::accumulate(counts, size_t(0), [](size_t s, size_t c) -> size_t { return s + c; });
        fmt::print("{}\n", part2);
    });

    fmt::print("{}\n", ns.count());

    return 0;
}

#endif

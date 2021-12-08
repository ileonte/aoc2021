#include <aoc.h>

struct DigitEncoding {
    uint8_t data[7]{};
};
std::ostream& operator<<(std::ostream& out, DigitEncoding const& de) {
    out << '<';
    for (size_t i = 0; i < 7; i++) {
        out << ((de.data[i] != 0) ? char('a' + i) : char(' '));
    }
    return out << '>';
}

static inline DigitEncoding substract(DigitEncoding const& e1, DigitEncoding const& e2) {
    DigitEncoding ret{};
    for (size_t i = 0; i < 7; i++) {
        auto v1 = (e1.data[i] != 0);
        auto v2 = (e2.data[i] != 0);
        ret.data[i] = v1 ? v1 - v2 : 0;
    }
    return ret;
}

static inline bool contains(DigitEncoding const& haystack, DigitEncoding const& needle) {
    for (size_t i = 0; i < 7; i++) {
        if ((needle.data[i] != 0) && (haystack.data[i] == 0)) return false;
    }
    return true;
}

static inline DigitEncoding parse_encoding(std::string_view sv) {
    DigitEncoding ret{};
    for (auto c : sv) {
        if (c >= 'a' && c <= 'g') ret.data[c - 'a'] = 1;
    }
    return ret;
}

static inline bool equal(DigitEncoding const& e1, DigitEncoding const& e2) {
    return std::memcmp(e1.data, e2.data, sizeof(e1.data)) == 0;
}

/*
 * Notation:
 *  - `sX` - segment X, as shown in the diagram below:
 *       0000
 *      1    2
 *      1    2
 *       3333
 *      4    5
 *      4    5
 *       6666
 *  - `X` - the literal digit X
 *
 * 1, 4, 7, 8 - known (sizes 2, 4, 3, 7 respectively)
 *
 * `s1` + `s3` = `4` - `1`
 *  `6` = (len == 6) && !contains(`1`)
 *  `0` = ((len == 6) && !contains(`s1` + `s3`))
 *  `9` = ((len == 6 && contains(`7`) && is_not(`0`)))  # remaining (len == 6)
 *  `5` = ((len == 5) && contains(`s1` + `s3`))
 * `s4` = `6` - `5`
 *  `2` = ((len == 5) && contains(`s4`)
 *  `3` = remaining (len == 5)
 */
static inline void unscramble_encoding(std::vector<std::string_view> const& in, std::vector<DigitEncoding>& ret) {
    ret.resize(10);

    for (auto p : in) {
        switch (p.size()) {
            case 2: ret[1] = parse_encoding(p); break;
            case 3: ret[7] = parse_encoding(p); break;
            case 4: ret[4] = parse_encoding(p); break;
            case 7: ret[8] = parse_encoding(p); break;
        }
    }

    auto s1s3 = substract(ret[4], ret[1]);
    auto str_6 = std::string_view{};
    auto str_0 = std::string_view{};
    for (auto p : in) {
        if (p.size() != 6) continue;
        auto v = parse_encoding(p);
        if (!contains(v, ret[1])) {
            ret[6] = v;
            str_6 = p;
            continue;
        }
        if (!contains(v, s1s3)) {
            ret[0] = v;
            str_0 = p;
            continue;
        }
    }
    for (auto p : in) {
        if (p.size() != 6) continue;
        if (p != str_6 && p != str_0) {
            ret[9] = parse_encoding(p);
            break;
        }
    }

    auto str_5 = std::string_view{};
    for (auto p : in) {
        if (p.size() != 5) continue;
        auto v = parse_encoding(p);
        if (contains(v, s1s3)) {
            ret[5] = v;
            str_5 = p;
            break;
        }
    }

    auto s4 = substract(ret[6], ret[5]);
    for (auto p : in) {
        if (p.size() != 5) continue;
        if (p == str_5) continue;
        auto v = parse_encoding(p);
        if (contains(v, s4)) {
            ret[2] = v;
            continue;
        } else {
            ret[3] = v;
            continue;
        }
    }
}

int main() {
    static const auto pipe_re = std::regex("\\s*\\|\\s*");
    static const auto space_re = std::regex("\\s+");

    auto line = std::string{};
    auto part1 = size_t{0};
    auto part2 = size_t{0};
    auto digits = std::vector<DigitEncoding>{};

    while (std::getline(std::cin, line)) {
        auto main_parts = aoc::str_split(aoc::trim(aoc::to_sv(line)), pipe_re);

        unscramble_encoding(aoc::str_split(main_parts[0], space_re), digits);

        size_t value{0};
        for (auto& p : aoc::str_split(main_parts[1], space_re)) {
            auto l = p.size();
            part1 += (l == 2 || l == 4 || l == 3 || l == 7);

            auto v = parse_encoding(p);
            for (size_t i = 0; i < 10; i++) {
                if (equal(v, digits.at(i))) {
                    value = value * 10 + i;
                    break;
                }
            }
        }
        part2 += value;
    }

    fmt::print("{}\n", part1);
    fmt::print("{}\n", part2);

    return 0;
}

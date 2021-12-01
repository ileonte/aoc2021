#include <aoc.h>

int main() {
    long v1{};
    long v2{};
    long v3{};
    long t{};
    long p1{};
    long p2{};

    std::cin >> v1;
    std::cin >> v2; p1 += (v2 > v1);
    std::cin >> v3; p1 += (v3 > v1);

    while (std::cin >> t) {
        auto s1 = v1 + v2 + v3;
        auto s2 = v2 + v3 + t;

        p1 += (t > v3);
        p2 += (s2 > s1);

        v1 = v2;
        v2 = v3;
        v3 = t;
    }

    fmt::print("{}\n", p1);
    fmt::print("{}\n", p2);

    return 0;
}

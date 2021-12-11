#include <aoc.h>

#define FIELD_SIZE 10

using Field = std::array<char, FIELD_SIZE * FIELD_SIZE>;

static inline size_t flash(Field& field, int x, int y) {
    auto ret = size_t{1};
    field.at(y * FIELD_SIZE + x) = 10;
    for (auto dy = std::max(0, y - 1); dy <= std::min(FIELD_SIZE - 1, y + 1); dy++) {
        for (auto dx = std::max(0, x - 1); dx <= std::min(FIELD_SIZE - 1, x + 1); dx++) {
            auto& c = field.at(dy * FIELD_SIZE + dx);
            switch (c) {
                case 0: [[fallthrough]];
                case 1: [[fallthrough]];
                case 2: [[fallthrough]];
                case 3: [[fallthrough]];
                case 4: [[fallthrough]];
                case 5: [[fallthrough]];
                case 6: [[fallthrough]];
                case 7: [[fallthrough]];
                case 8: {
                    c += 1;
                    break;
                }
                case 9: {
                    ret += flash(field, dx, dy);
                    break;
                }
                default: continue;
            }
        }
    }
    return ret;
}

static inline size_t run_step(Field& field) {
    auto ret = size_t{};
    for (int y = 0; y < FIELD_SIZE; y++) {
        for (int x = 0; x < FIELD_SIZE; x++) {
            auto& c = field.at(y * FIELD_SIZE + x);
            switch (c) {
                case 0: [[fallthrough]];
                case 1: [[fallthrough]];
                case 2: [[fallthrough]];
                case 3: [[fallthrough]];
                case 4: [[fallthrough]];
                case 5: [[fallthrough]];
                case 6: [[fallthrough]];
                case 7: [[fallthrough]];
                case 8: {
                    c += 1;
                    break;
                }
                case 9: {
                    ret += flash(field, x, y);
                    break;
                }
                default: continue;
            }
        }
    }
    for (auto& c : field) {
        if (c > 9) c = 0;
    }
    return ret;
}

int main() {
    auto line = std::string{};
    auto field = Field{};
    for (size_t y = 0; y < FIELD_SIZE; y++) {
        if (!std::getline(std::cin, line)) return 1;
        auto sv = aoc::trim(aoc::to_sv(line));
        if (sv.size() != FIELD_SIZE) return 1;
        for (size_t x = 0; x < FIELD_SIZE; x++)
            field.at(y * FIELD_SIZE + x) = sv.at(x) - '0';
    }

    auto part1 = size_t{};
    for (size_t step = 0; ; step++) {
        auto flashes = run_step(field);
        part1 += flashes;
        if (step == 99) fmt::print("{}\n", part1);
        if (flashes >= field.size()) {
            fmt::print("{}\n", step + 1);
            break;
        }
    }

    return 0;
}

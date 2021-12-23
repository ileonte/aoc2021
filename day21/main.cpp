#include <aoc.h>

static inline std::tuple<size_t, size_t, size_t> play_game(size_t p1_pos, size_t p2_pos, size_t max_score, size_t die_sides, size_t start_roll = 1) {
    auto p1_score = size_t{};
    auto p2_score = size_t{};
    auto rolls = size_t{};
    auto next_roll = start_roll - 1;

    while (true) {
        auto step = next_roll + 1;
        auto inc = step;
        next_roll = (next_roll + 1) % die_sides;
        step = next_roll + 1;
        inc += step;
        next_roll = (next_roll + 1) % die_sides;
        step = next_roll + 1;
        inc += step;
        next_roll = (next_roll + 1) % die_sides;

        rolls += 3;
        p1_pos = (p1_pos + inc) % 10;
        p1_score += (p1_pos + 1);
        if (p1_score >= max_score) break;

        step = next_roll + 1;
        inc = step;
        next_roll = (next_roll + 1) % die_sides;
        step = next_roll + 1;
        inc += step;
        next_roll = (next_roll + 1) % die_sides;
        step = next_roll + 1;
        inc += step;
        next_roll = (next_roll + 1) % die_sides;

        rolls += 3;
        p2_pos = (p2_pos + inc) % 10;
        p2_score += (p2_pos + 1);
        if (p2_score >= max_score) break;
    }

    auto losing_score = std::min(p1_score, p2_score);
    return {losing_score == p1_score ? 2 : 1, losing_score, rolls};
}

static inline size_t spow(size_t n, size_t p) {
    auto ret = size_t{1};
    while (p--) ret *= n;
    return ret;
}

int main() {
    auto p1_pos = size_t{};
    auto p2_pos = size_t{};

    auto line = std::string{};
    if (!std::getline(std::cin, line)) return 1;
    auto sv = aoc::substr(aoc::trim(line), 28);
    if (!aoc::from_chars(sv, p1_pos)) return 1;

    if (!std::getline(std::cin, line)) return 1;
    sv = aoc::substr(aoc::trim(line), 28);
    if (!aoc::from_chars(sv, p2_pos)) return 1;

    p1_pos = std::min(p1_pos - 1, size_t(9));
    p2_pos = std::min(p2_pos - 1, size_t(9));

    {
        auto [winning_player, score, rolls] = play_game(p1_pos, p2_pos, 1000, 100);
        fmt::print("{}\n", score * rolls);
    }

    {
        auto [winning_player, score, rolls] = play_game(p1_pos, p2_pos, 21, 3, 1);
        fmt::print("{} {} {}\n", winning_player, score, rolls);
    }
    {
        auto [winning_player, score, rolls] = play_game(p1_pos, p2_pos, 21, 3, 2);
        fmt::print("{} {} {}\n", winning_player, score, rolls);
    }
    {
        auto [winning_player, score, rolls] = play_game(p1_pos, p2_pos, 21, 3, 3);
        fmt::print("{} {} {}\n", winning_player, score, rolls);
    }

    return 0;
}

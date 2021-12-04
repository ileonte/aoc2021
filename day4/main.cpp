#include <aoc.h>

struct Board {
    static constexpr const size_t WIDTH{5};
    static constexpr const size_t HEIGHT{5};
    static constexpr const uint8_t SEEN{0x80};

    uint8_t data[WIDTH * HEIGHT]{};

    static bool read(std::istream& in, Board& ret) {
        static const auto split_re = std::regex("\\s+");
        auto line = std::string{};
        auto sv = std::string_view{};

        if (!std::getline(in, line)) return false;

        for (size_t i = 0; i < HEIGHT; i++) {
            if (!std::getline(in, line)) return false;
            sv = aoc::trim(aoc::to_sv(line));
            auto pieces = aoc::str_split(sv, split_re);
            if (pieces.size() != WIDTH) return false;
            for (size_t j = 0; j < WIDTH; j++) {
                if (!aoc::from_chars(pieces.at(j), ret.data[i * WIDTH + j]))
                    return false;
            }
        }

        return true;
    }

    void play(uint8_t number) {
        for (auto& d : data) {
            if ((d & ~SEEN) == number)
                d |= SEEN;
        }
    }

    bool check_bingo() const {
        for (size_t i = 0; i < HEIGHT; i++) {
            auto* row = data + i * WIDTH;
            auto row_bingo{true};
            for (size_t j = 0; j < WIDTH; j++)
                row_bingo = row_bingo && ((row[j] & SEEN) == SEEN);
            if (row_bingo) return true;
        }

        for (size_t j = 0; j < WIDTH; j++) {
            auto col_bingo{true};
            for (size_t i = 0; i < HEIGHT; i++)
                col_bingo = col_bingo && ((data[i * WIDTH + j] & SEEN) == SEEN);
            if (col_bingo) return true;
        }

        return false;
    }

    size_t get_score(uint8_t last_played_number) const {
        auto unmarked_score = aoc::accumulate(data, size_t(0), [](size_t s, uint8_t v) -> size_t {
            return ((v & SEEN) != SEEN) ? s + v : s;
        });
        return unmarked_score * last_played_number;
    }

    void print() const {
        for (size_t i = 0; i < HEIGHT; i++) {
            auto* row = data + i * WIDTH;
            for (size_t j = 0; j < WIDTH; j++) {
                if ((row[j] & SEEN) == SEEN) fmt::print("*{:2}* ", (row[j] & ~SEEN));
                else fmt::print(" {:2}  ", row[j]);
            }
            fmt::print("\n");
        }
    }
};

int main() {
    auto line = std::string{};
    auto numbers = std::vector<uint8_t>{};
    auto winning_number_first = uint8_t{};
    auto winning_moves_first = std::numeric_limits<size_t>::max();
    auto winning_board_first = Board{};
    auto winning_number_last = uint8_t{};
    auto winning_moves_last = std::numeric_limits<size_t>::min();
    auto winning_board_last = Board{};

    if (!std::getline(std::cin, line)) return 1;
    auto pieces = aoc::str_split(aoc::trim(aoc::to_sv(line)), ',');
    numbers.resize(pieces.size(), 0);
    for (size_t i = 0; i < pieces.size(); i++) {
        if (!aoc::from_chars(pieces.at(i), numbers.at(i)))
            return 1;
    }

    auto current_board = Board{};
    while (Board::read(std::cin, current_board)) {
        for (size_t i = 0; i < numbers.size(); i++) {
            current_board.play(numbers.at(i));
            if (current_board.check_bingo()) {
                if (i < winning_moves_first) {
                    winning_number_first = numbers.at(i);
                    winning_moves_first = i;
                    winning_board_first = current_board;
                }
                if (i > winning_moves_last) {
                    winning_number_last = numbers.at(i);
                    winning_moves_last = i;
                    winning_board_last = current_board;
                }
                break;
            }
        }
    }

    fmt::print("{}\n", winning_board_first.get_score(winning_number_first));
    fmt::print("{}\n", winning_board_last.get_score(winning_number_last));
    
    return 0;
}

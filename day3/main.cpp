#include <aoc.h>

static std::vector<std::string> lines{};
static std::vector<size_t> one_counts{}, zero_counts{};
static size_t bit_size{};
static size_t gamma_{};
static size_t epsilon_{};

static inline bool init_counts(std::string_view line) {
    if (line.empty()) {
        fmt::print("Invalid first init line: empty!\n");
        return false;
    }

    bit_size = line.size();
    if (bit_size > 64) {
        fmt::print("Unsupported bit size: {}\n", bit_size);
        return false;
    }

    one_counts.resize(bit_size, 0);
    zero_counts.resize(bit_size, 0);

    for (size_t i = 0; i < bit_size; i++) {
        switch (line[i]) {
            case '1': one_counts[i]++; break;
            case '0': zero_counts[i]++; break;
            default: {
                fmt::print("Invalid character '{}' at position {}\n", line[i], i);
                return false;
            }
        }
    }

    return true;
}

static inline bool update_counts(std::string_view line) {
    if (line.size() != bit_size) {
        fmt::print("Invalid line length {} (expected {})\n", line.size(), bit_size);
        return false;
    }

    for (size_t i = 0; i < bit_size; i++) {
        switch (line[i]) {
            case '1': one_counts[i]++; break;
            case '0': zero_counts[i]++; break;
            default: {
                fmt::print("Invalid character '{}' at position {}\n", line[i], i);
                return false;
            }
        }
    }

    return true;
}

using filter_fn = char (*)(std::vector<size_t> const& one_counts, std::vector<size_t> const& zero_counts, size_t pos);
static inline size_t filter_by(filter_fn filter) {
    auto values = std::vector<std::string_view>{};
    auto ones = one_counts;
    auto zeroes = zero_counts;

    for (auto& l : lines) values.push_back(aoc::to_sv(l));

    for (size_t bit = 0; bit < bit_size; bit++) {
        auto ch = filter(ones, zeroes, bit);
        auto removed = size_t{0};

        for (auto it = values.begin(); it != values.end(); ) {
            if ((*it)[bit] != ch) {
                for (size_t i = 0; i < bit_size; i++) {
                    ones[i] -= ((*it)[i] == '1');
                    zeroes[i] -= ((*it)[i] == '0');
                }
                values.erase(it);
                removed++;
            } else {
                it++;
            }
        }
        if (!removed || values.size() <= 1) break;
    }

    size_t ret{};
    if (values.size()) {
        ret |= values[0][0] - '0';
        for (size_t bit = 1; bit < bit_size; bit++) {
            ret <<= 1;
            ret |= values[0][bit] - '0';
        }
    }
    return ret;
}

int main() {
    std::string line{};

    std::getline(std::cin, line);
    auto sv = aoc::trim(aoc::to_sv(line));
    if (!init_counts(sv)) return 1;
    lines.push_back(std::string(sv.data(), sv.size()));
    while (std::getline(std::cin, line)) {
        sv = aoc::trim(aoc::to_sv(line));
        if (!update_counts(sv)) return 1;
        lines.push_back(std::string(sv.data(), sv.size()));
    }

    if (one_counts[0] > zero_counts[0]) gamma_ |= 1;
    if (one_counts[0] < zero_counts[0]) epsilon_ |= 1;
    for (size_t i = 1; i < bit_size; i++) {
        gamma_ <<= 1;
        epsilon_ <<= 1;
        if (one_counts[i] > zero_counts[i]) gamma_ |= 1;
        if (one_counts[i] < zero_counts[i]) epsilon_ |= 1;
    }
    fmt::print("{}\n", gamma_ * epsilon_);

    auto o2_rating = filter_by([](std::vector<size_t> const& ones, std::vector<size_t> const& zeroes, size_t pos) -> char {
        return ones[pos] >= zeroes[pos] ? '1' : '0';
    });
    auto co2_rating = filter_by([](std::vector<size_t> const& ones, std::vector<size_t> const& zeroes, size_t pos) -> char {
        return ones[pos] < zeroes[pos] ? '1' : '0';
    });
    fmt::print("{}\n", o2_rating * co2_rating);
    
    return 0;
}

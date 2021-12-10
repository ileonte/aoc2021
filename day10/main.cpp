#include <aoc.h>

static inline char opposite(char c) {
    switch (c) {
        case '<': return '>'; break;
        case '{': return '}'; break;
        case '[': return ']'; break;
        case '(': return ')'; break;

        case '>': return '<'; break;
        case '}': return '{'; break;
        case ']': return '['; break;
        case ')': return '('; break;
    }

    std::abort();
    return 0;
}

static inline size_t error_score(char c) {
    switch (c) {
        case '>': return 25137; break;
        case '}': return 1197; break;
        case ']': return 57; break;
        case ')': return 3; break;
        default: return 0; break;
    }
}

static inline size_t completion_score(char c) {
    switch (c) {
        case '>': return 4; break;
        case '}': return 3; break;
        case ']': return 2; break;
        case ')': return 1; break;
        default: return 0; break;
    }
}

static inline size_t check_for_errors(std::stack<char>& stack, std::string_view line) {
    auto size = stack.size();
    for (auto c : line) {
        switch (c) {
            case '<': [[fallthrough]];
            case '{': [[fallthrough]];
            case '[': [[fallthrough]];
            case '(': {
                stack.push(c);
                break;
            }

            case '>': [[fallthrough]];
            case '}': [[fallthrough]];
            case ']': [[fallthrough]];
            case ')': {
                if (!stack.size() || stack.top() != opposite(c)) {
                    while (stack.size() > size) stack.pop();
                    return error_score(c);
                }
                stack.pop();
                break;
            }
        }
    }
    return 0;
}

static inline size_t get_completion_score(std::string_view line) {
    auto stack = std::stack<char>{};
    for (auto c : line) {
        switch (c) {
            case '<': [[fallthrough]];
            case '{': [[fallthrough]];
            case '[': [[fallthrough]];
            case '(': {
                stack.push(c);
                break;
            }

            case '>': [[fallthrough]];
            case '}': [[fallthrough]];
            case ']': [[fallthrough]];
            case ')': {
                stack.pop();
                break;
            }
        }
    }

    auto score = size_t{};
    while (!stack.empty()) {
        score = score * 5 + completion_score(opposite(stack.top()));
        stack.pop();
    }
    return score;
}

int main() {
    auto stack = std::stack<char>{};

    auto line = std::string{};
    auto part1 = size_t{};
    auto part2 = std::vector<size_t>{};
    while (std::getline(std::cin, line)) {
        auto sv = aoc::trim(aoc::to_sv(line));
        auto error_score = check_for_errors(stack, sv);
        part1 += error_score;

        if (error_score) continue;
        part2.push_back(get_completion_score(sv));
    }
    std::sort(part2.begin(), part2.end());

    fmt::print("{}\n", part1);
    fmt::print("{}\n", part2.at(part2.size() / 2));

    return 0;
}

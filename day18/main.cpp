#include <aoc.h>

static inline size_t parse_pair(std::string_view str, size_t start = 0) {
    if (start >= str.size()) return start;
    auto pos = start;
    auto level = size_t{str[pos++] == '['};
    while (level && (pos < str.size())) {
        switch (str[pos]) {
            case '[': level++; break;
            case ']': level--; break;
        }
        if (level == std::numeric_limits<size_t>::max()) return start;
        pos++;
    }
    return str[pos - 1] == ']' ? pos - 1 : start;
}

static inline bool may_be_int(char c) {
    return ((c >= '0' && c <= '9') || (c == '-'));
}

struct SnailfishNumber {
    enum Type {
        Empty,
        RegularNumber,
        Pair,
    };

    Type type{};
    SnailfishNumber* parent{};
    union {
        int numeric_value;
        struct {
            SnailfishNumber* l;
            SnailfishNumber* r;
        } pair_value;
    };

    SnailfishNumber() = default;
    SnailfishNumber(int v) {
        type = RegularNumber;
        numeric_value = v;
    }
    ~SnailfishNumber() {
        if (type == Pair) {
            delete pair_value.l;
            delete pair_value.r;
        }
    }

    static SnailfishNumber* from_string(std::string_view str) {
        if (str.empty()) return nullptr;

        if (may_be_int(str[0])) {
            auto v = int{};
            if (!aoc::from_chars(str, v)) return nullptr;
            auto* ret = new SnailfishNumber{};
            ret->type = RegularNumber;
            ret->numeric_value = v;
            return ret;
        }

        auto left = std::string_view{};
        auto right = std::string_view{};
        auto end = parse_pair(str);

        if (end != str.size() - 1) return nullptr;
        str = aoc::substr(str, 1, str.size() - 2);

        if (str.empty()) return nullptr;
        auto start = size_t{};
        if (str[start] == '[') {
            end = parse_pair(str, start);
            if (end == start) return nullptr;
            left = aoc::substr(str, start, end - start + 1);
            start = end + 1;
        } else if (may_be_int(str[start])) {
            auto pos = start;
            while ((pos < str.size()) && may_be_int(str[pos])) pos++;
            left = aoc::substr(str, start, pos - start);
            start = pos;
        } else {
            return nullptr;
        }

        if (start >= str.size()) return nullptr;
        if (str[start] != ',') return nullptr;

        start++;
        if (str[start] == '[') {
            end = parse_pair(str, start);
            if (end == start) return nullptr;
            right = aoc::substr(str, start, end - start + 1);
        } else if (may_be_int(str[start])) {
            auto pos = start;
            while ((pos < str.size()) && may_be_int(str[pos])) pos++;
            right = aoc::substr(str, start, pos - start);
        } else {
            return nullptr;
        }

        auto* l = from_string(left);
        auto* r = from_string(right);
        if (!l || !r) {
            if (l) delete l;
            if (r) delete r;
            return nullptr;
        }
        auto* ret = new SnailfishNumber{};
        ret->type = Pair;
        ret->pair_value.l = l;
        ret->pair_value.r = r;
        l->parent = ret;
        r->parent = ret;
        return ret;
    }

    bool is_number_pair() const {
        return (type == Pair) && (pair_value.l->type == RegularNumber) && (pair_value.r->type == RegularNumber);
    }

    size_t nesting() const {
        auto ret = size_t{};
        auto p = parent;
        while (p) {
            ret++;
            p = p->parent;
        }
        return ret;
    }

    long long magnitude() const {
        switch (type) {
            case Empty: return 0;
            case RegularNumber: return numeric_value;
            case Pair: return 3 * pair_value.l->magnitude() + 2 * pair_value.r->magnitude();
        }
        std::abort();
    }

    void flatten(std::vector<SnailfishNumber*>& v) {
        switch (type) {
            case Empty: [[fallthrough]];
            case RegularNumber: {
                v.push_back(this);
                break;
            }
            default: {
                if (is_number_pair()) {
                    v.push_back(this);
                } else {
                    pair_value.l->flatten(v);
                    pair_value.r->flatten(v);
                }
                break;
            }
        }
    }

    std::vector<SnailfishNumber*> flatten() {
        auto ret = std::vector<SnailfishNumber*>{};
        flatten(ret);
        return ret;
    }

    SnailfishNumber* duplicate() const {
        switch (type) {
            case Empty: return new SnailfishNumber{};
            case RegularNumber: return new SnailfishNumber(numeric_value);
            case Pair: {
                auto ret = new SnailfishNumber{};
                ret->type = Pair;
                ret->pair_value.l = pair_value.l->duplicate();
                ret->pair_value.r = pair_value.r->duplicate();
                ret->pair_value.l->parent = ret;
                ret->pair_value.r->parent = ret;
                return ret;
            }
        }
        std::abort();
    }

    void add(SnailfishNumber const* n) {
        if (type == Empty) return;

        auto* l = new SnailfishNumber{};
        l->parent = this;
        l->type = type;
        if (type == RegularNumber)
            l->numeric_value = numeric_value;
        else {
            l->pair_value.l = pair_value.l;
            l->pair_value.r = pair_value.r;
            pair_value.l->parent = l;
            pair_value.r->parent = l;
        }

        type = Pair;
        pair_value.l = l;
        pair_value.r = n->duplicate();
        pair_value.r->parent = this;
    }

    void print(std::ostream& out = std::cout) const {
        switch (type) {
            case Empty: {
                fmt::print(out, "[]");
                break;
            }
            case RegularNumber: {
                fmt::print(out, "{}", numeric_value);
                break;
            }
            case Pair: {
                fmt::print(out, "[");
                pair_value.l->print(out);
                fmt::print(out, ",");
                pair_value.r->print(out);
                fmt::print(out, "]");
                break;
            }
        }
    }

    void reduce() {
        auto pieces = std::vector<SnailfishNumber*>{};
        pieces.reserve(128);

        while (true) {

            pieces.clear();
            flatten(pieces);

            auto action_taken = false;
            for (size_t idx = 0; idx < pieces.size(); idx++) {
                auto p = pieces[idx];
                if (p->type != Pair) continue;
                if (p->nesting() != 4) continue;

                SnailfishNumber *left{nullptr}, *right{nullptr};
                if (idx) {
                    left = pieces[idx - 1];
                    if (left->type == Pair) left = left->pair_value.r;
                }
                if (idx < pieces.size() - 1) {
                    right = pieces[idx + 1];
                    if (right->type == Pair) right = right->pair_value.l;
                }
                p->explode(left, right);

                action_taken = true;
                break;
            }
            if (action_taken) continue;

            for (size_t idx = 0; idx < pieces.size(); idx++) {
                auto p = pieces[idx];
                if (p->type == Pair) {
                    if (p->pair_value.l->numeric_value >= 10) {
                        p->pair_value.l->split();
                        action_taken = true;
                        break;
                    }
                    if (p->pair_value.r->numeric_value >= 10) {
                        p->pair_value.r->split();
                        action_taken = true;
                        break;
                    }
                } else {
                    if (p->numeric_value >= 10) {
                        p->split();
                        action_taken = true;
                        break;
                    }
                }
            }

            if (!action_taken) break;
        }
    }

private:
    void explode(SnailfishNumber* left, SnailfishNumber* right) {
        if (left) left->numeric_value += pair_value.l->numeric_value;
        if (right) right->numeric_value += pair_value.r->numeric_value;
        delete pair_value.l;
        delete pair_value.r;
        type = RegularNumber;
        numeric_value = 0;
    }

    void split() {
        auto lv = numeric_value / 2;
        auto rv = numeric_value - lv;
        type = Pair;
        pair_value.l = new SnailfishNumber(lv);
        pair_value.r = new SnailfishNumber(rv);
        pair_value.l->parent = this;
        pair_value.r->parent = this;
    }
};
std::ostream& operator<<(std::ostream& out, SnailfishNumber const& n) {
    n.print(out);
    return out;
}

static inline size_t add(SnailfishNumber const* n1, SnailfishNumber const* n2) {
    auto sum = n1->duplicate();
    sum->add(n2);
    sum->reduce();
    auto m = sum->magnitude();
    delete sum;
    return m;
}

int main() {
    auto numbers = std::vector<SnailfishNumber*>{};
    auto line = std::string{};
    if (!std::getline(std::cin, line)) return 1;
    auto sv = aoc::trim(line);
    auto* n = SnailfishNumber::from_string(sv);
    if (!n) {
        fmt::print(std::cerr, "Error: Failed to parse '{}'\n", sv);
        return 1;
    }
    numbers.push_back(n);

    while (std::getline(std::cin, line)) {
        sv = aoc::trim(line);
        n = SnailfishNumber::from_string(sv);
        if (!n) {
            fmt::print(std::cerr, "Error: Failed to parse '{}'\n", sv);
            return 1;
        }
        numbers.push_back(n);
    }

    auto sum = numbers[0]->duplicate();
    for (size_t i = 1; i < numbers.size(); i++) {
        sum->add(numbers[i]);
        sum->reduce();
    }
    auto part1 = sum->magnitude();
    delete sum;

    auto part2 = size_t{};
    for (auto n1 : numbers) {
        for (auto n2 : numbers) {
            if (n1 == n2) continue;
            part2 = std::max(part2, std::max(add(n1, n2), add(n2, n1)));
        }
    }

    fmt::print("{}\n", part1);
    fmt::print("{}\n", part2);

    for (auto n : numbers) delete n;

    return 0;
}

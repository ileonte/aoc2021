#include <aoc.h>

namespace std {
    template<>
    struct hash<vector<uint64_t>> {
        size_t operator()(vector<uint64_t> const& v) const {
            // https://github.com/boostorg/container_hash/blob/develop/include/boost/container_hash/hash.hpp#L342
            constexpr const uint64_t m = (uint64_t(0xc6a4a793) << 32) + 0x5bd1e995;
            constexpr const int r = 47;

            auto h = v.size();
            for (auto k : v) {
                k *= m;
                k ^= k >> r;
                k *= m;

                h ^= k;
                h *= m;

                h += 0xe6546b64;
            }
            return h;
        }
    };
}

struct Graph {
    using IntType = uint64_t;
    using Path = std::vector<IntType>;

    std::vector<std::string> node_names{};
    std::vector<Path> adj_list{};
    std::vector<size_t> constraints{};

    IntType alloc_node(std::string const& name) {
        for (size_t id = 0; id < node_names.size(); id++) {
            if (node_names.at(id) == name) return id;
        }
        node_names.push_back(name);
        adj_list.resize(node_names.size());
        constraints.resize(node_names.size());
        constraints.back() = std::islower(name.at(0)) ? 1 : std::numeric_limits<size_t>::max();
        return node_names.size() - 1;
    }

    bool add_edge(std::string const& desc) {
        auto ar = aoc::str_split(aoc::trim(aoc::to_sv(desc)), '-');
        if (ar.size() != 2) return 1;
        if (ar.at(0).empty() || ar.at(1).empty())
            return false;

        auto name1 = std::string(ar[0]);
        auto name2 = std::string(ar[1]);

        auto id1 = alloc_node(name1);
        auto id2 = alloc_node(name2);

        auto& al1 = adj_list[id1];
        if (auto it = std::find(al1.begin(), al1.end(), id2); it == al1.end())
            al1.emplace_back(id2);

        auto& al2 = adj_list[id2];
        if (auto it = std::find(al2.begin(), al2.end(), id1); it == al2.end())
            al2.emplace_back(id1);

        return true;
    }

    void constrained_dft(IntType start, IntType end, std::unordered_set<size_t>& paths) const {
        auto counts = std::vector<size_t>{};
        auto path = Path{};

        path.reserve(128);
        counts.resize(node_names.size());

        dft_recurse(start, end, path, paths, counts);
    }

private:
    inline void dft_recurse(IntType current, IntType target, Path& path, std::unordered_set<size_t>& paths, std::vector<size_t>& counts) const {
        if (current == target) {
            auto hash = std::hash<Path>{}(path);
            paths.insert(hash);
            return;
        }

        path.push_back(current);

        counts.at(current) += 1;
        for (auto n : adj_list.at(current)) {
            if (counts.at(n) >= constraints.at(n)) continue;
            dft_recurse(n, target, path, paths, counts);
        }
        counts.at(current) -= 1;

        path.pop_back();
    }
};

int main() {
    auto graph = Graph{};
    auto id_start = graph.alloc_node("start");
    auto id_end = graph.alloc_node("end");

    auto line = std::string{};
    while (std::getline(std::cin, line))
        if (!graph.add_edge(line)) return 1;

    auto paths = std::unordered_set<size_t>{};
    auto start = std::chrono::steady_clock::now();
    graph.constrained_dft(id_start, id_end, paths);
    auto end = std::chrono::steady_clock::now();
    auto ns1 = (end - start).count();
    fmt::print("{} ({})\n", paths.size(), ns1);

    paths.clear();
    start = std::chrono::steady_clock::now();
    for (size_t id = 2; id < graph.node_names.size(); id++) {
        auto const& name = graph.node_names.at(id);
        if (!std::islower(name.at(0))) continue;

        graph.constraints.at(id) = 2;
        graph.constrained_dft(id_start, id_end, paths);
        graph.constraints.at(id) = 1;
    }
    end = std::chrono::steady_clock::now();
    auto ns2 = (end - start).count();
    fmt::print("{} ({})\n", paths.size(), ns2);
    fmt::print("{}\n", ns1 + ns2);

    return 0;
}

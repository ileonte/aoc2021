#include <aoc.h>

struct Graph {
    using IntType = uint16_t;
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

    void constrained_dft(IntType start, IntType end, std::set<Path>& paths) const {
        auto counts = std::vector<size_t>{};
        auto path = Path{};

        path.reserve(128);
        counts.resize(node_names.size());

        dft_recurse(start, end, path, paths, counts);
    }

private:
    inline void dft_recurse(IntType current, IntType target, Path& path, std::set<Path>& paths, std::vector<size_t>& counts) const {
        if (current == target) {
            paths.insert(path);
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

    auto line = std::string{};
    while (std::getline(std::cin, line))
        if (!graph.add_edge(line)) return 1;

    auto id_start = graph.alloc_node("start");
    auto id_end = graph.alloc_node("end");

    auto paths = std::set<Graph::Path>{};
    graph.constrained_dft(id_start, id_end, paths);
    fmt::print("{}\n", paths.size());

    paths.clear();
    for (size_t id = 0; id < graph.node_names.size(); id++) {
        auto name = graph.node_names.at(id);
        if (!std::islower(name.at(0))) continue;
        if (name == "start" || name == "end") continue;

        graph.constraints.at(id) = 2;
        graph.constrained_dft(id_start, id_end, paths);
        graph.constraints.at(id) = 1;
    }
    fmt::print("{}\n", paths.size());

    return 0;
}

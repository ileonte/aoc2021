#include <aoc.h>

struct Bitstream {
    std::vector<char> data{};
    size_t pos{};

    static bool from_istream(std::istream& in, Bitstream& ret) {
        auto line = std::string{};
        if (!std::getline(in, line)) return false;
        if (line.size() % 2) return false;
        ret.data.clear();
        ret.data.reserve(line.size() * 4);
        for (size_t i = 0; i < line.size() / 2; i++) {
            auto sv = aoc::substr(line, i * 2, 2);
            auto v = uint8_t{};
            if (!aoc::from_chars(sv, v, 16)) return false;
            fmt::format_to(std::back_inserter(ret.data), "{:08b}", v);
        }
        return true;
    }

    void print() const {
        fmt::print("{}\n", std::string_view(data.data(), data.size()));
    }

    std::optional<size_t> read_int(size_t bit_size) {
        if ((pos + bit_size) >= data.size()) return std::nullopt;
        auto ret = size_t{};
        auto sv = std::string_view(data.data() + pos, bit_size);
        if (!aoc::from_chars(sv, ret, 2))
            return std::nullopt;
        pos += bit_size;
        return ret;
    }

    std::optional<Bitstream> read_substream(size_t bit_size) {
        if ((pos + bit_size) >= data.size()) return std::nullopt;
        auto ret = Bitstream{};
        ret.data.resize(bit_size + (8 - (bit_size % 8)), '0');
        std::copy(data.data() + pos, data.data() + pos + bit_size, ret.data.data());
        pos += bit_size;
        return ret;
    }
};

struct Packet {
    size_t ver{};
    size_t id{};
    size_t val{};
    std::vector<Packet> children{};

    static bool from_stream(Bitstream& stream, Packet& ret) {
        auto ver = stream.read_int(3).value_or(0);
        auto id = stream.read_int(3).value_or(0);

        ret = Packet{};
        ret.ver = ver;
        ret.id = id;

        switch (id) {
            case 4: {
                auto v = size_t{};
                while (true) {
                    auto piece = stream.read_int(5).value_or(0xff);
                    if (piece == 0xff) return false;
                    v = ((v << 4) | (piece & 0x0f));
                    if (piece <= 0x0f) break;
                }
                ret.val = v;
                return true;
            }

            default: {
                auto len_id = stream.read_int(1).value_or(2);
                if (len_id > 1) return false;

                if (len_id == 0) {
                    auto sub_len = stream.read_int(15).value_or(0xffff);
                    if (sub_len == 0xffff) return false;
                    if (auto substream = stream.read_substream(sub_len); substream) {
                        auto pkt = Packet{};
                        while (Packet::from_stream(substream.value(), pkt)) {
                            ret.children.push_back(pkt);
                        }
                        return true;
                    } else {
                        return false;
                    }
                } else {
                    auto sub_count = stream.read_int(11).value_or(0xffff);
                    if (sub_count == 0xffff) return false;
                    for (size_t i = 0; i < sub_count; i++) {
                        auto pkt = Packet{};
                        if (!Packet::from_stream(stream, pkt)) return false;
                        ret.children.push_back(pkt);
                    }
                    return true;
                }

                return true;
            }
        }

        return true;
    }

    size_t ver_sum() const {
        return ver + aoc::accumulate(children, size_t(0), [](size_t s, Packet const& p) -> size_t { return s + p.ver_sum(); });
    }

    size_t evaluate() const {
        switch (id) {
            case 0: return aoc::accumulate(children, size_t(0), [](size_t s, Packet const& p) { return s + p.evaluate(); });
            case 1: return aoc::accumulate(children, size_t(1), [](size_t s, Packet const& p) { return s * p.evaluate(); });
            case 2: {
                auto ret = std::numeric_limits<size_t>::max();
                for (auto& p : children) ret = std::min(ret, p.evaluate());
                return ret;
            }
            case 3: {
                auto ret = std::numeric_limits<size_t>::min();
                for (auto& p : children) ret = std::max(ret, p.evaluate());
                return ret;
            }
            case 4: return val;
            case 5: return (children[0].evaluate() > children[1].evaluate()) ? 1 : 0;
            case 6: return (children[0].evaluate() < children[1].evaluate()) ? 1 : 0;
            case 7: return (children[0].evaluate() == children[1].evaluate()) ? 1 : 0;
        }
        std::abort();
    }

    void print(size_t ident = 0) const {
        for (size_t i = 0; i < ident; i++)
            fmt::print(" ");
        fmt::print("PKT ver={}, id={}", ver, id);
        if (id == 4) {
            fmt::print(", val={}\n", val);
        } else {
            fmt::print(", children:\n");
            for (auto& p : children)
                p.print(ident + 4);
        }
    }
};

int main() {
    auto stream = Bitstream{};
    auto packets = std::vector<Packet>{};

    if (!Bitstream::from_istream(std::cin, stream)) return 1;

    auto start = std::chrono::steady_clock::now();
    auto pkt = Packet{};
    while (Packet::from_stream(stream, pkt)) {
        packets.push_back(pkt);
    }
    auto end = std::chrono::steady_clock::now();
    auto ns1 = (end - start).count();

    start = std::chrono::steady_clock::now();
    auto part1 = aoc::accumulate(packets, size_t(0), [](size_t s, Packet const& p) -> size_t { return s + p.ver_sum(); });
    end = std::chrono::steady_clock::now();
    auto ns2 = (end - start).count();

    start = std::chrono::steady_clock::now();
    auto part2 = packets[0].evaluate();
    end = std::chrono::steady_clock::now();
    auto ns3 = (end - start).count();

    fmt::print("{} ({})\n", part1, ns2);
    fmt::print("{} ({})\n", part2, ns3);
    fmt::print("{} ({} + {} + {})\n", ns1 + ns2 + ns3, ns1, ns2, ns3);

    return 0;
}

#ifndef DSA_COMPRESSION_HUFFMAN_H
#define DSA_COMPRESSION_HUFFMAN_H

#include "algorithms/compression/bit_io.h"

#include <array>
#include <cmath>
#include <cstdint>
#include <map>
#include <queue>
#include <stdexcept>
#include <vector>

namespace dsa::compression {

// ---------------------------------------------------------------------------
// Huffman coding (canonical, static table)
//
// Build a prefix tree from symbol frequencies, derive code lengths, then turn
// the lengths into a *canonical* code. Canonical codes are important: the
// decoder only needs the lengths, not the tree, so the header stays small.
//
// The encoded format is:
//   [4 bytes] original byte count, big endian
//   [2 bytes] number of symbols in the table
//   [n bytes] (symbol, code length) pairs
//   [rest]    packed code bits, MSB first
//
// Time: O(n log k) to build, O(n) to encode/decode. Space: O(k).
// ---------------------------------------------------------------------------

struct HuffmanCode {
    std::uint32_t bits = 0;
    int length = 0;
};

inline std::array<std::uint64_t, 256> byteFrequencies(
    const std::vector<std::uint8_t>& input) {
    std::array<std::uint64_t, 256> frequencies{};
    for (std::uint8_t value : input) {
        ++frequencies[value];
    }
    return frequencies;
}

// Returns a code length for every symbol with a non-zero frequency.
inline std::array<std::uint8_t, 256> buildHuffmanLengths(
    const std::array<std::uint64_t, 256>& frequencies) {
    std::array<std::uint8_t, 256> lengths{};

    struct Node {
        std::uint64_t weight;
        int order;
        int left;
        int right;
        int symbol;
    };

    std::vector<Node> nodes;
    nodes.reserve(512);

    auto compare = [&nodes](int a, int b) {
        if (nodes[a].weight != nodes[b].weight) {
            return nodes[a].weight > nodes[b].weight;
        }
        return nodes[a].order > nodes[b].order;
    };
    std::priority_queue<int, std::vector<int>, decltype(compare)> queue(compare);

    int order = 0;
    int used = 0;
    for (int symbol = 0; symbol < 256; ++symbol) {
        if (frequencies[symbol] == 0) continue;
        nodes.push_back({frequencies[symbol], order++, -1, -1, symbol});
        queue.push(static_cast<int>(nodes.size()) - 1);
        ++used;
    }

    if (used == 0) return lengths;
    if (used == 1) {
        for (int symbol = 0; symbol < 256; ++symbol) {
            if (frequencies[symbol] != 0) lengths[symbol] = 1;
        }
        return lengths;
    }

    while (queue.size() > 1) {
        const int left = queue.top();
        queue.pop();
        const int right = queue.top();
        queue.pop();
        nodes.push_back({nodes[left].weight + nodes[right].weight, order++,
                         left, right, -1});
        queue.push(static_cast<int>(nodes.size()) - 1);
    }

    const int root = queue.top();
    std::vector<std::pair<int, int>> stack{{root, 0}};
    while (!stack.empty()) {
        const auto [index, depth] = stack.back();
        stack.pop_back();
        const Node& node = nodes[index];
        if (node.symbol >= 0) {
            lengths[node.symbol] =
                static_cast<std::uint8_t>(depth == 0 ? 1 : depth);
        } else {
            stack.push_back({node.left, depth + 1});
            stack.push_back({node.right, depth + 1});
        }
    }
    return lengths;
}

inline std::array<HuffmanCode, 256> buildCanonicalCodes(
    const std::array<std::uint8_t, 256>& lengths) {
    std::array<int, 257> count{};
    for (int symbol = 0; symbol < 256; ++symbol) {
        if (lengths[symbol] != 0) ++count[lengths[symbol]];
    }

    std::array<int, 257> firstCode{};
    int code = 0;
    for (int length = 1; length <= 256; ++length) {
        code = (code + count[length - 1]) << 1;
        firstCode[length] = code;
    }

    std::array<HuffmanCode, 256> codes{};
    for (int symbol = 0; symbol < 256; ++symbol) {
        const int length = lengths[symbol];
        if (length == 0) continue;
        codes[symbol].length = length;
        codes[symbol].bits = static_cast<std::uint32_t>(firstCode[length]++);
    }
    return codes;
}

inline std::vector<std::uint8_t> huffmanEncode(
    const std::vector<std::uint8_t>& input) {
    const auto frequencies = byteFrequencies(input);
    const auto lengths = buildHuffmanLengths(frequencies);
    const auto codes = buildCanonicalCodes(lengths);

    std::vector<std::uint8_t> output;
    const std::uint32_t originalSize = static_cast<std::uint32_t>(input.size());
    output.push_back(static_cast<std::uint8_t>((originalSize >> 24) & 0xFF));
    output.push_back(static_cast<std::uint8_t>((originalSize >> 16) & 0xFF));
    output.push_back(static_cast<std::uint8_t>((originalSize >> 8) & 0xFF));
    output.push_back(static_cast<std::uint8_t>(originalSize & 0xFF));

    int symbolCount = 0;
    for (int symbol = 0; symbol < 256; ++symbol) {
        if (lengths[symbol] != 0) ++symbolCount;
    }
    output.push_back(static_cast<std::uint8_t>((symbolCount >> 8) & 0xFF));
    output.push_back(static_cast<std::uint8_t>(symbolCount & 0xFF));
    for (int symbol = 0; symbol < 256; ++symbol) {
        if (lengths[symbol] == 0) continue;
        output.push_back(static_cast<std::uint8_t>(symbol));
        output.push_back(lengths[symbol]);
    }

    BitWriter writer(output);
    for (std::uint8_t value : input) {
        writer.writeBits(codes[value].bits, codes[value].length);
    }
    writer.flush();
    return output;
}

inline std::vector<std::uint8_t> huffmanDecode(
    const std::vector<std::uint8_t>& encoded) {
    if (encoded.size() < 6) {
        throw std::invalid_argument("huffmanDecode: header is too short");
    }

    const std::uint32_t originalSize =
        (static_cast<std::uint32_t>(encoded[0]) << 24) |
        (static_cast<std::uint32_t>(encoded[1]) << 16) |
        (static_cast<std::uint32_t>(encoded[2]) << 8) |
        static_cast<std::uint32_t>(encoded[3]);

    const int symbolCount = (static_cast<int>(encoded[4]) << 8) | encoded[5];
    std::size_t position = 6;
    if (encoded.size() < position + static_cast<std::size_t>(symbolCount) * 2) {
        throw std::invalid_argument("huffmanDecode: truncated code table");
    }

    std::array<std::uint8_t, 256> lengths{};
    for (int i = 0; i < symbolCount; ++i) {
        const std::uint8_t symbol = encoded[position++];
        const std::uint8_t length = encoded[position++];
        if (length == 0 || length > 32) {
            throw std::invalid_argument("huffmanDecode: invalid code length");
        }
        lengths[symbol] = length;
    }

    const auto codes = buildCanonicalCodes(lengths);
    std::array<std::map<std::uint32_t, std::uint8_t>, 33> table;
    for (int symbol = 0; symbol < 256; ++symbol) {
        if (lengths[symbol] == 0) continue;
        table[lengths[symbol]][codes[symbol].bits] =
            static_cast<std::uint8_t>(symbol);
    }

    std::vector<std::uint8_t> output;
    output.reserve(originalSize);

    BitReader reader(encoded, position);
    std::uint32_t code = 0;
    int length = 0;
    while (output.size() < originalSize) {
        const int bit = reader.readBit();
        if (bit < 0) {
            throw std::runtime_error("huffmanDecode: truncated bitstream");
        }
        code = (code << 1) | static_cast<std::uint32_t>(bit);
        ++length;
        if (length > 32) {
            throw std::runtime_error("huffmanDecode: invalid code");
        }
        const auto found = table[length].find(code);
        if (found != table[length].end()) {
            output.push_back(found->second);
            code = 0;
            length = 0;
        }
    }
    return output;
}

// Shannon entropy of the input in bits per byte, useful for reasoning about
// how close a coder is to the theoretical limit.
inline double byteEntropy(const std::vector<std::uint8_t>& input) {
    if (input.empty()) return 0.0;
    const auto frequencies = byteFrequencies(input);
    const double total = static_cast<double>(input.size());
    double entropy = 0.0;
    for (std::uint64_t frequency : frequencies) {
        if (frequency == 0) continue;
        const double probability = static_cast<double>(frequency) / total;
        entropy -= probability * std::log2(probability);
    }
    return entropy;
}

} // namespace dsa::compression

#endif

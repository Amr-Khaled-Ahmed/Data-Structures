#ifndef DSA_COMPRESSION_ADAPTIVE_HUFFMAN_H
#define DSA_COMPRESSION_ADAPTIVE_HUFFMAN_H

#include "algorithms/compression/bit_io.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace dsa::compression {

// ---------------------------------------------------------------------------
// Adaptive Huffman coding (FGK: Faller-Gallager-Knuth)
//
// A static Huffman coder has to send the code table before the data. Adaptive
// Huffman skips that: encoder and decoder build the same tree on the fly and
// update it after every symbol. New symbols are introduced through a special
// NYT ("not yet transmitted") leaf, which is split into the new symbol and a
// fresh NYT.
//
// The update step is the interesting part: a node is swapped with the
// highest-order node of the same weight before its weight is incremented, which
// keeps the tree a valid Huffman tree (the "sibling property").
//
// Time: O(n * alphabet) because finding the leader scans the order array.
// Space: O(alphabet).
// ---------------------------------------------------------------------------

class AdaptiveHuffman {
public:
    AdaptiveHuffman() { reset(); }

    void encodeSymbol(std::uint8_t symbol, BitWriter& writer) {
        const int existing = symbolNode_[symbol];
        if (existing >= 0) {
            writeCode(existing, writer);
            update(existing);
        } else {
            writeCode(nytNode_, writer);
            writer.writeBits(symbol, 8);
            spawn(symbol);
        }
    }

    std::uint8_t decodeSymbol(BitReader& reader) {
        int node = root_;
        while (!nodes_[node].isNyt && nodes_[node].symbol < 0) {
            const int bit = reader.readBit();
            if (bit < 0) {
                throw std::runtime_error(
                    "adaptiveHuffmanDecode: truncated bitstream");
            }
            node = bit == 0 ? nodes_[node].left : nodes_[node].right;
        }

        if (nodes_[node].isNyt) {
            const int symbol = reader.readBits(8);
            if (symbol < 0) {
                throw std::runtime_error(
                    "adaptiveHuffmanDecode: truncated new symbol");
            }
            spawn(static_cast<std::uint8_t>(symbol));
            return static_cast<std::uint8_t>(symbol);
        }

        const int symbol = nodes_[node].symbol;
        update(node);
        return static_cast<std::uint8_t>(symbol);
    }

private:
    struct Node {
        int symbol = -1;
        long long weight = 0;
        int order = 0;
        int parent = -1;
        int left = -1;
        int right = -1;
        bool isNyt = false;
    };

    void reset() {
        nodes_.clear();
        symbolNode_.fill(-1);
        orderNode_.fill(-1);
        nextOrder_ = 512;
        root_ = createNode(-1, 0, nextOrder_--, true);
        nytNode_ = root_;
    }

    int createNode(int symbol, long long weight, int order, bool isNyt) {
        nodes_.push_back({symbol, weight, order, -1, -1, -1, isNyt});
        const int index = static_cast<int>(nodes_.size()) - 1;
        orderNode_[order] = index;
        return index;
    }

    void spawn(std::uint8_t symbol) {
        const int internal = nytNode_;
        nodes_[internal].isNyt = false;
        nodes_[internal].symbol = -1;

        const int symbolLeaf = createNode(symbol, 1, nextOrder_--, false);
        const int newNyt = createNode(-1, 0, nextOrder_--, true);

        nodes_[internal].left = symbolLeaf;
        nodes_[internal].right = newNyt;
        nodes_[internal].weight = 1;
        nodes_[symbolLeaf].parent = internal;
        nodes_[newNyt].parent = internal;

        symbolNode_[symbol] = symbolLeaf;
        nytNode_ = newNyt;

        update(nodes_[internal].parent);
    }

    void update(int node) {
        while (node >= 0) {
            const int leader = findLeader(node);
            if (leader != node && leader != nodes_[node].parent) {
                swapNodes(node, leader);
            }
            ++nodes_[node].weight;
            node = nodes_[node].parent;
        }
    }

    int findLeader(int node) const {
        const long long weight = nodes_[node].weight;
        for (int order = 512; order >= 0; --order) {
            const int candidate = orderNode_[order];
            if (candidate < 0) continue;
            if (nodes_[candidate].weight != weight) continue;
            if (candidate == node) return node;
            if (isAncestor(candidate, node)) return nodes_[node].parent;
            return candidate;
        }
        return node;
    }

    bool isAncestor(int candidate, int node) const {
        int current = nodes_[node].parent;
        while (current >= 0) {
            if (current == candidate) return true;
            current = nodes_[current].parent;
        }
        return false;
    }

    void swapNodes(int a, int b) {
        const int parentA = nodes_[a].parent;
        const int parentB = nodes_[b].parent;
        if (parentA < 0 || parentB < 0) return;

        if (nodes_[parentA].left == a) {
            nodes_[parentA].left = b;
        } else {
            nodes_[parentA].right = b;
        }
        if (nodes_[parentB].left == b) {
            nodes_[parentB].left = a;
        } else {
            nodes_[parentB].right = a;
        }

        nodes_[a].parent = parentB;
        nodes_[b].parent = parentA;
        std::swap(nodes_[a].order, nodes_[b].order);
        orderNode_[nodes_[a].order] = a;
        orderNode_[nodes_[b].order] = b;
    }

    void writeCode(int node, BitWriter& writer) const {
        std::vector<int> bits;
        int current = node;
        while (nodes_[current].parent >= 0) {
            const int parent = nodes_[current].parent;
            bits.push_back(nodes_[parent].right == current ? 1 : 0);
            current = parent;
        }
        for (auto it = bits.rbegin(); it != bits.rend(); ++it) {
            writer.writeBit(*it);
        }
    }

    std::vector<Node> nodes_;
    std::array<int, 256> symbolNode_{};
    std::array<int, 513> orderNode_{};
    int nextOrder_ = 512;
    int root_ = -1;
    int nytNode_ = -1;
};

inline std::vector<std::uint8_t> adaptiveHuffmanEncode(
    const std::vector<std::uint8_t>& input) {
    std::vector<std::uint8_t> output;
    const std::uint32_t originalSize = static_cast<std::uint32_t>(input.size());
    output.push_back(static_cast<std::uint8_t>((originalSize >> 24) & 0xFF));
    output.push_back(static_cast<std::uint8_t>((originalSize >> 16) & 0xFF));
    output.push_back(static_cast<std::uint8_t>((originalSize >> 8) & 0xFF));
    output.push_back(static_cast<std::uint8_t>(originalSize & 0xFF));

    AdaptiveHuffman coder;
    BitWriter writer(output);
    for (std::uint8_t symbol : input) {
        coder.encodeSymbol(symbol, writer);
    }
    writer.flush();
    return output;
}

inline std::vector<std::uint8_t> adaptiveHuffmanDecode(
    const std::vector<std::uint8_t>& encoded) {
    if (encoded.size() < 4) {
        throw std::invalid_argument(
            "adaptiveHuffmanDecode: missing size header");
    }
    const std::uint32_t originalSize =
        (static_cast<std::uint32_t>(encoded[0]) << 24) |
        (static_cast<std::uint32_t>(encoded[1]) << 16) |
        (static_cast<std::uint32_t>(encoded[2]) << 8) |
        static_cast<std::uint32_t>(encoded[3]);

    AdaptiveHuffman coder;
    BitReader reader(encoded, 4);
    std::vector<std::uint8_t> output;
    output.reserve(originalSize);
    for (std::uint32_t i = 0; i < originalSize; ++i) {
        output.push_back(coder.decodeSymbol(reader));
    }
    return output;
}

} // namespace dsa::compression

#endif

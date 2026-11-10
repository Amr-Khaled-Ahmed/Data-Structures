#ifndef DSA_COMPRESSION_MODIFIED_HUFFMAN_H
#define DSA_COMPRESSION_MODIFIED_HUFFMAN_H

#include "algorithms/compression/huffman.h"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace dsa::compression {

// ---------------------------------------------------------------------------
// Modified Huffman (teaching variant)
//
// The real Modified Huffman code is the T.4 one-dimensional fax standard: it
// assigns fixed codes to runs of white and black pixels. Implementing the full
// T.4 tables is mostly data entry, so this file teaches the *idea* used by that
// standard instead:
//
//   1. turn the input into runs of equal bytes,
//   2. Huffman-code the run lengths (the part that dominates fax images),
//   3. store the run values raw.
//
// The result is a valid, self-describing stream and round-trips exactly, but
// it is not bit-compatible with a T.4 fax machine.
//
// Time: O(n log k). Space: O(n).
// ---------------------------------------------------------------------------

inline std::vector<std::uint8_t> modifiedHuffmanEncode(
    const std::vector<std::uint8_t>& input) {
    std::vector<std::uint8_t> counts;
    std::vector<std::uint8_t> values;

    std::size_t i = 0;
    while (i < input.size()) {
        const std::uint8_t value = input[i];
        std::size_t run = 1;
        while (i + run < input.size() && input[i + run] == value && run < 255) {
            ++run;
        }
        counts.push_back(static_cast<std::uint8_t>(run));
        values.push_back(value);
        i += run;
    }

    const std::vector<std::uint8_t> encodedCounts = huffmanEncode(counts);

    std::vector<std::uint8_t> output;
    const std::uint32_t originalSize = static_cast<std::uint32_t>(input.size());
    output.push_back(static_cast<std::uint8_t>((originalSize >> 24) & 0xFF));
    output.push_back(static_cast<std::uint8_t>((originalSize >> 16) & 0xFF));
    output.push_back(static_cast<std::uint8_t>((originalSize >> 8) & 0xFF));
    output.push_back(static_cast<std::uint8_t>(originalSize & 0xFF));

    const std::uint32_t pairCount =
        static_cast<std::uint32_t>(counts.size());
    output.push_back(static_cast<std::uint8_t>((pairCount >> 24) & 0xFF));
    output.push_back(static_cast<std::uint8_t>((pairCount >> 16) & 0xFF));
    output.push_back(static_cast<std::uint8_t>((pairCount >> 8) & 0xFF));
    output.push_back(static_cast<std::uint8_t>(pairCount & 0xFF));

    const std::uint32_t countStreamSize =
        static_cast<std::uint32_t>(encodedCounts.size());
    output.push_back(static_cast<std::uint8_t>((countStreamSize >> 24) & 0xFF));
    output.push_back(static_cast<std::uint8_t>((countStreamSize >> 16) & 0xFF));
    output.push_back(static_cast<std::uint8_t>((countStreamSize >> 8) & 0xFF));
    output.push_back(static_cast<std::uint8_t>(countStreamSize & 0xFF));

    output.insert(output.end(), encodedCounts.begin(), encodedCounts.end());
    output.insert(output.end(), values.begin(), values.end());
    return output;
}

inline std::vector<std::uint8_t> modifiedHuffmanDecode(
    const std::vector<std::uint8_t>& encoded) {
    if (encoded.size() < 12) {
        throw std::invalid_argument(
            "modifiedHuffmanDecode: header is too short");
    }

    const std::uint32_t pairCount =
        (static_cast<std::uint32_t>(encoded[4]) << 24) |
        (static_cast<std::uint32_t>(encoded[5]) << 16) |
        (static_cast<std::uint32_t>(encoded[6]) << 8) |
        static_cast<std::uint32_t>(encoded[7]);
    const std::uint32_t countStreamSize =
        (static_cast<std::uint32_t>(encoded[8]) << 24) |
        (static_cast<std::uint32_t>(encoded[9]) << 16) |
        (static_cast<std::uint32_t>(encoded[10]) << 8) |
        static_cast<std::uint32_t>(encoded[11]);

    if (encoded.size() < 12 + static_cast<std::size_t>(countStreamSize) +
                              static_cast<std::size_t>(pairCount)) {
        throw std::invalid_argument(
            "modifiedHuffmanDecode: truncated payload");
    }

    const std::vector<std::uint8_t> countStream(
        encoded.begin() + 12,
        encoded.begin() + 12 + static_cast<std::ptrdiff_t>(countStreamSize));
    const std::vector<std::uint8_t> counts = huffmanDecode(countStream);
    if (counts.size() != pairCount) {
        throw std::runtime_error(
            "modifiedHuffmanDecode: run-length count mismatch");
    }

    const std::size_t valuesStart =
        12 + static_cast<std::size_t>(countStreamSize);
    std::vector<std::uint8_t> output;
    for (std::uint32_t i = 0; i < pairCount; ++i) {
        output.insert(output.end(), counts[i], encoded[valuesStart + i]);
    }
    return output;
}

} // namespace dsa::compression

#endif

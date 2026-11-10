#ifndef DSA_COMPRESSION_LZ78_H
#define DSA_COMPRESSION_LZ78_H

#include <cstdint>
#include <map>
#include <stdexcept>
#include <vector>

namespace dsa::compression {

// ---------------------------------------------------------------------------
// LZ78 (Lempel-Ziv 1978)
//
// LZ78 builds an explicit dictionary of phrases. For every new phrase it emits
// (index of the longest known prefix, the one byte that extends it). Unlike
// LZ77 there is no sliding window: the dictionary grows for the whole stream.
//
// Format:
//   [4 bytes] token count, big endian
//   [3 bytes] per token: 16-bit dictionary index, then one byte
//
// Time: O(n * phrase length) expected. Space: O(n).
// ---------------------------------------------------------------------------

struct LZ78Token {
    std::uint16_t index = 0;
    std::uint8_t next = 0;
};

inline std::vector<LZ78Token> lz78Tokenize(
    const std::vector<std::uint8_t>& input) {
    std::map<std::vector<std::uint8_t>, std::uint16_t> dictionary;
    std::uint16_t nextIndex = 1;

    std::vector<LZ78Token> tokens;
    std::vector<std::uint8_t> current;

    for (std::uint8_t value : input) {
        current.push_back(value);
        if (dictionary.find(current) != dictionary.end()) {
            continue;
        }

        std::uint16_t index = 0;
        if (current.size() > 1) {
            std::vector<std::uint8_t> prefix(current.begin(), current.end() - 1);
            index = dictionary.at(prefix);
        }
        tokens.push_back({index, value});
        if (nextIndex < 65535) {
            dictionary[current] = nextIndex++;
        }
        current.clear();
    }

    if (!current.empty()) {
        std::vector<std::uint8_t> prefix(current.begin(), current.end() - 1);
        const std::uint16_t index =
            prefix.empty() ? 0 : dictionary.at(prefix);
        tokens.push_back({index, current.back()});
    }
    return tokens;
}

inline std::vector<std::uint8_t> lz78Encode(
    const std::vector<std::uint8_t>& input) {
    const auto tokens = lz78Tokenize(input);

    std::vector<std::uint8_t> output;
    const std::uint32_t count = static_cast<std::uint32_t>(tokens.size());
    output.push_back(static_cast<std::uint8_t>((count >> 24) & 0xFF));
    output.push_back(static_cast<std::uint8_t>((count >> 16) & 0xFF));
    output.push_back(static_cast<std::uint8_t>((count >> 8) & 0xFF));
    output.push_back(static_cast<std::uint8_t>(count & 0xFF));

    for (const LZ78Token& token : tokens) {
        output.push_back(static_cast<std::uint8_t>((token.index >> 8) & 0xFF));
        output.push_back(static_cast<std::uint8_t>(token.index & 0xFF));
        output.push_back(token.next);
    }
    return output;
}

inline std::vector<std::uint8_t> lz78Decode(
    const std::vector<std::uint8_t>& encoded) {
    if (encoded.size() < 4) {
        throw std::invalid_argument("lz78Decode: missing token count");
    }
    const std::uint32_t count =
        (static_cast<std::uint32_t>(encoded[0]) << 24) |
        (static_cast<std::uint32_t>(encoded[1]) << 16) |
        (static_cast<std::uint32_t>(encoded[2]) << 8) |
        static_cast<std::uint32_t>(encoded[3]);

    if (encoded.size() != 4 + static_cast<std::size_t>(count) * 3) {
        throw std::invalid_argument("lz78Decode: truncated token stream");
    }

    std::vector<std::vector<std::uint8_t>> dictionary;
    std::vector<std::uint8_t> output;
    std::size_t position = 4;

    for (std::uint32_t i = 0; i < count; ++i) {
        const std::uint16_t index =
            static_cast<std::uint16_t>((encoded[position] << 8) |
                                       encoded[position + 1]);
        const std::uint8_t next = encoded[position + 2];
        position += 3;

        if (index > dictionary.size()) {
            throw std::runtime_error("lz78Decode: invalid dictionary index");
        }
        std::vector<std::uint8_t> entry =
            index == 0 ? std::vector<std::uint8_t>{}
                       : dictionary[index - 1];
        entry.push_back(next);
        output.insert(output.end(), entry.begin(), entry.end());
        dictionary.push_back(entry);
    }
    return output;
}

} // namespace dsa::compression

#endif

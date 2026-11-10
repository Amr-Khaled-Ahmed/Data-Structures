#ifndef DSA_COMPRESSION_LZ77_H
#define DSA_COMPRESSION_LZ77_H

#include <cstdint>
#include <stdexcept>
#include <vector>

namespace dsa::compression {

// ---------------------------------------------------------------------------
// LZ77 (Lempel-Ziv 1977)
//
// The encoder keeps a sliding window of already-seen bytes and emits a token
// for each position:
//
//   (offset, length, next)
//
// offset == 0 means "no match": the token is a single literal in `next`.
// Otherwise the decoder copies `length` bytes from `offset` bytes back in the
// output, then appends `next`.
//
// This is a greedy teaching implementation with a linear search inside the
// window. Real LZ77-family codecs (DEFLATE, zlib) use hash chains / suffix
// structures to find matches faster.
//
// Time: O(n * window * maxMatch) worst case. Space: O(n).
// ---------------------------------------------------------------------------

struct LZ77Token {
    std::uint16_t offset = 0;
    std::uint16_t length = 0;
    std::uint8_t next = 0;
};

inline std::vector<LZ77Token> lz77Tokenize(
    const std::vector<std::uint8_t>& input,
    int window = 4096,
    int maxMatch = 258) {
    std::vector<LZ77Token> tokens;
    std::size_t i = 0;

    while (i < input.size()) {
        int bestLength = 0;
        int bestOffset = 0;

        const std::size_t windowStart =
            i > static_cast<std::size_t>(window) ? i - window : 0;

        for (std::size_t candidate = windowStart; candidate < i; ++candidate) {
            int length = 0;
            while (length < maxMatch &&
                   i + static_cast<std::size_t>(length) < input.size() &&
                   input[candidate + static_cast<std::size_t>(length)] ==
                       input[i + static_cast<std::size_t>(length)]) {
                ++length;
            }
            if (length > bestLength) {
                bestLength = length;
                bestOffset = static_cast<int>(i - candidate);
            }
        }

        // A token always carries a trailing literal. If the match reaches the
        // end of the input, shorten it by one so `next` stays valid.
        if (bestLength >= 3) {
            if (i + static_cast<std::size_t>(bestLength) >= input.size()) {
                if (bestLength > 3) {
                    --bestLength;
                } else {
                    tokens.push_back({0, 0, input[i]});
                    ++i;
                    continue;
                }
            }
            const std::uint8_t next =
                input[i + static_cast<std::size_t>(bestLength)];
            tokens.push_back({static_cast<std::uint16_t>(bestOffset),
                              static_cast<std::uint16_t>(bestLength), next});
            i += static_cast<std::size_t>(bestLength) + 1;
        } else {
            tokens.push_back({0, 0, input[i]});
            ++i;
        }
    }
    return tokens;
}

inline std::vector<std::uint8_t> lz77Encode(
    const std::vector<std::uint8_t>& input,
    int window = 4096,
    int maxMatch = 258) {
    const auto tokens = lz77Tokenize(input, window, maxMatch);

    std::vector<std::uint8_t> output;
    const std::uint32_t count = static_cast<std::uint32_t>(tokens.size());
    output.push_back(static_cast<std::uint8_t>((count >> 24) & 0xFF));
    output.push_back(static_cast<std::uint8_t>((count >> 16) & 0xFF));
    output.push_back(static_cast<std::uint8_t>((count >> 8) & 0xFF));
    output.push_back(static_cast<std::uint8_t>(count & 0xFF));

    for (const LZ77Token& token : tokens) {
        output.push_back(static_cast<std::uint8_t>((token.offset >> 8) & 0xFF));
        output.push_back(static_cast<std::uint8_t>(token.offset & 0xFF));
        output.push_back(static_cast<std::uint8_t>((token.length >> 8) & 0xFF));
        output.push_back(static_cast<std::uint8_t>(token.length & 0xFF));
        output.push_back(token.next);
    }
    return output;
}

inline std::vector<std::uint8_t> lz77Decode(
    const std::vector<std::uint8_t>& encoded) {
    if (encoded.size() < 4) {
        throw std::invalid_argument("lz77Decode: missing token count");
    }
    const std::uint32_t count =
        (static_cast<std::uint32_t>(encoded[0]) << 24) |
        (static_cast<std::uint32_t>(encoded[1]) << 16) |
        (static_cast<std::uint32_t>(encoded[2]) << 8) |
        static_cast<std::uint32_t>(encoded[3]);

    if (encoded.size() != 4 + static_cast<std::size_t>(count) * 5) {
        throw std::invalid_argument("lz77Decode: truncated token stream");
    }

    std::vector<std::uint8_t> output;
    std::size_t position = 4;
    for (std::uint32_t i = 0; i < count; ++i) {
        const std::uint16_t offset =
            static_cast<std::uint16_t>((encoded[position] << 8) |
                                       encoded[position + 1]);
        const std::uint16_t length =
            static_cast<std::uint16_t>((encoded[position + 2] << 8) |
                                       encoded[position + 3]);
        const std::uint8_t next = encoded[position + 4];
        position += 5;

        if (length == 0) {
            output.push_back(next);
            continue;
        }
        if (offset == 0 || offset > output.size()) {
            throw std::runtime_error("lz77Decode: invalid match offset");
        }
        for (std::uint16_t j = 0; j < length; ++j) {
            output.push_back(output[output.size() - offset]);
        }
        output.push_back(next);
    }
    return output;
}

} // namespace dsa::compression

#endif

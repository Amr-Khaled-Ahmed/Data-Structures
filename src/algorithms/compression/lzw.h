#ifndef DSA_COMPRESSION_LZW_H
#define DSA_COMPRESSION_LZW_H

#include "algorithms/compression/bit_io.h"

#include <cstdint>
#include <map>
#include <stdexcept>
#include <vector>

namespace dsa::compression {

// ---------------------------------------------------------------------------
// LZW (Lempel-Ziv-Welch)
//
// LZW starts with a dictionary of the 256 single-byte strings and grows it as
// it reads. Codes are 12 bits, which gives at most 4096 entries. Code 256 is
// reserved as the "clear" instruction that resets the dictionary; this is what
// GIF and TIFF use.
//
// The stream is self-terminating because the original size is stored first.
//
// Time: O(n) expected. Space: O(dictionary).
// ---------------------------------------------------------------------------

inline std::vector<std::uint8_t> lzwEncode(
    const std::vector<std::uint8_t>& input) {
    std::vector<std::uint8_t> output;
    const std::uint32_t originalSize = static_cast<std::uint32_t>(input.size());
    output.push_back(static_cast<std::uint8_t>((originalSize >> 24) & 0xFF));
    output.push_back(static_cast<std::uint8_t>((originalSize >> 16) & 0xFF));
    output.push_back(static_cast<std::uint8_t>((originalSize >> 8) & 0xFF));
    output.push_back(static_cast<std::uint8_t>(originalSize & 0xFF));

    std::map<std::vector<std::uint8_t>, std::uint16_t> dictionary;
    for (int value = 0; value < 256; ++value) {
        const std::vector<std::uint8_t> key{
            static_cast<std::uint8_t>(value)};
        dictionary[key] = static_cast<std::uint16_t>(value);
    }
    std::uint16_t nextCode = 257;

    BitWriter writer(output);
    std::vector<std::uint8_t> current;

    for (std::uint8_t value : input) {
        current.push_back(value);
        if (dictionary.find(current) != dictionary.end()) {
            continue;
        }

        std::vector<std::uint8_t> prefix(current.begin(), current.end() - 1);
        writer.writeBits(dictionary.at(prefix), 12);

        if (nextCode < 4096) {
            dictionary[current] = nextCode++;
        } else {
            writer.writeBits(256, 12);
            dictionary.clear();
            for (int i = 0; i < 256; ++i) {
                const std::vector<std::uint8_t> key{
                    static_cast<std::uint8_t>(i)};
                dictionary[key] = static_cast<std::uint16_t>(i);
            }
            nextCode = 257;
        }
        current.assign(1, value);
    }

    if (!current.empty()) {
        writer.writeBits(dictionary.at(current), 12);
    }
    writer.flush();
    return output;
}

inline std::vector<std::uint8_t> lzwDecode(
    const std::vector<std::uint8_t>& encoded) {
    if (encoded.size() < 4) {
        throw std::invalid_argument("lzwDecode: missing size header");
    }
    const std::uint32_t originalSize =
        (static_cast<std::uint32_t>(encoded[0]) << 24) |
        (static_cast<std::uint32_t>(encoded[1]) << 16) |
        (static_cast<std::uint32_t>(encoded[2]) << 8) |
        static_cast<std::uint32_t>(encoded[3]);

    std::vector<std::vector<std::uint8_t>> dictionary;
    dictionary.reserve(4096);
    for (int value = 0; value < 256; ++value) {
        dictionary.push_back({static_cast<std::uint8_t>(value)});
    }
    // Code 256 is reserved as "clear", so keep an empty placeholder there.
    // The next free code is therefore dictionary.size() == 257.
    dictionary.push_back({});

    BitReader reader(encoded, 4);
    std::vector<std::uint8_t> output;
    output.reserve(originalSize);
    std::vector<std::uint8_t> previous;

    while (output.size() < originalSize) {
        const int code = reader.readBits(12);
        if (code < 0) {
            throw std::runtime_error("lzwDecode: truncated bitstream");
        }
        if (code == 256) {
            dictionary.resize(256);
            dictionary.push_back({});
            previous.clear();
            continue;
        }

        std::vector<std::uint8_t> entry;
        if (code < static_cast<int>(dictionary.size())) {
            entry = dictionary[static_cast<std::size_t>(code)];
        } else if (code == static_cast<int>(dictionary.size()) &&
                   !previous.empty()) {
            entry = previous;
            entry.push_back(previous.front());
        } else {
            throw std::runtime_error("lzwDecode: invalid code");
        }

        output.insert(output.end(), entry.begin(), entry.end());
        if (!previous.empty() && dictionary.size() < 4096) {
            std::vector<std::uint8_t> newEntry = previous;
            newEntry.push_back(entry.front());
            dictionary.push_back(newEntry);
        }
        previous = entry;
    }

    output.resize(originalSize);
    return output;
}

} // namespace dsa::compression

#endif

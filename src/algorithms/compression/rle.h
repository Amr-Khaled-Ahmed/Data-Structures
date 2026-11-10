#ifndef DSA_COMPRESSION_RLE_H
#define DSA_COMPRESSION_RLE_H

#include <cstdint>
#include <stdexcept>
#include <vector>

namespace dsa::compression {

// ---------------------------------------------------------------------------
// Run-Length Encoding (RLE)
//
// A run is stored as a (count, value) pair. Runs longer than 255 are split
// into several pairs. RLE is excellent for data with long repeats (icons,
// fax images, simple bitmaps) and can make arbitrary data larger, so it is
// usually a first stage in front of a stronger coder.
//
// Time: O(n). Space: O(n) worst case.
// ---------------------------------------------------------------------------

inline std::vector<std::uint8_t> rleEncode(
    const std::vector<std::uint8_t>& input) {
    std::vector<std::uint8_t> output;
    output.reserve(input.size());

    std::size_t i = 0;
    while (i < input.size()) {
        const std::uint8_t value = input[i];
        std::size_t run = 1;
        while (i + run < input.size() && input[i + run] == value && run < 255) {
            ++run;
        }
        output.push_back(static_cast<std::uint8_t>(run));
        output.push_back(value);
        i += run;
    }
    return output;
}

inline std::vector<std::uint8_t> rleDecode(
    const std::vector<std::uint8_t>& encoded) {
    if (encoded.size() % 2 != 0) {
        throw std::invalid_argument("rleDecode: truncated (count, value) pair");
    }

    std::vector<std::uint8_t> output;
    for (std::size_t i = 0; i + 1 < encoded.size(); i += 2) {
        output.insert(output.end(), encoded[i], encoded[i + 1]);
    }
    return output;
}

} // namespace dsa::compression

#endif

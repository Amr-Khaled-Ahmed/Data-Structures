#ifndef DSA_COMPRESSION_DPCM_H
#define DSA_COMPRESSION_DPCM_H

#include <algorithm>
#include <cstdint>
#include <vector>

namespace dsa::compression {

// ---------------------------------------------------------------------------
// Differential Pulse-Code Modulation (DPCM)
//
// Neighbouring samples in images, audio and sensor streams are usually
// similar. DPCM stores the *difference* from the previous sample instead of
// the sample itself. The differences cluster around zero, so a later entropy
// coder (Huffman, arithmetic) needs fewer bits.
//
// This is exactly the DC predictor used by JPEG: every 8x8 block stores the
// difference between its DC coefficient and the previous block's DC.
//
// Time: O(n). Space: O(n).
// ---------------------------------------------------------------------------

inline std::vector<int> dpcmEncode(const std::vector<std::uint8_t>& input) {
    std::vector<int> residuals;
    residuals.reserve(input.size());
    int previous = 0;
    for (std::uint8_t value : input) {
        residuals.push_back(static_cast<int>(value) - previous);
        previous = value;
    }
    return residuals;
}

inline std::vector<std::uint8_t> dpcmDecode(const std::vector<int>& residuals) {
    std::vector<std::uint8_t> output;
    output.reserve(residuals.size());
    int previous = 0;
    for (int residual : residuals) {
        int value = previous + residual;
        value = std::clamp(value, 0, 255);
        output.push_back(static_cast<std::uint8_t>(value));
        previous = value;
    }
    return output;
}

// Quantized DPCM: the residual is divided by `step`, transmitted as an integer,
// and multiplied back by the decoder. This is the simplest form of lossy
// predictive coding.
inline std::vector<int> dpcmEncodeQuantized(
    const std::vector<std::uint8_t>& input, int step) {
    if (step <= 0) return dpcmEncode(input);
    std::vector<int> residuals;
    residuals.reserve(input.size());
    int previous = 0;
    for (std::uint8_t value : input) {
        const int difference = static_cast<int>(value) - previous;
        const int quantized =
            difference >= 0 ? (difference + step / 2) / step
                            : -((-difference + step / 2) / step);
        residuals.push_back(quantized);
        previous = static_cast<int>(value);
    }
    return residuals;
}

inline std::vector<std::uint8_t> dpcmDecodeQuantized(
    const std::vector<int>& residuals, int step) {
    if (step <= 0) return dpcmDecode(residuals);
    std::vector<std::uint8_t> output;
    output.reserve(residuals.size());
    int previous = 0;
    for (int residual : residuals) {
        int value = previous + residual * step;
        value = std::clamp(value, 0, 255);
        output.push_back(static_cast<std::uint8_t>(value));
        previous = value;
    }
    return output;
}

} // namespace dsa::compression

#endif

#ifndef DSA_COMPRESSION_ARITHMETIC_CODING_H
#define DSA_COMPRESSION_ARITHMETIC_CODING_H

#include "algorithms/compression/bit_io.h"
#include "algorithms/compression/huffman.h"

#include <array>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace dsa::compression {

// ---------------------------------------------------------------------------
// Arithmetic coding (static order-0 model)
//
// Arithmetic coding maps the whole message to one number in [0, 1) instead of
// giving every symbol an integer number of bits. It can get within a few bits
// of the entropy limit, which is why it beats Huffman when symbol
// probabilities are very skewed.
//
// Two forms are provided:
//   arithmeticEncode / arithmeticDecode           32-bit binary intervals
//   arithmeticEncodeFloat / arithmeticDecodeFloat long double intervals
//
// The float version is the textbook presentation (subdivide [0,1), emit bits
// while the interval stays in one half). The binary version is what real
// codecs use: it keeps the same idea with integer low/high registers and the
// E1/E2/E3 renormalisation rules.
//
// Both store the frequency table in the stream, so the decoder needs no side
// information. Time: O(n * alphabet). Space: O(alphabet).
// ---------------------------------------------------------------------------

namespace detail {

struct ArithmeticModel {
    std::array<std::uint32_t, 256> frequency{};
    std::array<std::uint64_t, 257> cumulative{};
    std::uint64_t total = 0;
};

inline ArithmeticModel buildArithmeticModel(
    const std::vector<std::uint8_t>& input) {
    ArithmeticModel model;
    for (std::uint8_t value : input) ++model.frequency[value];
    for (int symbol = 0; symbol < 256; ++symbol) {
        model.cumulative[symbol] = model.total;
        model.total += model.frequency[symbol];
    }
    model.cumulative[256] = model.total;
    return model;
}

inline void writeArithmeticHeader(std::vector<std::uint8_t>& output,
                                  const ArithmeticModel& model,
                                  std::uint32_t originalSize) {
    output.push_back(static_cast<std::uint8_t>((originalSize >> 24) & 0xFF));
    output.push_back(static_cast<std::uint8_t>((originalSize >> 16) & 0xFF));
    output.push_back(static_cast<std::uint8_t>((originalSize >> 8) & 0xFF));
    output.push_back(static_cast<std::uint8_t>(originalSize & 0xFF));

    int symbolCount = 0;
    for (int symbol = 0; symbol < 256; ++symbol) {
        if (model.frequency[symbol] != 0) ++symbolCount;
    }
    output.push_back(static_cast<std::uint8_t>((symbolCount >> 8) & 0xFF));
    output.push_back(static_cast<std::uint8_t>(symbolCount & 0xFF));

    for (int symbol = 0; symbol < 256; ++symbol) {
        const std::uint32_t frequency = model.frequency[symbol];
        if (frequency == 0) continue;
        output.push_back(static_cast<std::uint8_t>(symbol));
        output.push_back(static_cast<std::uint8_t>((frequency >> 24) & 0xFF));
        output.push_back(static_cast<std::uint8_t>((frequency >> 16) & 0xFF));
        output.push_back(static_cast<std::uint8_t>((frequency >> 8) & 0xFF));
        output.push_back(static_cast<std::uint8_t>(frequency & 0xFF));
    }
}

inline ArithmeticModel readArithmeticHeader(const std::vector<std::uint8_t>& input,
                                            std::uint32_t& originalSize,
                                            std::size_t& position) {
    if (input.size() < 6) {
        throw std::invalid_argument("arithmeticDecode: header is too short");
    }
    originalSize = (static_cast<std::uint32_t>(input[0]) << 24) |
                   (static_cast<std::uint32_t>(input[1]) << 16) |
                   (static_cast<std::uint32_t>(input[2]) << 8) |
                   static_cast<std::uint32_t>(input[3]);
    const int symbolCount = (static_cast<int>(input[4]) << 8) | input[5];
    position = 6;
    if (input.size() < position + static_cast<std::size_t>(symbolCount) * 5) {
        throw std::invalid_argument("arithmeticDecode: truncated table");
    }

    ArithmeticModel model;
    for (int i = 0; i < symbolCount; ++i) {
        const std::uint8_t symbol = input[position++];
        const std::uint32_t frequency =
            (static_cast<std::uint32_t>(input[position]) << 24) |
            (static_cast<std::uint32_t>(input[position + 1]) << 16) |
            (static_cast<std::uint32_t>(input[position + 2]) << 8) |
            static_cast<std::uint32_t>(input[position + 3]);
        position += 4;
        model.frequency[symbol] = frequency;
    }
    for (int symbol = 0; symbol < 256; ++symbol) {
        model.cumulative[symbol] = model.total;
        model.total += model.frequency[symbol];
    }
    model.cumulative[256] = model.total;
    return model;
}

inline int findSymbol(const ArithmeticModel& model, std::uint64_t cumulativeValue) {
    for (int symbol = 0; symbol < 256; ++symbol) {
        if (model.frequency[symbol] == 0) continue;
        if (cumulativeValue >= model.cumulative[symbol] &&
            cumulativeValue < model.cumulative[symbol + 1]) {
            return symbol;
        }
    }
    throw std::runtime_error("arithmeticDecode: cumulative value out of range");
}

} // namespace detail

inline std::vector<std::uint8_t> arithmeticEncode(
    const std::vector<std::uint8_t>& input) {
    const auto model = detail::buildArithmeticModel(input);
    std::vector<std::uint8_t> output;
    detail::writeArithmeticHeader(output, model,
                                  static_cast<std::uint32_t>(input.size()));

    constexpr std::uint32_t kHalf = 0x80000000u;
    constexpr std::uint32_t kQuarter = 0x40000000u;
    constexpr std::uint32_t kThreeQuarters = 0xC0000000u;

    BitWriter writer(output);
    std::uint32_t low = 0;
    std::uint32_t high = 0xFFFFFFFFu;
    std::uint64_t pending = 0;

    auto emit = [&](int bit) {
        writer.writeBit(bit);
        while (pending > 0) {
            writer.writeBit(bit ^ 1);
            --pending;
        }
    };

    for (std::uint8_t symbol : input) {
        const std::uint64_t range =
            static_cast<std::uint64_t>(high) - low + 1;
        high = low + static_cast<std::uint32_t>(
                         (range * model.cumulative[symbol + 1]) / model.total - 1);
        low = low + static_cast<std::uint32_t>(
                        (range * model.cumulative[symbol]) / model.total);

        while (true) {
            if (high < kHalf) {
                emit(0);
            } else if (low >= kHalf) {
                emit(1);
                low -= kHalf;
                high -= kHalf;
            } else if (low >= kQuarter && high < kThreeQuarters) {
                ++pending;
                low -= kQuarter;
                high -= kQuarter;
            } else {
                break;
            }
            low <<= 1;
            high = (high << 1) | 1u;
        }
    }

    ++pending;
    emit(low < kQuarter ? 0 : 1);
    writer.flush();
    return output;
}

inline std::vector<std::uint8_t> arithmeticDecode(
    const std::vector<std::uint8_t>& encoded) {
    std::uint32_t originalSize = 0;
    std::size_t position = 0;
    const auto model =
        detail::readArithmeticHeader(encoded, originalSize, position);
    if (originalSize == 0) return {};

    constexpr std::uint32_t kHalf = 0x80000000u;
    constexpr std::uint32_t kQuarter = 0x40000000u;
    constexpr std::uint32_t kThreeQuarters = 0xC0000000u;

    BitReader reader(encoded, position);
    std::uint32_t low = 0;
    std::uint32_t high = 0xFFFFFFFFu;
    std::uint32_t value = 0;
    for (int i = 0; i < 32; ++i) {
        const int bit = reader.readBit();
        value = (value << 1) | static_cast<std::uint32_t>(bit < 0 ? 0 : bit);
    }

    std::vector<std::uint8_t> output;
    output.reserve(originalSize);

    for (std::uint32_t i = 0; i < originalSize; ++i) {
        const std::uint64_t range =
            static_cast<std::uint64_t>(high) - low + 1;
        const std::uint64_t scaled =
            (static_cast<std::uint64_t>(value - low) + 1) * model.total - 1;
        std::uint64_t cumulativeValue = scaled / range;
        if (cumulativeValue >= model.total) cumulativeValue = model.total - 1;
        const int symbol = detail::findSymbol(model, cumulativeValue);

        output.push_back(static_cast<std::uint8_t>(symbol));
        high = low + static_cast<std::uint32_t>(
                         (range * model.cumulative[symbol + 1]) / model.total - 1);
        low = low + static_cast<std::uint32_t>(
                        (range * model.cumulative[symbol]) / model.total);

        while (true) {
            if (high < kHalf) {
                // Nothing to subtract.
            } else if (low >= kHalf) {
                value -= kHalf;
                low -= kHalf;
                high -= kHalf;
            } else if (low >= kQuarter && high < kThreeQuarters) {
                value -= kQuarter;
                low -= kQuarter;
                high -= kQuarter;
            } else {
                break;
            }
            low <<= 1;
            high = (high << 1) | 1u;
            const int bit = reader.readBit();
            value = (value << 1) | static_cast<std::uint32_t>(bit < 0 ? 0 : bit);
        }
    }
    return output;
}

// --- Textbook floating-point form -----------------------------------------
// Kept separate because it is the clearest way to see the algorithm. It is
// not used for large inputs: long double precision becomes the limit.

inline std::vector<std::uint8_t> arithmeticEncodeFloat(
    const std::vector<std::uint8_t>& input) {
    const auto model = detail::buildArithmeticModel(input);
    std::vector<std::uint8_t> output;
    detail::writeArithmeticHeader(output, model,
                                  static_cast<std::uint32_t>(input.size()));

    BitWriter writer(output);
    long double low = 0.0L;
    long double high = 1.0L;
    std::uint64_t pending = 0;

    auto emit = [&](int bit) {
        writer.writeBit(bit);
        while (pending > 0) {
            writer.writeBit(bit ^ 1);
            --pending;
        }
    };

    for (std::uint8_t symbol : input) {
        const long double range = high - low;
        high = low + range *
                         static_cast<long double>(model.cumulative[symbol + 1]) /
                         static_cast<long double>(model.total);
        low = low + range *
                        static_cast<long double>(model.cumulative[symbol]) /
                        static_cast<long double>(model.total);

        while (true) {
            if (high < 0.5L) {
                emit(0);
            } else if (low >= 0.5L) {
                emit(1);
                low -= 0.5L;
                high -= 0.5L;
            } else if (low >= 0.25L && high < 0.75L) {
                ++pending;
                low -= 0.25L;
                high -= 0.25L;
            } else {
                break;
            }
            low *= 2.0L;
            high *= 2.0L;
        }
    }

    ++pending;
    emit(low < 0.25L ? 0 : 1);
    writer.flush();
    return output;
}

inline std::vector<std::uint8_t> arithmeticDecodeFloat(
    const std::vector<std::uint8_t>& encoded) {
    std::uint32_t originalSize = 0;
    std::size_t position = 0;
    const auto model =
        detail::readArithmeticHeader(encoded, originalSize, position);
    if (originalSize == 0) return {};

    BitReader reader(encoded, position);
    long double low = 0.0L;
    long double high = 1.0L;
    long double value = 0.0L;
    long double weight = 0.5L;
    for (int i = 0; i < 32; ++i) {
        const int bit = reader.readBit();
        value += static_cast<long double>(bit < 0 ? 0 : bit) * weight;
        weight *= 0.5L;
    }

    std::vector<std::uint8_t> output;
    output.reserve(originalSize);

    for (std::uint32_t i = 0; i < originalSize; ++i) {
        const long double range = high - low;
        const long double scaled = (value - low) / range;
        std::uint64_t cumulativeValue = static_cast<std::uint64_t>(
            scaled * static_cast<long double>(model.total));
        if (cumulativeValue >= model.total) cumulativeValue = model.total - 1;
        const int symbol = detail::findSymbol(model, cumulativeValue);

        output.push_back(static_cast<std::uint8_t>(symbol));
        high = low + range *
                         static_cast<long double>(model.cumulative[symbol + 1]) /
                         static_cast<long double>(model.total);
        low = low + range *
                        static_cast<long double>(model.cumulative[symbol]) /
                        static_cast<long double>(model.total);

        while (true) {
            if (high < 0.5L) {
                // Nothing to subtract.
            } else if (low >= 0.5L) {
                value -= 0.5L;
                low -= 0.5L;
                high -= 0.5L;
            } else if (low >= 0.25L && high < 0.75L) {
                value -= 0.25L;
                low -= 0.25L;
                high -= 0.25L;
            } else {
                break;
            }
            low *= 2.0L;
            high *= 2.0L;
            const int bit = reader.readBit();
            value = value * 2.0L +
                    static_cast<long double>(bit < 0 ? 0 : bit) /
                        4294967296.0L;
        }
    }
    return output;
}

} // namespace dsa::compression

#endif

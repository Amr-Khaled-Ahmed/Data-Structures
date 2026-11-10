#ifndef DSA_COMPRESSION_JPEG_H
#define DSA_COMPRESSION_JPEG_H

#include "algorithms/compression/bit_io.h"
#include "algorithms/compression/dct.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <map>
#include <stdexcept>
#include <vector>

namespace dsa::compression {

// ---------------------------------------------------------------------------
// Baseline JPEG (grayscale, 8-bit, no subsampling)
//
// This is a real baseline JPEG encoder and decoder, not a toy: it writes the
// standard marker segments (SOI, APP0, DQT, SOF0, DHT, SOS, EOI), uses the
// luminance quantization table and the luminance Huffman tables from the
// JPEG specification (Annex K), performs the 8x8 DCT, quantizes, zigzags,
// differential-codes the DC coefficient, run-length codes the AC
// coefficients, and applies 0xFF byte stuffing.
//
// Colour JPEG (YCbCr + chroma subsampling) and progressive JPEG are not
// implemented here. The single-component stream is still a valid .jpg that
// any baseline decoder can open.
//
// Reference: ITU-T T.81 / ISO/IEC 10918-1.
// ---------------------------------------------------------------------------

struct JpegImage {
    int width = 0;
    int height = 0;
    std::vector<std::uint8_t> pixels; // row-major grayscale
};

namespace detail {

constexpr std::array<int, 64> kJpegZigzag = {
    0,  1,  8,  16, 9,  2,  3,  10, 17, 24, 32, 25, 18, 11, 4,  5,
    12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6,  7,  14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63};

constexpr std::array<int, 64> kJpegBaseQuant = {
    16, 11, 10, 16, 24,  40,  51,  61,  12, 12, 14, 19, 26, 58, 60, 55,
    14, 13, 16, 24, 40,  57,  69,  56,  14, 17, 22, 29, 51, 87, 80, 62,
    18, 22, 37, 56, 68,  109, 103, 77,  24, 35, 55, 64, 81, 104, 113, 92,
    49, 64, 78, 87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99};

inline const std::array<int, 17>& dcLuminanceBits() {
    static const std::array<int, 17> bits = {
        0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0};
    return bits;
}

inline const std::vector<int>& dcLuminanceValues() {
    static const std::vector<int> values = {0, 1, 2, 3, 4, 5,
                                            6, 7, 8, 9, 10, 11};
    return values;
}

inline const std::array<int, 17>& acLuminanceBits() {
    static const std::array<int, 17> bits = {
        0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7D};
    return bits;
}

inline const std::vector<int>& acLuminanceValues() {
    static const std::vector<int> values = {
        0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41,
        0x06, 0x13, 0x51, 0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91,
        0xA1, 0x08, 0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 0xD1, 0xF0, 0x24,
        0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16, 0x17, 0x18, 0x19, 0x1A,
        0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38,
        0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x53,
        0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66,
        0x67, 0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
        0x7A, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x92, 0x93,
        0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5,
        0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
        0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9,
        0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE1,
        0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF1, 0xF2,
        0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA};
    return values;
}

struct JpegHuffmanTable {
    std::array<std::uint32_t, 256> code{};
    std::array<int, 256> length{};
    std::array<std::map<std::uint32_t, int>, 17> lookup;
};

inline JpegHuffmanTable buildJpegHuffmanTable(
    const std::array<int, 17>& bits, const std::vector<int>& values) {
    JpegHuffmanTable table;
    std::uint32_t code = 0;
    std::size_t k = 0;
    for (int length = 1; length <= 16; ++length) {
        for (int i = 0; i < bits[length]; ++i) {
            if (k >= values.size()) {
                throw std::invalid_argument("JPEG: too few Huffman values");
            }
            const int symbol = values[k++];
            table.code[static_cast<std::size_t>(symbol)] = code;
            table.length[static_cast<std::size_t>(symbol)] = length;
            table.lookup[length][code] = symbol;
            ++code;
        }
        code <<= 1;
    }
    return table;
}

inline std::array<int, 64> scaledQuantizationTable(int quality) {
    quality = std::clamp(quality, 1, 100);
    const int scale = quality < 50 ? 5000 / quality : 200 - quality * 2;
    std::array<int, 64> table{};
    for (int i = 0; i < 64; ++i) {
        const int value = (kJpegBaseQuant[static_cast<std::size_t>(i)] * scale + 50) / 100;
        table[static_cast<std::size_t>(i)] = std::clamp(value, 1, 255);
    }
    return table;
}

inline int amplitudeCategory(int value) {
    if (value == 0) return 0;
    int magnitude = value < 0 ? -value : value;
    int category = 0;
    while (magnitude != 0) {
        magnitude >>= 1;
        ++category;
    }
    return category;
}

inline int amplitudeBits(int value, int category) {
    return value >= 0 ? value : value + (1 << category) - 1;
}

inline void appendMarker(std::vector<std::uint8_t>& output, std::uint8_t marker) {
    output.push_back(0xFF);
    output.push_back(marker);
}

inline void appendLength(std::vector<std::uint8_t>& output, std::uint16_t length) {
    output.push_back(static_cast<std::uint8_t>((length >> 8) & 0xFF));
    output.push_back(static_cast<std::uint8_t>(length & 0xFF));
}

inline std::uint16_t readLength(const std::vector<std::uint8_t>& input,
                                std::size_t& position) {
    if (position + 2 > input.size()) {
        throw std::invalid_argument("JPEG: truncated length field");
    }
    const std::uint16_t length = static_cast<std::uint16_t>(
        (input[position] << 8) | input[position + 1]);
    position += 2;
    return length;
}

} // namespace detail

inline std::vector<std::uint8_t> jpegEncode(const JpegImage& image,
                                            int quality = 90) {
    if (image.width <= 0 || image.height <= 0) {
        throw std::invalid_argument("jpegEncode: image must be non-empty");
    }
    if (image.pixels.size() !=
        static_cast<std::size_t>(image.width) * image.height) {
        throw std::invalid_argument("jpegEncode: pixel count mismatch");
    }

    const auto quantization = detail::scaledQuantizationTable(quality);
    const auto dcTable = detail::buildJpegHuffmanTable(
        detail::dcLuminanceBits(), detail::dcLuminanceValues());
    const auto acTable = detail::buildJpegHuffmanTable(
        detail::acLuminanceBits(), detail::acLuminanceValues());

    std::vector<std::uint8_t> output;

    // SOI
    detail::appendMarker(output, 0xD8);

    // APP0 / JFIF
    detail::appendMarker(output, 0xE0);
    detail::appendLength(output, 16);
    output.push_back('J');
    output.push_back('F');
    output.push_back('I');
    output.push_back('F');
    output.push_back(0);
    output.push_back(1);
    output.push_back(1);
    output.push_back(0);
    output.push_back(0);
    output.push_back(1);
    output.push_back(0);
    output.push_back(1);
    output.push_back(0);
    output.push_back(0);

    // DQT
    detail::appendMarker(output, 0xDB);
    detail::appendLength(output, 67);
    output.push_back(0x00);
    for (int i = 0; i < 64; ++i) {
        output.push_back(static_cast<std::uint8_t>(
            quantization[static_cast<std::size_t>(
                detail::kJpegZigzag[static_cast<std::size_t>(i)])]));
    }

    // SOF0
    detail::appendMarker(output, 0xC0);
    detail::appendLength(output, 11);
    output.push_back(8);
    output.push_back(static_cast<std::uint8_t>((image.height >> 8) & 0xFF));
    output.push_back(static_cast<std::uint8_t>(image.height & 0xFF));
    output.push_back(static_cast<std::uint8_t>((image.width >> 8) & 0xFF));
    output.push_back(static_cast<std::uint8_t>(image.width & 0xFF));
    output.push_back(1);
    output.push_back(1);
    output.push_back(0x11);
    output.push_back(0);

    // DHT (DC luminance)
    detail::appendMarker(output, 0xC4);
    detail::appendLength(output, 31);
    output.push_back(0x00);
    for (int length = 1; length <= 16; ++length) {
        output.push_back(static_cast<std::uint8_t>(
            detail::dcLuminanceBits()[static_cast<std::size_t>(length)]));
    }
    for (int value : detail::dcLuminanceValues()) {
        output.push_back(static_cast<std::uint8_t>(value));
    }

    // DHT (AC luminance)
    detail::appendMarker(output, 0xC4);
    detail::appendLength(output, 181);
    output.push_back(0x10);
    for (int length = 1; length <= 16; ++length) {
        output.push_back(static_cast<std::uint8_t>(
            detail::acLuminanceBits()[static_cast<std::size_t>(length)]));
    }
    for (int value : detail::acLuminanceValues()) {
        output.push_back(static_cast<std::uint8_t>(value));
    }

    // SOS
    detail::appendMarker(output, 0xDA);
    detail::appendLength(output, 8);
    output.push_back(1);
    output.push_back(1);
    output.push_back(0x00);
    output.push_back(0);
    output.push_back(63);
    output.push_back(0);

    // Entropy-coded data
    std::vector<std::uint8_t> entropy;
    BitWriter writer(entropy);

    const int blocksX = (image.width + 7) / 8;
    const int blocksY = (image.height + 7) / 8;
    int previousDc = 0;

    std::vector<double> block(64);
    std::vector<int> quantized(64);

    for (int blockY = 0; blockY < blocksY; ++blockY) {
        for (int blockX = 0; blockX < blocksX; ++blockX) {
            for (int y = 0; y < 8; ++y) {
                for (int x = 0; x < 8; ++x) {
                    const int sourceY = std::min(blockY * 8 + y, image.height - 1);
                    const int sourceX = std::min(blockX * 8 + x, image.width - 1);
                    const double sample = static_cast<double>(
                        image.pixels[static_cast<std::size_t>(sourceY) *
                                         image.width +
                                     sourceX]);
                    block[static_cast<std::size_t>(y) * 8 + x] = sample - 128.0;
                }
            }

            const auto coefficients = dct2d(block, 8);
            for (int i = 0; i < 64; ++i) {
                quantized[static_cast<std::size_t>(i)] = static_cast<int>(
                    std::llround(coefficients[static_cast<std::size_t>(i)] /
                                 quantization[static_cast<std::size_t>(i)]));
            }

            const int dc = quantized[0];
            const int difference = dc - previousDc;
            previousDc = dc;
            const int dcCategory = detail::amplitudeCategory(difference);
            writer.writeBits(dcTable.code[static_cast<std::size_t>(dcCategory)],
                             dcTable.length[static_cast<std::size_t>(dcCategory)]);
            if (dcCategory > 0) {
                writer.writeBits(
                    static_cast<std::uint32_t>(
                        detail::amplitudeBits(difference, dcCategory)),
                    dcCategory);
            }

            int zeroRun = 0;
            for (int k = 1; k < 64; ++k) {
                const int value = quantized[static_cast<std::size_t>(
                    detail::kJpegZigzag[static_cast<std::size_t>(k)])];
                if (value == 0) {
                    ++zeroRun;
                    continue;
                }
                while (zeroRun > 15) {
                    writer.writeBits(
                        acTable.code[0xF0], acTable.length[0xF0]);
                    zeroRun -= 16;
                }
                const int category = detail::amplitudeCategory(value);
                const int symbol = (zeroRun << 4) | category;
                writer.writeBits(acTable.code[static_cast<std::size_t>(symbol)],
                                 acTable.length[static_cast<std::size_t>(symbol)]);
                writer.writeBits(
                    static_cast<std::uint32_t>(
                        detail::amplitudeBits(value, category)),
                    category);
                zeroRun = 0;
            }
            if (zeroRun > 0) {
                writer.writeBits(acTable.code[0x00], acTable.length[0x00]);
            }
        }
    }
    writer.flush(1);

    for (std::uint8_t byte : entropy) {
        output.push_back(byte);
        if (byte == 0xFF) output.push_back(0x00);
    }

    detail::appendMarker(output, 0xD9); // EOI
    return output;
}

inline JpegImage jpegDecode(const std::vector<std::uint8_t>& jpeg) {
    if (jpeg.size() < 4 || jpeg[0] != 0xFF || jpeg[1] != 0xD8) {
        throw std::invalid_argument("jpegDecode: missing SOI marker");
    }

    std::array<int, 64> quantization{};
    detail::JpegHuffmanTable dcTable;
    detail::JpegHuffmanTable acTable;
    bool haveDcTable = false;
    bool haveAcTable = false;

    int width = 0;
    int height = 0;

    std::size_t position = 2;
    std::size_t entropyStart = 0;

    while (position + 1 < jpeg.size()) {
        if (jpeg[position] != 0xFF) {
            throw std::invalid_argument("jpegDecode: expected marker");
        }
        while (position < jpeg.size() && jpeg[position] == 0xFF) ++position;
        if (position >= jpeg.size()) break;
        const std::uint8_t marker = jpeg[position++];

        if (marker == 0xD9) break; // EOI
        if (marker == 0xD8 || marker == 0x01 ||
            (marker >= 0xD0 && marker <= 0xD7)) {
            continue; // standalone markers
        }

        const std::uint16_t length = detail::readLength(jpeg, position);
        const std::size_t segmentEnd =
            position + static_cast<std::size_t>(length) - 2;
        if (segmentEnd > jpeg.size()) {
            throw std::invalid_argument("jpegDecode: truncated segment");
        }

        if (marker == 0xDB) { // DQT
            const std::uint8_t info = jpeg[position++];
            const int precision = info >> 4;
            if (precision == 0) {
                for (int i = 0; i < 64; ++i) {
                    quantization[static_cast<std::size_t>(
                        detail::kJpegZigzag[static_cast<std::size_t>(i)])] =
                        jpeg[position++];
                }
            } else {
                for (int i = 0; i < 64; ++i) {
                    const int value = (jpeg[position] << 8) | jpeg[position + 1];
                    position += 2;
                    quantization[static_cast<std::size_t>(
                        detail::kJpegZigzag[static_cast<std::size_t>(i)])] = value;
                }
            }
        } else if (marker == 0xC4) { // DHT
            const std::uint8_t info = jpeg[position++];
            const int tableClass = info >> 4;
            std::array<int, 17> bits{};
            int total = 0;
            for (int length = 1; length <= 16; ++length) {
                bits[static_cast<std::size_t>(length)] = jpeg[position++];
                total += bits[static_cast<std::size_t>(length)];
            }
            std::vector<int> values(static_cast<std::size_t>(total));
            for (int i = 0; i < total; ++i) {
                values[static_cast<std::size_t>(i)] = jpeg[position++];
            }
            if (tableClass == 0) {
                dcTable = detail::buildJpegHuffmanTable(bits, values);
                haveDcTable = true;
            } else {
                acTable = detail::buildJpegHuffmanTable(bits, values);
                haveAcTable = true;
            }
        } else if (marker == 0xC0 || marker == 0xC1) { // SOF0 / SOF1
            const int precision = jpeg[position++];
            if (precision != 8) {
                throw std::invalid_argument(
                    "jpegDecode: only 8-bit precision is supported");
            }
            height = (jpeg[position] << 8) | jpeg[position + 1];
            position += 2;
            width = (jpeg[position] << 8) | jpeg[position + 1];
            position += 2;
            const int components = jpeg[position++];
            if (components != 1) {
                throw std::invalid_argument(
                    "jpegDecode: only single-component (grayscale) JPEG is "
                    "supported");
            }
            position += 3; // component id, sampling, quant table
        } else if (marker == 0xDA) { // SOS
            position = segmentEnd;
            entropyStart = position;
            break;
        } else {
            position = segmentEnd;
        }
    }

    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("jpegDecode: missing frame header");
    }
    if (!haveDcTable || !haveAcTable) {
        throw std::invalid_argument("jpegDecode: missing Huffman tables");
    }
    if (entropyStart == 0) {
        throw std::invalid_argument("jpegDecode: missing scan header");
    }

    // Extract the entropy-coded segment and undo 0xFF byte stuffing.
    std::vector<std::uint8_t> entropy;
    for (std::size_t i = entropyStart; i + 1 < jpeg.size(); ++i) {
        if (jpeg[i] == 0xFF) {
            const std::uint8_t next = jpeg[i + 1];
            if (next == 0x00) {
                entropy.push_back(0xFF);
                ++i;
            } else if (next == 0xD9) {
                break;
            } else {
                // Restart or another marker: stop the scan.
                break;
            }
        } else {
            entropy.push_back(jpeg[i]);
        }
    }

    BitReader reader(entropy);
    auto decodeSymbol = [](BitReader& bits,
                           const detail::JpegHuffmanTable& table) {
        std::uint32_t code = 0;
        for (int length = 1; length <= 16; ++length) {
            const int bit = bits.readBit();
            if (bit < 0) {
                throw std::runtime_error("jpegDecode: truncated entropy data");
            }
            code = (code << 1) | static_cast<std::uint32_t>(bit);
            const auto found = table.lookup[static_cast<std::size_t>(length)]
                                   .find(code);
            if (found != table.lookup[static_cast<std::size_t>(length)].end()) {
                return found->second;
            }
        }
        throw std::runtime_error("jpegDecode: invalid Huffman code");
    };

    auto receiveExtend = [](BitReader& bits, int category) {
        if (category == 0) return 0;
        const int value = bits.readBits(category);
        if (value < 0) {
            throw std::runtime_error("jpegDecode: truncated coefficient");
        }
        if (value < (1 << (category - 1))) {
            return value - (1 << category) + 1;
        }
        return value;
    };

    JpegImage image;
    image.width = width;
    image.height = height;
    image.pixels.assign(static_cast<std::size_t>(width) * height, 0);

    const int blocksX = (width + 7) / 8;
    const int blocksY = (height + 7) / 8;
    int previousDc = 0;

    std::vector<int> coefficients(64);
    std::vector<double> dequantized(64);

    for (int blockY = 0; blockY < blocksY; ++blockY) {
        for (int blockX = 0; blockX < blocksX; ++blockX) {
            std::fill(coefficients.begin(), coefficients.end(), 0);

            const int dcCategory = decodeSymbol(reader, dcTable);
            const int dcDifference = receiveExtend(reader, dcCategory);
            previousDc += dcDifference;
            coefficients[0] = previousDc;

            int k = 1;
            while (k < 64) {
                const int symbol = decodeSymbol(reader, acTable);
                if (symbol == 0x00) break; // EOB
                if (symbol == 0xF0) {
                    k += 16; // ZRL
                    continue;
                }
                const int zeroRun = symbol >> 4;
                const int category = symbol & 0x0F;
                k += zeroRun;
                if (k >= 64) break;
                const int value = receiveExtend(reader, category);
                coefficients[static_cast<std::size_t>(
                    detail::kJpegZigzag[static_cast<std::size_t>(k)])] = value;
                ++k;
            }

            for (int i = 0; i < 64; ++i) {
                dequantized[static_cast<std::size_t>(i)] =
                    static_cast<double>(coefficients[static_cast<std::size_t>(i)]) *
                    quantization[static_cast<std::size_t>(i)];
            }

            const auto block = idct2d(dequantized, 8);
            for (int y = 0; y < 8; ++y) {
                const int targetY = blockY * 8 + y;
                if (targetY >= height) continue;
                for (int x = 0; x < 8; ++x) {
                    const int targetX = blockX * 8 + x;
                    if (targetX >= width) continue;
                    const double sample =
                        block[static_cast<std::size_t>(y) * 8 + x] + 128.0;
                    const int value = static_cast<int>(std::llround(sample));
                    image.pixels[static_cast<std::size_t>(targetY) * width +
                                 targetX] =
                        static_cast<std::uint8_t>(std::clamp(value, 0, 255));
                }
            }
        }
    }

    return image;
}

} // namespace dsa::compression

#endif

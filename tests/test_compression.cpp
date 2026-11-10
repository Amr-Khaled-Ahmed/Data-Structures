/*
============================================================
 Tests for the data-compression module
============================================================

Every lossless codec must round-trip exactly. The lossy pieces
(quantization, JPEG, the MPEG-style codec) are checked with a
quality bound instead of equality.
*/

#include "algorithms/compression/adaptive_huffman.h"
#include "algorithms/compression/arithmetic_coding.h"
#include "algorithms/compression/dct.h"
#include "algorithms/compression/dpcm.h"
#include "algorithms/compression/huffman.h"
#include "algorithms/compression/jpeg.h"
#include "algorithms/compression/lz77.h"
#include "algorithms/compression/lz78.h"
#include "algorithms/compression/lzw.h"
#include "algorithms/compression/modified_huffman.h"
#include "algorithms/compression/motion_estimation.h"
#include "algorithms/compression/mpeg.h"
#include "algorithms/compression/quantization.h"
#include "algorithms/compression/rle.h"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

using namespace dsa::compression;
using namespace std;

namespace {

int failures = 0;

void require(bool condition, const string& message) {
    if (!condition) {
        cerr << "FAILED: " << message << '\n';
        ++failures;
    }
}

vector<uint8_t> toBytes(const string& text) {
    return {text.begin(), text.end()};
}

void checkLossless(const string& name, const vector<uint8_t>& input) {
    require(rleDecode(rleEncode(input)) == input, name + ": RLE");
    require(huffmanDecode(huffmanEncode(input)) == input, name + ": Huffman");
    require(adaptiveHuffmanDecode(adaptiveHuffmanEncode(input)) == input,
            name + ": adaptive Huffman");
    require(modifiedHuffmanDecode(modifiedHuffmanEncode(input)) == input,
            name + ": modified Huffman");
    require(lz77Decode(lz77Encode(input)) == input, name + ": LZ77");
    require(lz78Decode(lz78Encode(input)) == input, name + ": LZ78");
    require(lzwDecode(lzwEncode(input)) == input, name + ": LZW");
    require(arithmeticDecode(arithmeticEncode(input)) == input,
            name + ": arithmetic (binary)");
    require(arithmeticDecodeFloat(arithmeticEncodeFloat(input)) == input,
            name + ": arithmetic (float)");
}

JpegImage makeTestImage(int width, int height) {
    JpegImage image;
    image.width = width;
    image.height = height;
    image.pixels.resize(static_cast<size_t>(width) * height);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const int checker = ((x / 8) + (y / 8)) % 2 ? 200 : 40;
            const int gradient = (x * 255) / (width - 1);
            image.pixels[static_cast<size_t>(y) * width + x] =
                static_cast<uint8_t>((checker + gradient) / 2);
        }
    }
    return image;
}

double psnr(const JpegImage& a, const JpegImage& b) {
    double squaredError = 0.0;
    for (size_t i = 0; i < a.pixels.size(); ++i) {
        const double difference =
            static_cast<double>(a.pixels[i]) - b.pixels[i];
        squaredError += difference * difference;
    }
    const double mse = squaredError / static_cast<double>(a.pixels.size());
    if (mse == 0.0) return 99.0;
    return 10.0 * log10(255.0 * 255.0 / mse);
}

JpegImage makeMovingFrame(int width, int height, int shiftX, int shiftY) {
    JpegImage frame;
    frame.width = width;
    frame.height = height;
    frame.pixels.assign(static_cast<size_t>(width) * height, 30);
    for (int y = 12; y < 36; ++y) {
        for (int x = 12; x < 36; ++x) {
            const int px = x + shiftX;
            const int py = y + shiftY;
            if (px >= 0 && px < width && py >= 0 && py < height) {
                frame.pixels[static_cast<size_t>(py) * width + px] = 220;
            }
        }
    }
    return frame;
}

} // namespace

int main() {
    // --- Lossless round-trips ---------------------------------------------
    checkLossless("empty", {});
    checkLossless("single", toBytes("a"));
    checkLossless("runs", toBytes("aaaaabbbbbcccccdddddaaaaabbbbb"));
    checkLossless("text", toBytes("the quick brown fox jumps over the lazy dog"));
    checkLossless("repeats",
                  toBytes("abracadabra abracadabra abracadabra"));
    checkLossless("binary",
                  [] {
                      vector<uint8_t> data;
                      for (int i = 0; i < 512; ++i) {
                          data.push_back(
                              static_cast<uint8_t>((i * 37 + i / 5) & 0xFF));
                      }
                      return data;
                  }());

    {
        const auto input = toBytes("abracadabra");
        require(byteEntropy(input) > 0.0 && byteEntropy(input) < 8.0,
                "entropy should be between 0 and 8 bits/byte");
        require(byteEntropy(toBytes("aaaaaaaaaa")) == 0.0,
                "a constant stream has zero entropy");
    }

    // --- DPCM --------------------------------------------------------------
    {
        const vector<uint8_t> pixels = {10, 12, 14, 16, 18, 20, 22, 24};
        require(dpcmDecode(dpcmEncode(pixels)) == pixels,
                "DPCM should be lossless");
        const auto quantized = dpcmEncodeQuantized(pixels, 4);
        const auto restored = dpcmDecodeQuantized(quantized, 4);
        require(restored.size() == pixels.size(),
                "quantized DPCM should preserve length");
    }

    // --- Quantization ------------------------------------------------------
    {
        ScalarQuantizer quantizer(4.0);
        const vector<double> values = {0.3, 3.8, -7.9, 12.2};
        const auto indices = quantizer.encode(values);
        const auto restored = quantizer.decode(indices);
        require(meanSquaredError(values, restored) < 4.0,
                "scalar quantization error should stay below half a step");
    }
    {
        VectorQuantizer quantizer(2, 2);
        const vector<vector<double>> training = {
            {0, 0}, {0.2, 0.1}, {0.1, 0.3}, {9, 9}, {9.2, 8.9}, {8.8, 9.1}};
        quantizer.train(training);
        const auto codes = quantizer.encode(training);
        require(codes[0] == codes[1] && codes[1] == codes[2],
                "VQ should cluster the low vectors");
        require(codes[3] == codes[4] && codes[4] == codes[5],
                "VQ should cluster the high vectors");
        require(codes[0] != codes[3],
                "VQ should separate the two clusters");
    }

    // --- DCT ---------------------------------------------------------------
    {
        vector<double> block(64);
        for (int i = 0; i < 64; ++i) block[static_cast<size_t>(i)] = i;
        const auto spectrum = dct2d(block, 8);
        const auto restored = idct2d(spectrum, 8);
        double maxError = 0.0;
        for (size_t i = 0; i < block.size(); ++i) {
            maxError = max(maxError, abs(block[i] - restored[i]));
        }
        require(maxError < 1e-9, "DCT inverse should reconstruct the block");
    }

    // --- JPEG --------------------------------------------------------------
    {
        const JpegImage image = makeTestImage(64, 48);
        const auto encoded = jpegEncode(image, 90);
        require(encoded.size() > 2 && encoded[0] == 0xFF && encoded[1] == 0xD8,
                "JPEG should start with SOI");
        require(encoded[encoded.size() - 2] == 0xFF &&
                    encoded.back() == 0xD9,
                "JPEG should end with EOI");

        const JpegImage decoded = jpegDecode(encoded);
        require(decoded.width == image.width && decoded.height == image.height,
                "JPEG should preserve dimensions");
        require(psnr(image, decoded) > 30.0,
                "JPEG quality 90 should stay above 30 dB");
    }

    // --- Motion estimation / MPEG-style codec ------------------------------
    {
        const int width = 64;
        const int height = 64;
        const auto reference = makeMovingFrame(width, height, 0, 0).pixels;
        const auto current = makeMovingFrame(width, height, 3, 2).pixels;

        const auto motion = encodeMotionCompensated(reference, current, width,
                                                    height, 16, 8);
        const auto reconstructed = decodeMotionCompensated(reference, motion);
        double squaredError = 0.0;
        for (size_t i = 0; i < current.size(); ++i) {
            const double difference =
                static_cast<double>(current[i]) - reconstructed[i];
            squaredError += difference * difference;
        }
        require(squaredError / static_cast<double>(current.size()) < 10.0,
                "motion compensation should reduce the residual");
    }
    {
        MpegSequence sequence;
        sequence.width = 64;
        sequence.height = 64;
        for (int i = 0; i < 4; ++i) {
            sequence.frames.push_back(makeMovingFrame(64, 64, i * 3, i * 2));
        }

        const auto stream = mpegEncode(sequence, 16, 8, 85);
        const auto decoded = mpegDecode(stream);
        require(decoded.frames.size() == sequence.frames.size(),
                "MPEG-style codec should preserve the frame count");
        for (size_t i = 0; i < decoded.frames.size(); ++i) {
            require(psnr(sequence.frames[i], decoded.frames[i]) > 30.0,
                    "MPEG-style frame should stay above 30 dB");
        }
    }

    if (failures == 0) {
        cout << "All compression tests passed\n";
        return 0;
    }
    cerr << failures << " compression test(s) failed\n";
    return 1;
}

#include "algorithms/compression/adaptive_huffman.h"
#include "algorithms/compression/arithmetic_coding.h"
#include "algorithms/compression/dct.h"
#include "algorithms/compression/dpcm.h"
#include "algorithms/compression/huffman.h"
#include "algorithms/compression/lz77.h"
#include "algorithms/compression/lz78.h"
#include "algorithms/compression/lzw.h"
#include "algorithms/compression/modified_huffman.h"
#include "algorithms/compression/quantization.h"
#include "algorithms/compression/rle.h"

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace dsa::compression;
using namespace std;

namespace {

vector<uint8_t> toBytes(const string& text) {
    return {text.begin(), text.end()};
}

void report(const string& name, size_t original, size_t compressed) {
    const double ratio =
        original == 0 ? 0.0 : static_cast<double>(compressed) / original;
    cout << left << setw(22) << name << right << setw(6) << original
         << " -> " << setw(6) << compressed << " bytes   ratio " << fixed
         << setprecision(2) << ratio << '\n';
}

} // namespace

int main() {
    const string text =
        "the quick brown fox jumps over the lazy dog. "
        "the quick brown fox jumps over the lazy dog. "
        "pack my box with five dozen liquor jugs. "
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    const vector<uint8_t> input = toBytes(text);

    cout << "Input: " << input.size() << " bytes\n";
    cout << "Shannon entropy: " << fixed << setprecision(3)
         << byteEntropy(input) << " bits/byte\n\n";

    cout << "--- Lossless ---\n";
    report("RLE", input.size(), rleEncode(input).size());
    report("Huffman", input.size(), huffmanEncode(input).size());
    report("Adaptive Huffman", input.size(), adaptiveHuffmanEncode(input).size());
    report("Modified Huffman", input.size(), modifiedHuffmanEncode(input).size());
    report("LZ77", input.size(), lz77Encode(input).size());
    report("LZ78", input.size(), lz78Encode(input).size());
    report("LZW", input.size(), lzwEncode(input).size());
    report("Arithmetic (binary)", input.size(), arithmeticEncode(input).size());
    report("Arithmetic (float)", input.size(),
           arithmeticEncodeFloat(input).size());

    cout << "\n--- Lossy building blocks ---\n";

    vector<double> samples;
    for (uint8_t value : input) samples.push_back(static_cast<double>(value));
    ScalarQuantizer quantizer(8.0);
    const auto indices = quantizer.encode(samples);
    const auto dequantized = quantizer.decode(indices);
    cout << "Scalar quantization step 8 MSE: "
         << meanSquaredError(samples, dequantized) << '\n';

    const auto residuals = dpcmEncode(input);
    cout << "DPCM first residuals: ";
    for (size_t i = 0; i < 8 && i < residuals.size(); ++i) {
        cout << residuals[i] << ' ';
    }
    cout << '\n';

    vector<double> block(64);
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            block[static_cast<size_t>(r) * 8 + c] = (r * 8 + c) % 256;
        }
    }
    const auto spectrum = dct2d(block, 8);
    const auto restored = idct2d(spectrum, 8);
    double maxError = 0.0;
    for (size_t i = 0; i < block.size(); ++i) {
        maxError = max(maxError, abs(block[i] - restored[i]));
    }
    cout << "DCT -> inverse DCT max error: " << maxError << '\n';

    cout << "\nEvery lossless codec above round-trips exactly; see "
            "tests/test_compression.cpp.\n";
    return 0;
}

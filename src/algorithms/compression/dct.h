#ifndef DSA_COMPRESSION_DCT_H
#define DSA_COMPRESSION_DCT_H

#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <vector>

namespace dsa::compression {

// ---------------------------------------------------------------------------
// Discrete Cosine Transform (DCT-II, orthonormal)
//
// The DCT turns a block of samples into frequency coefficients: the first
// coefficient is the average (DC), the rest describe how much each cosine
// wave contributes (AC). Natural images put most of their energy in the low
// frequencies, so many high-frequency coefficients become small and can be
// quantized away. This is the core of JPEG and MPEG.
//
// Forward:  X[k] = c(k) * sum_n x[n] cos(pi (2n+1) k / (2N))
// Inverse:  x[n] = sum_k c(k) X[k] cos(pi (2n+1) k / (2N))
// with c(0) = sqrt(1/N) and c(k) = sqrt(2/N) for k > 0.
//
// The 2D transform is separable: apply the 1D transform to every row, then
// to every column. Time: O(N^3) with the direct formula (fine for 8x8
// blocks); production codecs use fast O(N log N) DCT algorithms.
// ---------------------------------------------------------------------------

inline std::vector<double> dct1d(const std::vector<double>& input) {
    const std::size_t n = input.size();
    if (n == 0) return {};

    std::vector<double> output(n, 0.0);
    const double pi = std::acos(-1.0);
    for (std::size_t k = 0; k < n; ++k) {
        double sum = 0.0;
        for (std::size_t x = 0; x < n; ++x) {
            sum += input[x] *
                   std::cos(pi * (2.0 * static_cast<double>(x) + 1.0) *
                            static_cast<double>(k) /
                            (2.0 * static_cast<double>(n)));
        }
        const double scale =
            k == 0 ? std::sqrt(1.0 / static_cast<double>(n))
                   : std::sqrt(2.0 / static_cast<double>(n));
        output[k] = scale * sum;
    }
    return output;
}

inline std::vector<double> idct1d(const std::vector<double>& input) {
    const std::size_t n = input.size();
    if (n == 0) return {};

    std::vector<double> output(n, 0.0);
    const double pi = std::acos(-1.0);
    for (std::size_t x = 0; x < n; ++x) {
        double sum = 0.0;
        for (std::size_t k = 0; k < n; ++k) {
            const double scale =
                k == 0 ? std::sqrt(1.0 / static_cast<double>(n))
                       : std::sqrt(2.0 / static_cast<double>(n));
            sum += scale * input[k] *
                   std::cos(pi * (2.0 * static_cast<double>(x) + 1.0) *
                            static_cast<double>(k) /
                            (2.0 * static_cast<double>(n)));
        }
        output[x] = sum;
    }
    return output;
}

// Row-major N x N block.
inline std::vector<double> dct2d(const std::vector<double>& block, int n) {
    if (n <= 0 || block.size() != static_cast<std::size_t>(n) * n) {
        throw std::invalid_argument("dct2d: block size does not match n");
    }

    std::vector<double> intermediate(block.size(), 0.0);
    std::vector<double> row(static_cast<std::size_t>(n));
    std::vector<double> transformed;
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            row[static_cast<std::size_t>(c)] =
                block[static_cast<std::size_t>(r) * n + c];
        }
        transformed = dct1d(row);
        for (int c = 0; c < n; ++c) {
            intermediate[static_cast<std::size_t>(r) * n + c] =
                transformed[static_cast<std::size_t>(c)];
        }
    }

    std::vector<double> output(block.size(), 0.0);
    std::vector<double> column(static_cast<std::size_t>(n));
    for (int c = 0; c < n; ++c) {
        for (int r = 0; r < n; ++r) {
            column[static_cast<std::size_t>(r)] =
                intermediate[static_cast<std::size_t>(r) * n + c];
        }
        transformed = dct1d(column);
        for (int r = 0; r < n; ++r) {
            output[static_cast<std::size_t>(r) * n + c] =
                transformed[static_cast<std::size_t>(r)];
        }
    }
    return output;
}

inline std::vector<double> idct2d(const std::vector<double>& block, int n) {
    if (n <= 0 || block.size() != static_cast<std::size_t>(n) * n) {
        throw std::invalid_argument("idct2d: block size does not match n");
    }

    std::vector<double> intermediate(block.size(), 0.0);
    std::vector<double> column(static_cast<std::size_t>(n));
    std::vector<double> transformed;
    for (int c = 0; c < n; ++c) {
        for (int r = 0; r < n; ++r) {
            column[static_cast<std::size_t>(r)] =
                block[static_cast<std::size_t>(r) * n + c];
        }
        transformed = idct1d(column);
        for (int r = 0; r < n; ++r) {
            intermediate[static_cast<std::size_t>(r) * n + c] =
                transformed[static_cast<std::size_t>(r)];
        }
    }

    std::vector<double> output(block.size(), 0.0);
    std::vector<double> row(static_cast<std::size_t>(n));
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            row[static_cast<std::size_t>(c)] =
                intermediate[static_cast<std::size_t>(r) * n + c];
        }
        transformed = idct1d(row);
        for (int c = 0; c < n; ++c) {
            output[static_cast<std::size_t>(r) * n + c] =
                transformed[static_cast<std::size_t>(c)];
        }
    }
    return output;
}

} // namespace dsa::compression

#endif

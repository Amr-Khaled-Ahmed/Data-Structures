#ifndef DSA_COMPRESSION_QUANTIZATION_H
#define DSA_COMPRESSION_QUANTIZATION_H

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

namespace dsa::compression {

// ---------------------------------------------------------------------------
// Quantization
//
// Quantization is the step that makes lossy compression lossy: a large set of
// values is mapped onto a smaller set of representatives.
//
//   Scalar quantization: one value -> nearest multiple of `step`.
//   Vector quantization: a whole vector -> nearest entry in a small codebook,
//                        trained with k-means.
//
// The error is measured by the mean squared error (MSE), which is what JPEG
// and most transform codecs minimise in their own way.
// ---------------------------------------------------------------------------

struct ScalarQuantizer {
    double step = 1.0;

    explicit ScalarQuantizer(double quantizationStep = 1.0)
        : step(quantizationStep) {
        if (step <= 0.0) {
            throw std::invalid_argument("ScalarQuantizer: step must be > 0");
        }
    }

    std::vector<int> encode(const std::vector<double>& input) const {
        std::vector<int> indices;
        indices.reserve(input.size());
        for (double value : input) {
            indices.push_back(
                static_cast<int>(std::llround(value / step)));
        }
        return indices;
    }

    std::vector<double> decode(const std::vector<int>& indices) const {
        std::vector<double> output;
        output.reserve(indices.size());
        for (int index : indices) {
            output.push_back(static_cast<double>(index) * step);
        }
        return output;
    }
};

inline double meanSquaredError(const std::vector<double>& a,
                               const std::vector<double>& b) {
    if (a.size() != b.size()) {
        throw std::invalid_argument("meanSquaredError: size mismatch");
    }
    if (a.empty()) return 0.0;
    double sum = 0.0;
    for (std::size_t i = 0; i < a.size(); ++i) {
        const double difference = a[i] - b[i];
        sum += difference * difference;
    }
    return sum / static_cast<double>(a.size());
}

class VectorQuantizer {
public:
    VectorQuantizer(int dimensions, int codebookSize, int iterations = 50)
        : dimensions_(dimensions),
          codebookSize_(codebookSize),
          iterations_(iterations) {
        if (dimensions_ <= 0 || codebookSize_ <= 0 || iterations_ < 0) {
            throw std::invalid_argument("VectorQuantizer: invalid parameters");
        }
    }

    void train(const std::vector<std::vector<double>>& vectors) {
        if (vectors.empty()) {
            throw std::invalid_argument("VectorQuantizer: empty training set");
        }
        for (const auto& vector : vectors) {
            if (static_cast<int>(vector.size()) != dimensions_) {
                throw std::invalid_argument(
                    "VectorQuantizer: training vector has wrong size");
            }
        }

        codebook_.clear();
        codebook_.reserve(static_cast<std::size_t>(codebookSize_));
        for (int i = 0; i < codebookSize_; ++i) {
            codebook_.push_back(vectors[static_cast<std::size_t>(i) %
                                        vectors.size()]);
        }

        std::vector<int> assignment(vectors.size(), 0);
        for (int iteration = 0; iteration < iterations_; ++iteration) {
            bool changed = false;
            for (std::size_t i = 0; i < vectors.size(); ++i) {
                const int nearest = nearestCodeword(vectors[i]);
                if (nearest != assignment[i]) {
                    assignment[i] = nearest;
                    changed = true;
                }
            }
            if (!changed && iteration > 0) break;

            std::vector<std::vector<double>> sums(
                static_cast<std::size_t>(codebookSize_),
                std::vector<double>(static_cast<std::size_t>(dimensions_), 0.0));
            std::vector<int> counts(static_cast<std::size_t>(codebookSize_), 0);
            for (std::size_t i = 0; i < vectors.size(); ++i) {
                const int code = assignment[i];
                ++counts[static_cast<std::size_t>(code)];
                for (int d = 0; d < dimensions_; ++d) {
                    sums[static_cast<std::size_t>(code)]
                        [static_cast<std::size_t>(d)] +=
                        vectors[i][static_cast<std::size_t>(d)];
                }
            }
            for (int code = 0; code < codebookSize_; ++code) {
                if (counts[static_cast<std::size_t>(code)] == 0) {
                    codebook_[static_cast<std::size_t>(code)] =
                        vectors[static_cast<std::size_t>(code) % vectors.size()];
                    continue;
                }
                for (int d = 0; d < dimensions_; ++d) {
                    codebook_[static_cast<std::size_t>(code)]
                             [static_cast<std::size_t>(d)] =
                        sums[static_cast<std::size_t>(code)]
                            [static_cast<std::size_t>(d)] /
                        static_cast<double>(counts[static_cast<std::size_t>(code)]);
                }
            }
        }
    }

    std::vector<int> encode(
        const std::vector<std::vector<double>>& vectors) const {
        std::vector<int> indices;
        indices.reserve(vectors.size());
        for (const auto& vector : vectors) {
            indices.push_back(nearestCodeword(vector));
        }
        return indices;
    }

    std::vector<std::vector<double>> decode(
        const std::vector<int>& indices) const {
        std::vector<std::vector<double>> output;
        output.reserve(indices.size());
        for (int index : indices) {
            if (index < 0 || index >= static_cast<int>(codebook_.size())) {
                throw std::out_of_range("VectorQuantizer: invalid index");
            }
            output.push_back(codebook_[static_cast<std::size_t>(index)]);
        }
        return output;
    }

    const std::vector<std::vector<double>>& codebook() const {
        return codebook_;
    }

private:
    int nearestCodeword(const std::vector<double>& vector) const {
        if (static_cast<int>(vector.size()) != dimensions_) {
            throw std::invalid_argument("VectorQuantizer: wrong vector size");
        }
        int best = 0;
        double bestDistance = std::numeric_limits<double>::infinity();
        for (std::size_t code = 0; code < codebook_.size(); ++code) {
            double distance = 0.0;
            for (int d = 0; d < dimensions_; ++d) {
                const double difference =
                    vector[static_cast<std::size_t>(d)] -
                    codebook_[code][static_cast<std::size_t>(d)];
                distance += difference * difference;
            }
            if (distance < bestDistance) {
                bestDistance = distance;
                best = static_cast<int>(code);
            }
        }
        return best;
    }

    int dimensions_;
    int codebookSize_;
    int iterations_;
    std::vector<std::vector<double>> codebook_;
};

} // namespace dsa::compression

#endif

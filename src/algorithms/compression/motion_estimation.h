#ifndef DSA_COMPRESSION_MOTION_ESTIMATION_H
#define DSA_COMPRESSION_MOTION_ESTIMATION_H

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <vector>

namespace dsa::compression {

// ---------------------------------------------------------------------------
// Block-matching motion estimation
//
// Video frames are mostly the same as the previous frame, just shifted. Motion
// estimation finds, for every block in the current frame, the best-matching
// block in the reference frame. The displacement is a motion vector; the
// difference that remains is the residual.
//
// This is full search (exhaustive) with the sum of absolute differences (SAD)
// as the cost. Real encoders use diamond/hex searches, hierarchical search, or
// rate-distortion costs to cut the work.
//
// Time: O(pixels * (2r+1)^2). Space: O(pixels).
// ---------------------------------------------------------------------------

struct MotionVector {
    int dx = 0;
    int dy = 0;
};

struct MotionCompensatedFrame {
    int width = 0;
    int height = 0;
    int blockSize = 16;
    int blocksX = 0;
    int blocksY = 0;
    std::vector<MotionVector> vectors;
    std::vector<std::int16_t> residual;
};

inline long long blockSad(const std::vector<std::uint8_t>& reference,
                          int referenceX,
                          int referenceY,
                          const std::vector<std::uint8_t>& current,
                          int currentX,
                          int currentY,
                          int blockSize,
                          int width,
                          int height) {
    long long sad = 0;
    for (int y = 0; y < blockSize; ++y) {
        const int referencePixelY =
            std::clamp(referenceY + y, 0, height - 1);
        const int currentPixelY = std::clamp(currentY + y, 0, height - 1);
        for (int x = 0; x < blockSize; ++x) {
            const int referencePixelX =
                std::clamp(referenceX + x, 0, width - 1);
            const int currentPixelX = std::clamp(currentX + x, 0, width - 1);
            const int difference =
                static_cast<int>(reference[static_cast<std::size_t>(
                                     referencePixelY) * width +
                                 referencePixelX]) -
                static_cast<int>(current[static_cast<std::size_t>(
                                     currentPixelY) * width +
                                 currentPixelX]);
            sad += std::abs(difference);
        }
    }
    return sad;
}

inline MotionVector findBestMotionVector(
    const std::vector<std::uint8_t>& reference,
    const std::vector<std::uint8_t>& current,
    int width,
    int height,
    int blockX,
    int blockY,
    int blockSize,
    int searchRadius) {
    if (reference.size() != current.size() ||
        reference.size() != static_cast<std::size_t>(width) * height) {
        throw std::invalid_argument("findBestMotionVector: frame size mismatch");
    }

    MotionVector best;
    long long bestCost = -1;
    const int originX = blockX * blockSize;
    const int originY = blockY * blockSize;

    for (int dy = -searchRadius; dy <= searchRadius; ++dy) {
        for (int dx = -searchRadius; dx <= searchRadius; ++dx) {
            const long long cost = blockSad(reference, originX + dx,
                                            originY + dy, current, originX,
                                            originY, blockSize, width, height);
            if (bestCost < 0 || cost < bestCost) {
                bestCost = cost;
                best = {dx, dy};
            }
        }
    }
    return best;
}

inline MotionCompensatedFrame encodeMotionCompensated(
    const std::vector<std::uint8_t>& reference,
    const std::vector<std::uint8_t>& current,
    int width,
    int height,
    int blockSize = 16,
    int searchRadius = 8) {
    if (blockSize <= 0 || searchRadius < 0) {
        throw std::invalid_argument("encodeMotionCompensated: bad parameters");
    }
    if (reference.size() != current.size() ||
        reference.size() != static_cast<std::size_t>(width) * height) {
        throw std::invalid_argument(
            "encodeMotionCompensated: frame size mismatch");
    }

    MotionCompensatedFrame frame;
    frame.width = width;
    frame.height = height;
    frame.blockSize = blockSize;
    frame.blocksX = (width + blockSize - 1) / blockSize;
    frame.blocksY = (height + blockSize - 1) / blockSize;
    frame.vectors.resize(static_cast<std::size_t>(frame.blocksX) *
                         frame.blocksY);
    frame.residual.assign(static_cast<std::size_t>(width) * height, 0);

    for (int blockY = 0; blockY < frame.blocksY; ++blockY) {
        for (int blockX = 0; blockX < frame.blocksX; ++blockX) {
            const MotionVector vector = findBestMotionVector(
                reference, current, width, height, blockX, blockY, blockSize,
                searchRadius);
            frame.vectors[static_cast<std::size_t>(blockY) * frame.blocksX +
                          blockX] = vector;

            const int originX = blockX * blockSize;
            const int originY = blockY * blockSize;
            for (int y = 0; y < blockSize; ++y) {
                const int targetY = originY + y;
                if (targetY >= height) break;
                for (int x = 0; x < blockSize; ++x) {
                    const int targetX = originX + x;
                    if (targetX >= width) break;
                    const int predictionX =
                        std::clamp(targetX + vector.dx, 0, width - 1);
                    const int predictionY =
                        std::clamp(targetY + vector.dy, 0, height - 1);
                    const int prediction = reference[static_cast<std::size_t>(
                        predictionY) * width + predictionX];
                    const int actual = current[static_cast<std::size_t>(
                        targetY) * width + targetX];
                    frame.residual[static_cast<std::size_t>(targetY) * width +
                                   targetX] =
                        static_cast<std::int16_t>(actual - prediction);
                }
            }
        }
    }
    return frame;
}

inline std::vector<std::uint8_t> decodeMotionCompensated(
    const std::vector<std::uint8_t>& reference,
    const MotionCompensatedFrame& frame) {
    if (reference.size() !=
        static_cast<std::size_t>(frame.width) * frame.height) {
        throw std::invalid_argument(
            "decodeMotionCompensated: reference size mismatch");
    }

    std::vector<std::uint8_t> output(reference.size(), 0);
    for (int blockY = 0; blockY < frame.blocksY; ++blockY) {
        for (int blockX = 0; blockX < frame.blocksX; ++blockX) {
            const MotionVector vector =
                frame.vectors[static_cast<std::size_t>(blockY) * frame.blocksX +
                              blockX];
            const int originX = blockX * frame.blockSize;
            const int originY = blockY * frame.blockSize;
            for (int y = 0; y < frame.blockSize; ++y) {
                const int targetY = originY + y;
                if (targetY >= frame.height) break;
                for (int x = 0; x < frame.blockSize; ++x) {
                    const int targetX = originX + x;
                    if (targetX >= frame.width) break;
                    const int predictionX =
                        std::clamp(targetX + vector.dx, 0, frame.width - 1);
                    const int predictionY =
                        std::clamp(targetY + vector.dy, 0, frame.height - 1);
                    const int prediction = reference[static_cast<std::size_t>(
                        predictionY) * frame.width + predictionX];
                    const int value =
                        prediction +
                        frame.residual[static_cast<std::size_t>(targetY) *
                                           frame.width +
                                       targetX];
                    output[static_cast<std::size_t>(targetY) * frame.width +
                           targetX] =
                        static_cast<std::uint8_t>(std::clamp(value, 0, 255));
                }
            }
        }
    }
    return output;
}

} // namespace dsa::compression

#endif

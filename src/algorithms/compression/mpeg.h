#ifndef DSA_COMPRESSION_MPEG_H
#define DSA_COMPRESSION_MPEG_H

#include "algorithms/compression/jpeg.h"
#include "algorithms/compression/motion_estimation.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace dsa::compression {

// ---------------------------------------------------------------------------
// MPEG-style I/P frame codec (teaching model)
//
// This follows the *structure* of an MPEG video encoder:
//
//   I-frame: intra-coded, independent of other frames (JPEG here).
//   P-frame: motion-compensated from the previous frame; only motion vectors
//            and the quantized residual are stored.
//
// The container is intentionally simple:
//
//   "DMPG" | version | width | height | blockSize | searchRadius |
//   frameCount | { type | payloadSize | payload }
//
// IMPORTANT: this is NOT a conformant MPEG-1/MPEG-2 bitstream. A real MPEG
// stream adds sequence/picture/slice/macroblock layers, its own VLC tables,
// B-frames, chroma formats, quantiser matrices, and much more. What is
// implemented here is the part that matters for learning: I/P frames, block
// matching, motion vectors, and residual coding.
//
// Time: dominated by motion search, O(frames * pixels * searchArea).
// Space: O(pixels) per frame.
// ---------------------------------------------------------------------------

struct MpegSequence {
    int width = 0;
    int height = 0;
    std::vector<JpegImage> frames;
};

inline std::vector<std::uint8_t> mpegEncode(const MpegSequence& sequence,
                                            int blockSize = 16,
                                            int searchRadius = 8,
                                            int quality = 80) {
    if (sequence.frames.empty()) {
        throw std::invalid_argument("mpegEncode: no frames");
    }
    if (sequence.width <= 0 || sequence.height <= 0) {
        throw std::invalid_argument("mpegEncode: invalid dimensions");
    }

    std::vector<std::uint8_t> output;
    auto appendByte = [&output](std::uint8_t value) { output.push_back(value); };
    auto appendUint16 = [&output](std::uint16_t value) {
        output.push_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
        output.push_back(static_cast<std::uint8_t>(value & 0xFF));
    };
    auto appendUint32 = [&output](std::uint32_t value) {
        output.push_back(static_cast<std::uint8_t>((value >> 24) & 0xFF));
        output.push_back(static_cast<std::uint8_t>((value >> 16) & 0xFF));
        output.push_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
        output.push_back(static_cast<std::uint8_t>(value & 0xFF));
    };

    output.push_back('D');
    output.push_back('M');
    output.push_back('P');
    output.push_back('G');
    appendByte(1);
    appendUint16(static_cast<std::uint16_t>(sequence.width));
    appendUint16(static_cast<std::uint16_t>(sequence.height));
    appendUint16(static_cast<std::uint16_t>(blockSize));
    appendUint16(static_cast<std::uint16_t>(searchRadius));
    appendUint32(static_cast<std::uint32_t>(sequence.frames.size()));

    std::vector<std::uint8_t> reconstructed;
    for (std::size_t frameIndex = 0; frameIndex < sequence.frames.size();
         ++frameIndex) {
        const JpegImage& frame = sequence.frames[frameIndex];
        if (frame.width != sequence.width || frame.height != sequence.height ||
            frame.pixels.size() !=
                static_cast<std::size_t>(sequence.width) * sequence.height) {
            throw std::invalid_argument("mpegEncode: frame size mismatch");
        }

        if (frameIndex == 0) {
            const std::vector<std::uint8_t> payload = jpegEncode(frame, quality);
            appendByte(1); // I-frame
            appendUint32(static_cast<std::uint32_t>(payload.size()));
            output.insert(output.end(), payload.begin(), payload.end());
            // Predict from the frame the decoder will actually reconstruct.
            reconstructed = jpegDecode(payload).pixels;
            continue;
        }

        const MotionCompensatedFrame motion = encodeMotionCompensated(
            reconstructed, frame.pixels, sequence.width, sequence.height,
            blockSize, searchRadius);

        // Quantize the residual to one byte: q = round(residual / 2) + 128.
        JpegImage residualImage;
        residualImage.width = sequence.width;
        residualImage.height = sequence.height;
        residualImage.pixels.resize(frame.pixels.size());
        for (std::size_t i = 0; i < frame.pixels.size(); ++i) {
            const int quantized = std::clamp(
                static_cast<int>(std::lround(motion.residual[i] / 2.0)), -127,
                127);
            residualImage.pixels[i] =
                static_cast<std::uint8_t>(quantized + 128);
        }

        std::vector<std::uint8_t> payload;
        const std::uint32_t vectorCount =
            static_cast<std::uint32_t>(motion.vectors.size());
        payload.push_back(static_cast<std::uint8_t>((vectorCount >> 24) & 0xFF));
        payload.push_back(static_cast<std::uint8_t>((vectorCount >> 16) & 0xFF));
        payload.push_back(static_cast<std::uint8_t>((vectorCount >> 8) & 0xFF));
        payload.push_back(static_cast<std::uint8_t>(vectorCount & 0xFF));
        for (const MotionVector& vector : motion.vectors) {
            const std::int16_t dx = static_cast<std::int16_t>(vector.dx);
            const std::int16_t dy = static_cast<std::int16_t>(vector.dy);
            payload.push_back(static_cast<std::uint8_t>((dx >> 8) & 0xFF));
            payload.push_back(static_cast<std::uint8_t>(dx & 0xFF));
            payload.push_back(static_cast<std::uint8_t>((dy >> 8) & 0xFF));
            payload.push_back(static_cast<std::uint8_t>(dy & 0xFF));
        }
        const std::vector<std::uint8_t> residualPayload =
            jpegEncode(residualImage, quality);
        payload.insert(payload.end(), residualPayload.begin(),
                       residualPayload.end());

        appendByte(2); // P-frame
        appendUint32(static_cast<std::uint32_t>(payload.size()));
        output.insert(output.end(), payload.begin(), payload.end());

        // Reconstruct exactly as the decoder will, so the next P-frame predicts
        // from the same reference the decoder has. In particular, use the
        // JPEG-decoded residual, not the pre-compression residual bytes.
        const JpegImage decodedResidual = jpegDecode(residualPayload);
        MotionCompensatedFrame reconstructedMotion = motion;
        for (std::size_t i = 0; i < reconstructedMotion.residual.size(); ++i) {
            reconstructedMotion.residual[i] = static_cast<std::int16_t>(
                (static_cast<int>(decodedResidual.pixels[i]) - 128) * 2);
        }
        reconstructed =
            decodeMotionCompensated(reconstructed, reconstructedMotion);
    }

    return output;
}

inline MpegSequence mpegDecode(const std::vector<std::uint8_t>& stream) {
    if (stream.size() < 21 || stream[0] != 'D' || stream[1] != 'M' ||
        stream[2] != 'P' || stream[3] != 'G') {
        throw std::invalid_argument("mpegDecode: bad container header");
    }

    std::size_t position = 4;
    const std::uint8_t version = stream[position++];
    if (version != 1) {
        throw std::invalid_argument("mpegDecode: unsupported version");
    }

    auto readUint16 = [&stream, &position]() {
        const std::uint16_t value = static_cast<std::uint16_t>(
            (stream[position] << 8) | stream[position + 1]);
        position += 2;
        return value;
    };
    auto readUint32 = [&stream, &position]() {
        const std::uint32_t value =
            (static_cast<std::uint32_t>(stream[position]) << 24) |
            (static_cast<std::uint32_t>(stream[position + 1]) << 16) |
            (static_cast<std::uint32_t>(stream[position + 2]) << 8) |
            static_cast<std::uint32_t>(stream[position + 3]);
        position += 4;
        return value;
    };

    MpegSequence sequence;
    sequence.width = readUint16();
    sequence.height = readUint16();
    const int blockSize = readUint16();
    readUint16(); // searchRadius is not needed to decode
    const std::uint32_t frameCount = readUint32();

    std::vector<std::uint8_t> reconstructed;
    for (std::uint32_t frameIndex = 0; frameIndex < frameCount; ++frameIndex) {
        if (position + 5 > stream.size()) {
            throw std::invalid_argument("mpegDecode: truncated frame header");
        }
        const std::uint8_t type = stream[position++];
        const std::uint32_t payloadSize = readUint32();
        if (position + payloadSize > stream.size()) {
            throw std::invalid_argument("mpegDecode: truncated payload");
        }

        if (type == 1) {
            const std::vector<std::uint8_t> payload(
                stream.begin() + static_cast<std::ptrdiff_t>(position),
                stream.begin() +
                    static_cast<std::ptrdiff_t>(position + payloadSize));
            JpegImage frame = jpegDecode(payload);
            reconstructed = frame.pixels;
            sequence.frames.push_back(frame);
        } else if (type == 2) {
            std::size_t cursor = position;
            auto readPayloadUint32 = [&stream, &cursor]() {
                const std::uint32_t value =
                    (static_cast<std::uint32_t>(stream[cursor]) << 24) |
                    (static_cast<std::uint32_t>(stream[cursor + 1]) << 16) |
                    (static_cast<std::uint32_t>(stream[cursor + 2]) << 8) |
                    static_cast<std::uint32_t>(stream[cursor + 3]);
                cursor += 4;
                return value;
            };
            const std::uint32_t vectorCount = readPayloadUint32();

            MotionCompensatedFrame motion;
            motion.width = sequence.width;
            motion.height = sequence.height;
            motion.blockSize = blockSize;
            motion.blocksX = (sequence.width + blockSize - 1) / blockSize;
            motion.blocksY = (sequence.height + blockSize - 1) / blockSize;
            motion.vectors.resize(vectorCount);
            for (std::uint32_t i = 0; i < vectorCount; ++i) {
                const std::int16_t dx = static_cast<std::int16_t>(
                    (stream[cursor] << 8) | stream[cursor + 1]);
                cursor += 2;
                const std::int16_t dy = static_cast<std::int16_t>(
                    (stream[cursor] << 8) | stream[cursor + 1]);
                cursor += 2;
                motion.vectors[i] = {dx, dy};
            }

            const std::vector<std::uint8_t> residualPayload(
                stream.begin() + static_cast<std::ptrdiff_t>(cursor),
                stream.begin() + static_cast<std::ptrdiff_t>(position +
                                                             payloadSize));
            const JpegImage residualImage = jpegDecode(residualPayload);
            motion.residual.resize(residualImage.pixels.size());
            for (std::size_t i = 0; i < residualImage.pixels.size(); ++i) {
                motion.residual[i] = static_cast<std::int16_t>(
                    (static_cast<int>(residualImage.pixels[i]) - 128) * 2);
            }

            const std::vector<std::uint8_t> framePixels =
                decodeMotionCompensated(reconstructed, motion);
            JpegImage frame;
            frame.width = sequence.width;
            frame.height = sequence.height;
            frame.pixels = framePixels;
            reconstructed = framePixels;
            sequence.frames.push_back(frame);
        } else {
            throw std::invalid_argument("mpegDecode: unknown frame type");
        }

        position += payloadSize;
    }

    return sequence;
}

} // namespace dsa::compression

#endif

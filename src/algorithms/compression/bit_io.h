#ifndef DSA_COMPRESSION_BIT_IO_H
#define DSA_COMPRESSION_BIT_IO_H

#include <cstddef>
#include <cstdint>
#include <vector>

namespace dsa::compression {

// ---------------------------------------------------------------------------
// Bit-level reader and writer
//
// Compression codecs almost never align on byte boundaries. These two small
// helpers let the rest of the module write and read codes bit by bit while the
// container stays a plain std::vector<std::uint8_t>.
//
// Bits are written most-significant-bit first, which is the order used by
// Huffman codes, LZW and JPEG.
// ---------------------------------------------------------------------------

class BitWriter {
public:
    explicit BitWriter(std::vector<std::uint8_t>& output) : output_(output) {}

    void writeBit(int bit) {
        buffer_ = static_cast<std::uint8_t>((buffer_ << 1) | (bit & 1));
        ++filled_;
        if (filled_ == 8) {
            output_.push_back(buffer_);
            buffer_ = 0;
            filled_ = 0;
        }
    }

    void writeBits(std::uint32_t value, int count) {
        for (int i = count - 1; i >= 0; --i) {
            writeBit(static_cast<int>((value >> i) & 1U));
        }
    }

    // Pad the final partial byte with `padBit` (0 by default, 1 for JPEG).
    void flush(int padBit = 0) {
        if (filled_ > 0) {
            while (filled_ < 8) {
                buffer_ = static_cast<std::uint8_t>((buffer_ << 1) | (padBit & 1));
                ++filled_;
            }
            output_.push_back(buffer_);
            buffer_ = 0;
            filled_ = 0;
        }
    }

private:
    std::vector<std::uint8_t>& output_;
    std::uint8_t buffer_ = 0;
    int filled_ = 0;
};

class BitReader {
public:
    explicit BitReader(const std::vector<std::uint8_t>& input,
                       std::size_t start = 0)
        : input_(input), position_(start) {}

    // Returns -1 when the input is exhausted.
    int readBit() {
        if (remaining_ == 0) {
            if (position_ >= input_.size()) return -1;
            buffer_ = input_[position_++];
            remaining_ = 8;
        }
        const int bit = (buffer_ >> (remaining_ - 1)) & 1;
        --remaining_;
        return bit;
    }

    // Returns -1 when the input is exhausted before `count` bits are read.
    int readBits(int count) {
        int value = 0;
        for (int i = 0; i < count; ++i) {
            const int bit = readBit();
            if (bit < 0) return -1;
            value = (value << 1) | bit;
        }
        return value;
    }

    std::size_t position() const { return position_; }
    bool exhausted() const { return remaining_ == 0 && position_ >= input_.size(); }

private:
    const std::vector<std::uint8_t>& input_;
    std::size_t position_ = 0;
    std::uint8_t buffer_ = 0;
    int remaining_ = 0;
};

} // namespace dsa::compression

#endif

#include "algorithms/compression/motion_estimation.h"
#include "algorithms/compression/mpeg.h"

#include <cmath>
#include <iostream>
#include <vector>

using namespace dsa::compression;
using namespace std;

namespace {

JpegImage makeFrame(int width, int height, int shiftX, int shiftY) {
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

double framePsnr(const JpegImage& a, const JpegImage& b) {
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

} // namespace

int main() {
    const int width = 64;
    const int height = 64;

    MpegSequence sequence;
    sequence.width = width;
    sequence.height = height;
    for (int i = 0; i < 4; ++i) {
        sequence.frames.push_back(makeFrame(width, height, i * 3, i * 2));
    }

    const auto stream = mpegEncode(sequence, 16, 8, 85);
    const auto decoded = mpegDecode(stream);

    cout << "frames: " << decoded.frames.size() << '\n';
    cout << "container: " << stream.size() << " bytes for "
         << sequence.frames.size() << " frames (I then P/P/P)\n";
    for (size_t i = 0; i < decoded.frames.size(); ++i) {
        cout << "frame " << i << " PSNR " << framePsnr(sequence.frames[i],
                                                       decoded.frames[i])
             << " dB\n";
    }

    cout << "\nThis demonstrates I/P frames, block matching, motion vectors, "
            "and residual coding.\n"
            "It is a teaching model, not a conformant MPEG-1 bitstream.\n";
    return 0;
}

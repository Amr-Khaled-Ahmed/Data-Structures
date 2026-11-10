#include "algorithms/compression/jpeg.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace dsa::compression;
using namespace std;

namespace {

JpegImage makeImage(int width, int height) {
    JpegImage image;
    image.width = width;
    image.height = height;
    image.pixels.resize(static_cast<size_t>(width) * height);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const int checker = ((x / 8) + (y / 8)) % 2 ? 210 : 35;
            const int gradient = (x * 255) / (width - 1);
            const int circle =
                (x - width / 2) * (x - width / 2) +
                        (y - height / 2) * (y - height / 2) <
                    400
                ? 90
                : 0;
            image.pixels[static_cast<size_t>(y) * width + x] =
                static_cast<uint8_t>(
                    min(255, max(0, (checker + gradient) / 2 + circle)));
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

} // namespace

int main(int argc, char** argv) {
    const JpegImage image = makeImage(128, 96);

    for (int quality : {50, 75, 90}) {
        const vector<uint8_t> encoded = jpegEncode(image, quality);
        const JpegImage decoded = jpegDecode(encoded);
        cout << "quality " << quality << ": " << encoded.size()
             << " bytes, PSNR " << psnr(image, decoded) << " dB\n";

        if (argc > 1 && quality == 90) {
            const string path = argv[1];
            ofstream file(path, ios::binary);
            file.write(reinterpret_cast<const char*>(encoded.data()),
                       static_cast<streamsize>(encoded.size()));
            cout << "wrote " << path << '\n';
        }
    }

    cout << "The stream uses standard JPEG markers and luminance tables, so any "
            "baseline decoder can open it.\n";
    return 0;
}

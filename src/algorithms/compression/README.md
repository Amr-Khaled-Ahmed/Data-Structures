# Compression

This folder is the information-theory and data-compression wing of the library.
The theme is the same as the rest of the repository: every codec is written from
scratch so you can see *why* it compresses, not just call a library.

All headers live in `namespace dsa::compression` and are header-only, so a single
include is enough:

```cpp
#include "algorithms/compression/huffman.h"
#include "algorithms/compression/jpeg.h"
```

## Lossless compression

| File | Algorithm | Idea | Time |
| --- | --- | --- | --- |
| [`rle.h`](rle.h) | Run-Length Encoding | Replace runs with `(count, value)` | O(n) |
| [`huffman.h`](huffman.h) | Huffman (canonical, static) | Frequent symbols get short codes | O(n log k) |
| [`adaptive_huffman.h`](adaptive_huffman.h) | Adaptive Huffman (FGK) | Build the tree while decoding, no table sent | O(n · alphabet) |
| [`modified_huffman.h`](modified_huffman.h) | Modified Huffman (teaching variant) | Huffman-code run lengths, fax-style | O(n log k) |
| [`lz77.h`](lz77.h) | LZ77 | Sliding-window back-references | O(n · window · match) |
| [`lz78.h`](lz78.h) | LZ78 | Explicit growing phrase dictionary | O(n · phrase) |
| [`lzw.h`](lzw.h) | LZW | LZ78 dictionary with 12-bit codes and a clear code | O(n) expected |
| [`arithmetic_coding.h`](arithmetic_coding.h) | Arithmetic coding | One number for the whole message; binary and floating-point forms | O(n · alphabet) |

`huffman.h` also exposes `byteEntropy()`, which computes the Shannon entropy of
a byte stream so you can compare a coder against the theoretical limit.

## Lossy compression and quantization

| File | Algorithm | Idea |
| --- | --- | --- |
| [`quantization.h`](quantization.h) | Scalar + vector quantization | Map values/vectors to a small set of representatives; vector quantization trains a codebook with k-means |
| [`dpcm.h`](dpcm.h) | DPCM / predictive coding | Store differences from the previous sample |
| [`dct.h`](dct.h) | DCT-II / inverse DCT | Move energy into low-frequency coefficients |

## Image and video

| File | Algorithm | Status |
| --- | --- | --- |
| [`jpeg.h`](jpeg.h) | Baseline JPEG, grayscale | Real encoder **and** decoder: standard markers, Annex K luminance quant/Huffman tables, 8×8 DCT, zigzag, DC prediction, AC run-length, 0xFF stuffing. Produces a `.jpg` that external decoders open. |
| [`motion_estimation.h`](motion_estimation.h) | Block matching | Full-search motion estimation with SAD, motion vectors, motion-compensated residual |
| [`mpeg.h`](mpeg.h) | MPEG-style I/P frames | I-frame = intra (JPEG), P-frame = motion vectors + quantized residual. This is a teaching model, **not** a conformant MPEG-1/2 bitstream. |

### What "not conformant MPEG" means

The MPEG standards define much more than I/P frames: sequence, GOP, picture,
slice, and macroblock layers; their own VLC tables; B-frames; chroma formats;
quantiser matrices; rate control; and the exact bit syntax of every field. This
repository implements the parts that teach the ideas — motion estimation,
motion compensation, and residual coding — inside a simple container. Writing a
bit-exact MPEG-1 encoder/decoder is a separate, much larger project and is
tracked in [`docs/ROADMAP.md`](../../../docs/ROADMAP.md).

## Round-trip guarantees

- RLE, Huffman, adaptive Huffman, modified Huffman, LZ77, LZ78, LZW, and
  arithmetic coding are **lossless**: `decode(encode(x)) == x`.
- DPCM is lossless; `dpcmEncodeQuantized` is intentionally lossy.
- JPEG is lossy, but `jpegDecode(jpegEncode(image))` reproduces the image within
  a quality-dependent PSNR.
- The MPEG-style codec is lossy.

## Tests

```bash
cmake --build build --parallel
ctest --test-dir build --output-on-failure -R compression
```

`tests/test_compression.cpp` checks every lossless round-trip, the DCT inverse,
JPEG decode quality, and the motion-compensated reconstruction.

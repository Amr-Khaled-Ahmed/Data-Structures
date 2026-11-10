# Roadmap

This page lists what the repository already teaches and which topics would make
it a more complete learning resource. Pick an unchecked item, open an issue (or
just start), and follow [CONTRIBUTING.md](../CONTRIBUTING.md).

## Implemented

- [x] Array-based, singly, doubly, circular, ordered, and skip lists
- [x] Stack (with balanced-parentheses, postfix, and infix helpers)
- [x] Linked, array, circular-array, priority, and heap priority queues
- [x] Binary heap, max-heap, and max-priority queue
- [x] Ten hash functions / collision strategies
- [x] BST, AVL, red-black, and 2-3 trees
- [x] Interval tree, red-black interval tree, and segment tree
- [x] KD-tree, 2D range tree, and d-dimensional range tree
- [x] Disjoint set (array and tree representations)
- [x] Bloom filter
- [x] Suffix array
- [x] Trie, Fenwick tree, and sparse table
- [x] Deque and LRU cache
- [x] Hash map with separate chaining
- [x] Graph adjacency list
- [x] Ten sorting algorithms plus randomized selection
- [x] Linear and binary search (iterative and recursive)
- [x] Recursion examples (factorial, Tower of Hanoi)
- [x] Fibonacci (recursion, iterative DP, matrix power)
- [x] 0/1 knapsack DP
- [x] BFS, DFS, topological sort, and cycle detection
- [x] Connected components and Tarjan SCC
- [x] Dijkstra, Bellman-Ford, and Floyd-Warshall
- [x] Kruskal and Prim minimum spanning trees
- [x] Bipartite check, bridges, articulation points, and LCA
- [x] Trial division, sieve, Fermat, Miller-Rabin, AKS placeholder
- [x] Five NP-complete examples

### Data compression

- [x] RLE, LZ77, LZ78, and LZW
- [x] Static canonical Huffman, adaptive Huffman (FGK), modified Huffman
- [x] Arithmetic coding (binary and floating-point forms)
- [x] Scalar and vector quantization, DPCM
- [x] DCT-II and inverse DCT
- [x] Baseline grayscale JPEG encoder and decoder
- [x] Block-matching motion estimation and motion-compensated residuals
- [x] MPEG-style I/P frame container (teaching model)

## Good next contributions

The full phased checklist lives in [COVERAGE.md](COVERAGE.md). Start with the
next unchecked item in **Phase 1**.

### Data structures

- [ ] B-tree (the 2-3 tree is a good starting point)
- [ ] Treap / splay tree
- [ ] Binomial / Fibonacci heap
- [ ] Union-find with path compression
- [ ] Bitset and matrix

### Algorithms

- [ ] KMP, Z-algorithm, and Rabin-Karp
- [ ] Longest increasing subsequence, LCS, and edit distance
- [ ] Coin change and matrix chain multiplication
- [ ] N-queens, Sudoku, permutations
- [ ] Activity selection, fractional knapsack, Huffman coding
- [ ] GCD, extended Euclid, modular exponentiation, modular inverse
- [ ] A real AKS implementation (the current file is a teaching placeholder)
- [ ] Deterministic Miller-Rabin bases for 64-bit integers

### Compression

- [ ] A conformant MPEG-1/2 bitstream (sequence, slice, and macroblock layers,
      VLC tables, B-frames, chroma formats)
- [ ] Colour JPEG (YCbCr conversion and 4:2:0 chroma subsampling)
- [ ] A decoder that accepts arbitrary baseline JPEG files, not only the
      single-component streams produced here
- [ ] Progressive JPEG
- [ ] Rate-distortion motion search and sub-pixel motion vectors
- [ ] A DEFLATE-compatible stream (LZ77 + Huffman) that gzip/zlib can read

### Teaching improvements

- [ ] A diagram beside every tree rotation
- [ ] Step-by-step traces printed by every algorithm (some already do this)
- [ ] A test for every data structure operation
- [ ] More exercises at the end of each folder README
- [ ] Short videos or GIFs for the sorting algorithms

## Known limitations

These are intentional or documented so learners are not surprised:

- The AKS file is a **placeholder** that currently uses trial division. The real
  algorithm is much longer and is not implemented here.
- Fermat and Miller-Rabin are **probabilistic**; they can be fooled by
  Carmichael numbers and strong pseudoprimes.
- The disjoint-set implementations use union by size but **not path
  compression**, so `find` is O(log n), not near O(1).
- Some examples use the GNU header `<bits/stdc++.h>`. A compatibility header is
  provided for MSVC in `src/compat`.
- The NP-complete examples are small, readable search programs. They are not
  optimised for large inputs.
- The MPEG-style codec is a **teaching model**, not a conformant MPEG-1/2
  bitstream. It implements I/P frames, block matching, motion vectors, and
  residual coding inside a simple container.
- The modified Huffman implementation is a teaching variant, not bit-compatible
  with T.4 fax machines.
- The JPEG implementation handles 8-bit grayscale baseline images only. It
  writes and reads a valid `.jpg`, but colour and progressive modes are not
  implemented.

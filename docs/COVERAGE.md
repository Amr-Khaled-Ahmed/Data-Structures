# Coverage tracker

This page tracks what exists in the repository and what is still planned. It is
the checklist for the "write everything" goal: the work is split into phases so
each batch can be implemented, built, tested, and documented before the next one
starts.

Legend: `[x]` implemented, `[ ]` planned.

## Phase 1 — Core curriculum

### Graph traversal and connectivity

- [x] Graph representation (adjacency list)
- [x] BFS (distances, path, components)
- [x] DFS (recursive, iterative, discovery/finish times)
- [x] Topological sort (Kahn and DFS)
- [x] Cycle detection (directed and undirected)
- [x] Connected components
- [x] Strongly connected components (Tarjan)
- [x] Bipartite check (2-colouring)
- [x] Bridges and articulation points
- [x] Lowest common ancestor (binary lifting)

### Shortest paths and MST

- [x] Dijkstra
- [x] Bellman-Ford
- [x] Floyd-Warshall
- [x] Kruskal
- [x] Prim
- [x] Johnson's algorithm
- [x] SPFA
- [x] A* search
- [x] Shortest paths in a DAG

### Core data structures

- [x] Trie (prefix tree)
- [x] Fenwick tree (BIT)
- [x] Sparse table
- [x] Deque (growable circular buffer)
- [x] LRU cache
- [x] Hash map (separate chaining)
- [x] Graph adjacency list
- [x] Matrix (from-scratch 2D container + operations)
- [x] Bitset
- [x] Dynamic array (from scratch)
- [x] Min-heap as a separate header
- [x] Union-find with path compression
- [x] Ordered map / ordered set wrappers

### Dynamic programming

- [x] Fibonacci (recursion, matrix power, iterative)
- [x] 0/1 knapsack
- [x] Longest increasing subsequence (O(n log n))
- [x] Longest common subsequence
- [x] Edit distance (Levenshtein)
- [x] Coin change (minimum coins and counting ways)
- [x] Matrix chain multiplication
- [x] Rod cutting
- [x] Longest palindromic subsequence
- [x] Unbounded knapsack
- [x] Partition / subset sum DP
- [x] Bitmask TSP
- [x] Interval DP
- [x] Tree DP
- [x] Digit DP

### Strings

- [x] Suffix array with doubling
- [x] KMP (prefix function)
- [x] Z-algorithm
- [x] Rabin-Karp
- [x] Manacher (palindromes)
- [x] Aho-Corasick
- [x] Boyer-Moore
- [x] LCP array / Kasai
- [x] Longest common prefix of two strings

### Number theory and math

- [x] Trial division
- [x] Sieve of Eratosthenes
- [x] Fermat test
- [x] Miller-Rabin
- [x] AKS placeholder
- [x] GCD / LCM (Euclid)
- [x] Extended Euclid
- [x] Modular exponentiation
- [x] Modular inverse
- [x] Chinese remainder theorem
- [x] Euler's totient (sieve and factorisation)
- [x] Prime factorisation
- [x] Pollard's rho
- [x] Catalan numbers
- [x] nCr / Pascal's triangle
- [x] Matrix exponentiation (general)
- [x] Gaussian elimination
- [x] FFT / NTT

### Backtracking and greedy

- [x] Subset sum
- [x] 0/1 knapsack search
- [x] Graph colouring
- [x] Hamiltonian cycle
- [x] Travelling salesman
- [x] N-Queens
- [x] Sudoku solver
- [x] Permutations and combinations
- [x] Rat in a maze / word search
- [x] Palindrome partitioning
- [x] Activity selection
- [x] Fractional knapsack
- [x] Huffman coding
- [x] Interval scheduling / merge intervals
- [x] Jump game / gas station

### Misc core

- [x] Two pointers
- [x] Sliding window
- [x] Prefix sums / difference arrays
- [x] Binary search on the answer
- [x] Bit manipulation utilities
- [x] Big integer
- [x] Randomized shuffle / reservoir sampling
- [x] Game theory (Nim, Sprague-Grundy)

## Phase 2 — Advanced data structures

- [x] Treap
- [x] Splay tree
- [x] Randomized BST
- [x] Order-statistic tree
- [x] B-tree
- [x] B+ tree
- [x] Binomial heap
- [x] Fibonacci heap
- [x] Pairing heap
- [x] Min-max heap
- [x] Radix tree / compressed trie
- [x] Suffix tree
- [x] Suffix automaton
- [x] Rope (persistent string)
- [x] Van Emde Boas tree
- [x] Segment tree with lazy propagation
- [x] Persistent segment tree
- [x] Merge-sort tree
- [x] Wavelet tree
- [x] Cuckoo filter
- [x] Count-min sketch
- [x] HyperLogLog
- [x] Disjoint sparse table
- [x] Sqrt decomposition
- [x] Mo's algorithm

## Phase 3 — Advanced algorithms

### Graph

- [ ] Ford-Fulkerson
- [ ] Edmonds-Karp
- [ ] Dinic's max flow
- [ ] Min cut
- [ ] Bipartite matching (Hopcroft-Karp)
- [ ] Min-cost max-flow
- [ ] Eulerian path / circuit
- [ ] Heavy-light decomposition
- [ ] Euler tour + RMQ LCA
- [ ] Tarjan offline LCA
- [ ] Transitive closure
- [ ] 2-SAT
- [ ] Minimum bottleneck path

### Geometry

- [ ] Convex hull (Graham scan / Andrew monotone chain)
- [ ] Closest pair of points
- [ ] Line / segment intersection
- [ ] Point in polygon
- [ ] Polygon area
- [ ] Circle geometry
- [ ] Rotating calipers

### Advanced strings and DP

- [ ] Burrows-Wheeler transform
- [ ] Suffix automaton applications
- [ ] Regex / wildcard matching DP
- [ ] Egg dropping
- [ ] Stock trading DP family
- [ ] Tree knapsack
- [ ] SOS DP (sum over subsets)
- [ ] Divide-and-conquer DP optimisation

### Advanced math

- [ ] Discrete logarithm
- [ ] Linear sieve for multiplicative functions
- [ ] Möbius inversion
- [ ] Lucas theorem
- [ ] Berlekamp-Massey
- [ ] Computational geometry primitives

## Phase 4 — Data compression

### Lossless

- [x] Run-Length Encoding (RLE)
- [x] Huffman coding (canonical, static table)
- [x] Adaptive Huffman coding (FGK)
- [x] Modified Huffman (teaching variant)
- [x] LZ77
- [x] LZ78
- [x] LZW
- [x] Arithmetic coding (32-bit binary intervals)
- [x] Arithmetic coding (floating-point teaching form)

### Lossy and quantization

- [x] Scalar quantization
- [x] Vector quantization (k-means codebook)
- [x] DPCM / predictive coding
- [x] Quantized DPCM

### Transform, image and video

- [x] DCT-II and inverse DCT (1D and 2D)
- [x] Baseline grayscale JPEG encoder (standard markers, Annex K tables)
- [x] Baseline grayscale JPEG decoder
- [x] Block-matching motion estimation (full search, SAD)
- [x] Motion-compensated residual coding
- [x] MPEG-style I/P frame container (teaching model)
- [ ] Conformant MPEG-1/2 bitstream
- [ ] Colour JPEG (YCbCr, chroma subsampling)
- [ ] Progressive JPEG
- [ ] B-frames and rate-distortion motion search
- [ ] DEFLATE-compatible stream (LZ77 + Huffman)

## Phase 5 — Polish

- [ ] A test for every header-only structure
- [ ] A smoke test for every demo
- [ ] CI matrix: Linux GCC, Linux Clang, Windows MSVC
- [ ] Complexity table row for every new file
- [ ] Exercise section for every folder README
- [ ] Coverage report in CI

## Already present (for reference)

The repository started with these implementations; they are all still included:

- Lists: array-based, singly, doubly, circular, ordered, skip list.
- Stacks and queue variants, including priority queues and heap priority queue.
- Heaps: max-heap, heap sort, priority queue.
- Hashing: five hash functions, chaining (three variants), linear/quadratic/
  double probing.
- Trees: BST, AVL, red-black, 2-3, interval, augmented interval, red-black
  interval, segment, KD, 2D range, d-dimensional range.
- Disjoint set (array and tree), Bloom filter, suffix array.
- Sorting: bubble, selection, insertion, shell, merge, quick, heap, counting,
  radix, bucket, randomized select.
- Searching: linear and binary, iterative and recursive.
- Recursion examples, Fibonacci three ways, 0/1 knapsack.
- Primality: trial division, sieve, Fermat, Miller-Rabin, AKS placeholder.
- NP-complete search examples: subset sum, knapsack, colouring, Hamiltonian
  cycle, TSP.

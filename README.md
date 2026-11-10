# Data Structures & Algorithms Library

> Built from scratch: a learning-first C++20 library of data structures and
> algorithms. Every file explains **how the structure or algorithm is built**,
> not just how to call a library.

[![build-and-test](https://github.com/Amr-Khaled-Ahmed/data-structures-and-algorithms-library/actions/workflows/build.yml/badge.svg)](https://github.com/Amr-Khaled-Ahmed/data-structures-and-algorithms-library/actions/workflows/build.yml)
![C++20](https://img.shields.io/badge/C%2B%2B-20-blue)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

This is a learning-first library, not a production-grade one. The implementations
are written to be read: clear names, small helpers, complexity notes, and
comments that explain the *why*. Many programs also print a trace so you can
watch the algorithm work.

---

## Table of contents

- [Why this repository](#why-this-repository)
- [Quick start](#quick-start)
- [What is inside](#what-is-inside)
- [Repository layout](#repository-layout)
- [Learning path](#learning-path)
- [How to study a file](#how-to-study-a-file)
- [Testing and CI](#testing-and-ci)
- [Contributing](#contributing)
- [License](#license)
- [Credits](#credits)

---

## Why this repository

- **Built from scratch.** Linked lists use raw pointers, heaps use arrays, hash
  tables use hand-written hash functions, and trees implement their own
  rotations.
- **Teaching comments first.** Each file opens with the idea and the
  complexity. Non-obvious lines are explained in place.
- **Runnable examples.** Most topics have a `main()` you can build and run with
  CMake; the header-only algorithms are exercised by the guided tour and the
  test suite.
- **Consistent structure.** One concept per file, `snake_case` names, grouped by
  data structures vs algorithms.
- **Tests for the core.** Sorting and searching are covered by assertions, and
  every header is compiled independently in CI.

## Quick start

Requirements: a C++20 compiler (GCC 11+, Clang 14+, or MSVC 2022) and CMake
3.16+.

```bash
git clone https://github.com/Amr-Khaled-Ahmed/data-structures-and-algorithms-library.git
cd data-structures-and-algorithms-library

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure

./build/bin/dsa_demo
```

All example programs are written to `build/bin/`. See
[docs/BUILDING.md](docs/BUILDING.md) for Windows, macOS, single-file builds, and
troubleshooting.

## What is inside

### Data structures

| Topic | Files | What you learn | Key complexity |
| --- | --- | --- | --- |
| [Linked lists](src/data_structures/linked_lists/README.md) | 6 | Singly, doubly, circular, ordered, array-based, skip list | Search O(n), head insert O(1) |
| [Stacks](src/data_structures/stacks/README.md) | 1 | LIFO, balanced parentheses, infix/postfix, big-number addition | push/pop O(1) |
| [Queues](src/data_structures/queues/README.md) | 8 | Linked, array, circular, deque, priority, heap-priority queues | enqueue/dequeue O(1); heap PQ O(log n) |
| [Heaps](src/data_structures/heaps/README.md) | 8 | Max-heap, min-heap, min-max heap, binomial heap, pairing heap, Fibonacci heap, heapify, heap sort | insert/extract O(log n) |
| [Hashing](src/data_structures/hashing/README.md) | 11 | Hash functions, chaining, probing, full hash map | O(1) average |
| [Trees](src/data_structures/trees/README.md) | 24 | BST, AVL, red-black, 2-3, splay, B-tree, B+ tree, interval, KD, range, segment, lazy segment, merge-sort, persistent segment, wavelet, disjoint sparse table, radix tree, suffix tree, trie, Fenwick | O(log n) balanced |
| [Disjoint set](src/data_structures/disjoint_set/README.md) | 3 | Union-find, weight rule, union by size, path compression | O(α(n)) with path compression |
| [Graph](src/data_structures/graph/README.md) | 1 | Adjacency-list representation for traversal and shortest paths | O(V + E) space |
| [LRU cache](src/data_structures/cache/README.md) | 1 | Hash map + doubly linked list | O(1) average |
| [Bloom filter](src/data_structures/bloom_filter/README.md) | 1 | Bit array, double hashing, false positives | O(k) |
| [Matrix](src/data_structures/matrix/README.md) | 1 | Row-major dense matrix, add, multiply, transpose | O(n³) multiply |
| [Bitset](src/data_structures/bitset/README.md) | 1 | Fixed-size bits packed into 64-bit words | O(1) set/test, O(N/64) count |
| [Dynamic array](src/data_structures/dynamic_array/README.md) | 1 | Growable array, amortised append | O(1) amortised push, O(n) insert |
| [Big integer](src/data_structures/big_integer/README.md) | 1 | Arbitrary-precision signed arithmetic | O(n) add, O(n²) multiply |
| [Ordered set/map](src/data_structures/ordered/README.md) | 3 | Treap, ordered set, ordered map, order statistics | Expected O(log n) |
| [Strings](src/data_structures/strings/README.md) | 3 | Suffix array with doubling, suffix automaton, rope | build O(n log² n) / O(n), rope edits O(log n) |
| [Sqrt decomposition](src/data_structures/sqrt_decomposition/README.md) | 1 | Block decomposition for range add/sum | O(√n) per operation |
| [Probabilistic](src/data_structures/probabilistic/README.md) | 3 | Count-min sketch, HyperLogLog, cuckoo filter | O(1) expected |
| [Sets](src/data_structures/sets/README.md) | 1 | Van Emde Boas set, predecessor/successor | O(1) for 16-bit universe |

### Algorithms

| Topic | Files | What you learn |
| --- | --- | --- |
| [Sorting](src/algorithms/sorting/README.md) | 11 | Bubble, selection, insertion, shell, merge, quick, heap, counting, radix, bucket, randomized select |
| [Searching](src/algorithms/searching/README.md) | 4 | Linear and binary search, iterative and recursive |
| [Recursion](src/algorithms/recursion/README.md) | 1 | Factorial, Tower of Hanoi, base cases |
| [Dynamic programming](src/algorithms/dynamic_programming/README.md) | 15 | Fibonacci, knapsack, LIS, LCS, edit distance, coin change, matrix chain, rod cutting, LPS, unbounded knapsack, partition, bitmask TSP, tree DP, digit DP, interval DP |
| [Graphs](src/algorithms/graph/README.md) | 18 | BFS, DFS, topological sort, SCC, shortest paths (Dijkstra, Bellman-Ford, Floyd-Warshall, SPFA, DAG, A*, Johnson), MSTs, bridges, LCA |
| [Strings](src/algorithms/strings/README.md) | 8 | KMP, Z-algorithm, Rabin-Karp, Manacher, Aho-Corasick, Boyer-Moore, Kasai LCP, two-string LCP |
| [Backtracking](src/algorithms/backtracking/README.md) | 5 | N-Queens, Sudoku, permutations/combinations/subsets, rat in a maze, palindrome partitioning |
| [Greedy](src/algorithms/greedy/README.md) | 5 | Activity selection, fractional knapsack, Huffman, merge intervals, jump game |
| [Number theory](src/algorithms/number_theory/README.md) | 6 | GCD, modular arithmetic, totient, factorisation, CRT, Pollard rho, matrix power, Gaussian elimination, NTT |
| [Bit manipulation](src/algorithms/bit_manipulation/README.md) | 1 | Popcount, bitmasks, bit tricks |
| [Two pointers](src/algorithms/two_pointers/README.md) | 1 | Sliding window and sorted two-sum |
| [Prefix sums](src/algorithms/prefix_sums/README.md) | 1 | Prefix sums, range queries, difference arrays |
| [Binary search on the answer](src/algorithms/binary_search_answer/README.md) | 1 | Monotone predicates, minimum capacity |
| [Game theory](src/algorithms/game_theory/README.md) | 1 | Nim and Sprague-Grundy numbers |
| [Randomized](src/algorithms/randomized/README.md) | 1 | Fisher-Yates shuffle and reservoir sampling |
| [Range queries](src/algorithms/range_queries/README.md) | 1 | Mo's algorithm for offline range queries |
| [Primality](src/algorithms/primality/README.md) | 5 | Trial division, sieve, Fermat, Miller-Rabin, AKS placeholder |
| [NP-complete](src/algorithms/np_complete/README.md) | 5 | Subset sum, knapsack, graph colouring, Hamiltonian cycle, TSP |

### Sorting at a glance

| Algorithm | Best | Average | Worst | Space | Stable |
| --- | --- | --- | --- | --- | --- |
| Bubble sort | O(n) | O(n²) | O(n²) | O(1) | Yes |
| Selection sort | O(n²) | O(n²) | O(n²) | O(1) | No |
| Insertion sort | O(n) | O(n²) | O(n²) | O(1) | Yes |
| Shell sort | O(n log n) | depends on gap | O(n²) | O(1) | No |
| Merge sort | O(n log n) | O(n log n) | O(n log n) | O(n) | Yes |
| Quick sort | O(n log n) | O(n log n) | O(n²) | O(log n) | No |
| Heap sort | O(n log n) | O(n log n) | O(n log n) | O(1) | No |
| Counting sort | O(n + k) | O(n + k) | O(n + k) | O(k) | Yes |
| Radix sort | O(d(n + k)) | O(d(n + k)) | O(d(n + k)) | O(n + k) | Yes |
| Bucket sort | O(n + k) | O(n + k) | O(n²) | O(n + k) | Yes* |

\* Stable when the per-bucket sort is stable. The full tables — including trees,
hashing, graphs, DP, and NP-complete problems — are in
[docs/COMPLEXITY_CHEATSHEET.md](docs/COMPLEXITY_CHEATSHEET.md).

## Repository layout

```text
data-structures-and-algorithms-library/
├── src/
│   ├── data_structures/
│   │   ├── linked_lists/          # 6 list variants
│   │   ├── stacks/                # stack + expression helpers
│   │   ├── queues/                # 8 queue variants
│   │   ├── heaps/                 # max-heap, min-heap, priority queue
│   │   ├── hashing/               # hash functions + collision handling
│   │   ├── trees/                 # BST, AVL, RB, 2-3, spatial, trie, Fenwick
│   │   ├── graph/                 # adjacency-list representation
│   │   ├── cache/                 # LRU cache
│   │   ├── disjoint_set/          # union-find + path compression
│   │   ├── matrix/                # dense matrix
│   │   ├── dynamic_array/         # growable array
│   │   ├── big_integer/           # arbitrary-precision integer
│   │   ├── ordered/               # treap, ordered set/map
│   │   ├── sqrt_decomposition/    # block decomposition
│   │   ├── probabilistic/         # sketches and filters
│   │   ├── sets/                  # van Emde Boas set
│   │   ├── bitset/                # fixed-size bitset
│   │   ├── bloom_filter/          # probabilistic membership
│   │   └── strings/               # suffix array
│   ├── algorithms/
│   │   ├── sorting/               # 10 sorts + randomized select
│   │   ├── searching/             # linear + binary, iter/recursive
│   │   ├── recursion/
│   │   ├── dynamic_programming/  # 15 DP examples
│   │   ├── graph/                 # traversal, SCC, shortest paths, MSTs, LCA, A*
│   │   ├── strings/               # KMP, Z, Rabin-Karp, Manacher, Aho-Corasick, Boyer-Moore
│   │   ├── backtracking/          # N-Queens, Sudoku, permutations, mazes
│   │   ├── greedy/                # activity selection, knapsack, Huffman, intervals
│   │   ├── number_theory/         # GCD, modular arithmetic, CRT, factorisation, linear algebra
│   │   ├── bit_manipulation/      # bit tricks
│   │   ├── two_pointers/          # sliding window
│   │   ├── prefix_sums/           # prefix sums + difference arrays
│   │   ├── binary_search_answer/  # monotone predicates
│   │   ├── game_theory/           # Nim, Grundy numbers
│   │   ├── randomized/            # shuffle, reservoir sampling
│   │   ├── range_queries/         # Mo's algorithm
│   │   ├── primality/
│   │   └── np_complete/
│   └── demos/                     # guided tour + structure demos
├── tests/                         # assertion-based tests
├── docs/
│   ├── BUILDING.md                # build, run, troubleshoot
│   ├── COVERAGE.md                # phased checklist of every topic
│   ├── COMPLEXITY_CHEATSHEET.md   # all complexity tables
│   ├── LEARNING_PATH.md           # suggested study order + exercises
│   └── ROADMAP.md                 # done, next, known limitations
├── scripts/
│   ├── build.ps1                  # Windows build helper
│   └── check_headers.sh           # compile every header on its own
├── CMakeLists.txt
├── CONTRIBUTING.md
└── LICENSE
```

## Learning path

Do not read everything at once. Follow
[docs/LEARNING_PATH.md](docs/LEARNING_PATH.md), which groups the repository into
ten stages:

1. Linear structures (lists, stacks, queues)
2. Hashing
3. Trees
4. Heaps and priority queues
5. Sorting
6. Searching and selection
7. Recursion and dynamic programming
8. Graphs
9. Advanced trees and spatial structures
10. Number theory and NP-complete problems

Each stage includes what to read, what to run, and one exercise that proves you
understood the idea.

## How to study a file

1. Read the top comment block: idea, time, space, and any warning.
2. Find the invariant. For example: "the left subtree contains only smaller
   values" or "`dp[w]` is the best value for capacity `w`".
3. Trace a tiny input by hand (three to five elements).
4. Run the example and compare with your trace.
5. Change one value or one line, predict the result, then run it again.

If an algorithm prints a trace, read the trace *before* looking at the code
again. It usually makes the recursion or the loop obvious.

## Testing and CI

```bash
cmake -S . -B build
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

The test suite contains:

- `tests/test_algorithms.cpp` — checks that every sort produces a sorted array,
  every search returns the right index, and the recursion examples are correct.
- `tests/test_structures.cpp` — checks the core operations of the linked list,
  stack, queues, BST, AVL, heaps, skip list, and hash table.
- `scripts/check_headers.sh` — compiles every header on its own, which catches
  missing includes and broken include guards.

GitHub Actions runs the build and tests on every push and pull request.

## Contributing

Contributions are welcome. Please read [CONTRIBUTING.md](CONTRIBUTING.md).

The most important rule: **do not delete teaching comments**. If you change the
behaviour, update the comment to match it. New topics, extra exercises, and
clearer explanations are all valuable.

## License

Released under the [MIT License](LICENSE).

## Credits

Created and maintained by **Amr Eldhshan** and contributors.

The suffix array implementation credits its original student authors in the file
header. Bloom filter, interval, spatial, and NP-complete files also contain
explanations written during coursework; those comments are preserved.

---

If this repository helped you learn, consider giving it a star and opening a
pull request with the next topic you study.

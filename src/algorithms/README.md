# Algorithms

Each folder here answers one question: **how does this algorithm actually
work?** The implementations avoid library shortcuts where the point is to learn
the idea.

| Folder | Topics |
| --- | --- |
| [`sorting/`](sorting/README.md) | Ten sorting algorithms and randomized selection |
| [`searching/`](searching/README.md) | Linear and binary search, iterative and recursive |
| [`recursion/`](recursion/README.md) | Factorial and Tower of Hanoi |
| [`dynamic_programming/`](dynamic_programming/README.md) | Fibonacci, knapsack, LIS, LCS, edit distance, coin change, matrix chain, rod cutting, LPS, partition, bitmask TSP, tree DP, digit DP, interval DP |
| [`graph/`](graph/README.md) | BFS, DFS, topological sort, SCC, shortest paths (including SPFA, DAG, A*, Johnson), MSTs, bridges, LCA |
| [`strings/`](strings/README.md) | KMP, Z-algorithm, Rabin-Karp, Manacher, Aho-Corasick, Boyer-Moore, Kasai LCP, two-string LCP |
| [`compression/`](compression/README.md) | RLE, Huffman (static, adaptive, modified), LZ77/LZ78/LZW, arithmetic coding, scalar/vector quantization, DPCM, DCT, baseline JPEG, motion estimation, MPEG-style I/P codec |
| [`backtracking/`](backtracking/README.md) | N-Queens, Sudoku, permutations/combinations/subsets, rat in a maze, palindrome partitioning |
| [`greedy/`](greedy/README.md) | Activity selection, fractional knapsack, Huffman coding, merge intervals, jump game |
| [`number_theory/`](number_theory/README.md) | GCD, modular arithmetic, totient, factorisation, CRT, Pollard rho, matrix power, Gaussian elimination, NTT |
| [`bit_manipulation/`](bit_manipulation/README.md) | Popcount, bitmasks, lowest set bit, bit length |
| [`two_pointers/`](two_pointers/README.md) | Sliding window and sorted two-sum |
| [`prefix_sums/`](prefix_sums/README.md) | Prefix sums, range queries, difference arrays |
| [`binary_search_answer/`](binary_search_answer/README.md) | Binary search on a monotone predicate |
| [`game_theory/`](game_theory/README.md) | Nim and Sprague-Grundy numbers |
| [`randomized/`](randomized/README.md) | Fisher-Yates shuffle and reservoir sampling |
| [`range_queries/`](range_queries/README.md) | Mo's algorithm for offline range queries |
| [`primality/`](primality/README.md) | Trial division, sieve, Fermat, Miller-Rabin, AKS placeholder |
| [`np_complete/`](np_complete/README.md) | Five backtracking / search examples |

## How to read an algorithm

1. Read the top comment block: idea, time, space, stability.
2. Identify the **invariant** — what is true before and after each loop or
   recursive call?
3. Trace a tiny input by hand (three to five elements).
4. Run it and compare with your trace.
5. Change one line and predict the effect before running again.

## Testing

The header-only algorithms are covered by `tests/test_algorithms.cpp`:

```bash
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

The complete programs are built as `algo_*` targets and can be run directly.

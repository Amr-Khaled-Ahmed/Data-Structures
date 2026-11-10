# Complexity cheat sheet

Use this page as a revision aid, then open the implementation and check the
comments for the reasoning. `n` is the number of elements, `k` an extra
parameter (range, buckets, output size), `V` vertices, and `E` edges.

## Data structures

| Structure | Access | Search | Insert | Delete | Extra space | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| Array-based list | O(1) | O(n) | O(n) | O(n) | O(n) | Contiguous storage |
| Singly linked list | O(n) | O(n) | O(1) at head | O(1) at head | O(n) | No random access |
| Doubly linked list | O(n) | O(n) | O(1) at ends | O(1) at ends | O(n) | Two links per node |
| Circular linked list | O(n) | O(n) | O(1) at ends | O(1) at ends | O(n) | Tail connects to head |
| Ordered linked list | O(n) | O(n) | O(n) | O(n) | O(n) | Keeps sorted order |
| Skip list | O(log n) avg | O(log n) avg | O(log n) avg | O(log n) avg | O(n) | Worst case O(n) |
| Trie | O(L) | O(L) | O(L) | O(L) | O(total characters) | `L` = key length; prefix search |
| Fenwick tree (BIT) | - | O(log n) prefix | O(log n) | - | O(n) | Point updates + prefix sums |
| Sparse table | - | O(1) range min | - | - | O(n log n) | Static array, idempotent operation |
| Deque | O(1) ends | O(n) | O(1) amortized ends | O(1) amortized ends | O(n) | Growable circular buffer |
| LRU cache | - | O(1) avg | O(1) avg | O(1) avg | O(capacity) | Hash map + linked list |
| Graph (adjacency list) | - | O(degree) edge | O(1) edge | O(degree) edge | O(V + E) | Iterate neighbours in O(degree) |
| Stack | O(n) | O(n) | O(1) push | O(1) pop | O(n) | LIFO |
| Queue | O(n) | O(n) | O(1) enqueue | O(1) dequeue | O(n) | FIFO |
| Circular array queue | O(n) | O(n) | O(1) | O(1) | O(capacity) | Fixed capacity |
| Priority queue (sorted list) | O(1) front | O(n) | O(n) | O(1) | O(n) | Keeps priorities ordered |
| Priority queue (binary heap) | O(1) peek | O(n) | O(log n) | O(log n) | O(n) | Extract-min/max |
| Binary heap | O(1) root | O(n) | O(log n) | O(log n) | O(n) | Build heap in O(n) |
| Min-heap (template) | O(1) top | O(n) | O(log n) | O(log n) | O(n) | Mirror of a max-heap |
| Min-max heap | O(1) min/max | O(n) | O(log n) | O(log n) | O(n) | Min levels and max levels |
| Binomial heap | O(log n) min | O(n) | O(log n) amortised | O(log n) | O(n) | Collection of binomial trees |
| Pairing heap | O(1) min | O(n) | O(1) insert | O(log n) amortised extract | O(n) | Meldable, left-child/next-sibling |
| Fibonacci heap | O(1) min | O(n) | O(1) insert | O(log n) amortised extract | O(n) | Lazy tree collection (no decrease-key here) |
| Binary search tree | O(h) | O(h) | O(h) | O(h) | O(n) | `h` is height: O(log n) avg, O(n) worst |
| AVL tree | O(log n) | O(log n) | O(log n) | O(log n) | O(n) | Height-balanced |
| Splay tree | O(log n) amortised | O(log n) amortised | O(log n) amortised | O(log n) amortised | O(n) | Self-adjusting, no balance metadata |
| Radix tree | O(L) | O(L) | O(L) | O(L) | O(nodes) | Compressed trie edges |
| Suffix tree (naive) | O(m) search | O(m) search | - | - | O(n²) here | Build by inserting all suffixes |
| B-tree | O(log n) | O(log n) | O(log n) | - | O(n) | Multi-way, minimum degree t |
| B+ tree | O(log n) | O(log n) | O(log n) | - | O(n) | All keys in leaves, range-friendly |
| Red-black tree | O(log n) | O(log n) | O(log n) | O(log n) | O(n) | Fewer rotations than AVL |
| 2-3 tree | O(log n) | O(log n) | O(log n) | O(log n) | O(n) | Multi-way search tree |
| Interval tree | O(log n) | O(log n + k) | O(log n) | O(log n) | O(n) | Augmented with subtree `max` |
| KD-tree | O(log n) avg | O(n^(1-1/d) + k) | n/a | n/a | O(n) | Static spatial index |
| 2D range tree | O(log² n + k) query | - | - | - | O(n log n) | Build O(n log n) |
| Segment tree | O(log n + k) query | - | O(log n) | O(log n) | O(n log n) | Interval overlap queries |
| Lazy segment tree | O(log n) range sum | - | O(log n) range add | - | O(n) | Pending updates pushed down |
| Sqrt decomposition | O(1) point | O(√n) range | O(√n) range add | - | O(n) | Block decomposition |
| Merge-sort tree | - | O(log² n) count | - | - | O(n log n) | Sorted vectors per segment |
| Persistent segment tree | O(log n) version | O(log n) range sum | O(log n) new nodes | - | O(n log n) versions | Old versions preserved |
| Wavelet tree | - | O(log σ) rank/kth | - | - | O(n log σ) | Prefix counts per level |
| Disjoint sparse table | - | O(1) RMQ | - | - | O(n log n) | Static, idempotent operations |
| Suffix array | O(m log n) search* | - | - | - | O(n) | Built in O(n log² n) here |
| Suffix automaton | O(m) pattern | O(m) contains | - | - | O(n) | Minimal DFA of all substrings |
| Disjoint set (union by size) | - | O(log n) find | O(log n) union | - | O(n) | Add path compression for near O(1) |
| Disjoint set (size + path compression) | - | O(α(n)) find | O(α(n)) union | - | O(n) | α is the inverse Ackermann function |
| Matrix (dense) | O(1) element | O(rows × columns) | - | - | O(rows × columns) | Multiply O(n³) for n × n |
| Bitset (fixed N) | O(1) bit | O(1) test | O(1) set | O(1) reset | O(N/64) words | `count`/bitwise O(N/64) |
| Dynamic array | O(1) | O(n) | O(1) amortised at end | O(n) middle | O(n) | Doubles capacity when full |
| Big integer | - | O(n) compare | O(n) add | - | O(n) limbs | Schoolbook multiply O(n²) |
| Treap / ordered set | O(log n) expected | O(log n) expected | O(log n) expected | O(log n) expected | O(n) | Random priorities + subtree sizes |
| Ordered map | O(log n) expected | O(log n) expected | O(log n) expected | O(log n) expected | O(n) | Sorted keys, iteration in order |
| Bloom filter | - | O(k) | O(k) | - | O(m) bits | False positives only |
| Hash table | - | O(1) avg | O(1) avg | O(1) avg | O(n) | O(n) worst case |
| Count-min sketch | - | O(depth) estimate | O(depth) add | - | O(width × depth) | Never underestimates |
| HyperLogLog | - | O(1) add | - | - | O(2^precision) | ~1.04/√m relative error |
| Cuckoo filter | - | O(1) expected | O(1) expected | O(1) expected | O(capacity) | Deletion supported, false positives |
| Van Emde Boas set | - | O(1) contains | O(1) insert | O(1) remove | O(2^16 / 8) | Fixed 16-bit universe, O(1) successor |

For the interval structures, `k` is the number of intervals reported.

## Sorting algorithms

| Algorithm | Best | Average | Worst | Space | Stable |
| --- | --- | --- | --- | --- | --- |
| Bubble sort | O(n) | O(n²) | O(n²) | O(1) | Yes |
| Selection sort | O(n²) | O(n²) | O(n²) | O(1) | No |
| Insertion sort | O(n) | O(n²) | O(n²) | O(1) | Yes |
| Shell sort | O(n log n) | Depends on gap | O(n²) | O(1) | No |
| Merge sort | O(n log n) | O(n log n) | O(n log n) | O(n) | Yes |
| Quick sort | O(n log n) | O(n log n) | O(n²) | O(log n) | No |
| Heap sort | O(n log n) | O(n log n) | O(n log n) | O(1) | No |
| Counting sort | O(n + k) | O(n + k) | O(n + k) | O(k) | Yes |
| Radix sort | O(d(n + k)) | O(d(n + k)) | O(d(n + k)) | O(n + k) | Yes |
| Bucket sort | O(n + k) | O(n + k) | O(n²) | O(n + k) | Yes* |

\* Stable when the per-bucket sort is stable.

## Selection

| Algorithm | Average | Worst | Space |
| --- | --- | --- | --- |
| Randomized select | O(n) | O(n²) | O(1) extra (recursion stack) |

## Searching

| Algorithm | Best | Average | Worst | Space | Requires sorted input |
| --- | --- | --- | --- | --- | --- |
| Linear / sequential search | O(1) | O(n) | O(n) | O(1) iterative, O(n) recursive | No |
| Binary search | O(1) | O(log n) | O(log n) | O(1) iterative, O(log n) recursive | Yes |
| Hash lookup | O(1) | O(1) | O(n) | O(n) | No |

## Graphs

| Algorithm | Time | Space | Finds |
| --- | --- | --- | --- |
| BFS | O(V + E) | O(V) | Shortest path in edge count, components |
| DFS | O(V + E) | O(V) | Traversal order, discovery/finish times |
| Topological sort (Kahn / DFS) | O(V + E) | O(V) | Dependency order of a DAG |
| Cycle detection | O(V + E) | O(V) | Whether a cycle exists |
| Connected components | O(V + E) | O(V) | Components of an undirected graph |
| Tarjan SCC | O(V + E) | O(V) | Strongly connected components |
| Dijkstra (binary heap) | O((V + E) log V) | O(V + E) | Shortest paths from one source |
| Bellman-Ford | O(V · E) | O(V) | Shortest paths with negative edges |
| Floyd-Warshall | O(V³) | O(V²) | All-pairs shortest paths |
| Kruskal | O(E log E) | O(V + E) | Minimum spanning tree |
| Prim (binary heap) | O(E log V) | O(V + E) | Minimum spanning tree |
| Bipartite check | O(V + E) | O(V) | 2-colouring / odd-cycle test |
| Bridges / articulation points | O(V + E) | O(V) | Cut edges and cut vertices |
| LCA (binary lifting) | O(V log V) build, O(log V) query | O(V log V) | Lowest common ancestor, tree distance |
| SPFA | O(V · E) worst | O(V) | Queue-based Bellman-Ford, negative-cycle detection |
| DAG shortest paths | O(V + E) | O(V) | One pass after topological sort |
| A* | O(E log V) worst | O(V) | Goal-directed search with `f = g + h` |
| Johnson | O(VE + V(V + E) log V) | O(V²) | All pairs with negative edges (no negative cycle) |

## Dynamic programming

| Problem | Time | Space |
| --- | --- | --- |
| Fibonacci (plain recursion) | O(2ⁿ) | O(n) stack |
| Fibonacci (matrix power) | O(log n) | O(log n) |
| Fibonacci (iterative DP) | O(n) | O(1) |
| 0/1 knapsack | O(nW) | O(W) |
| Longest increasing subsequence | O(n log n) | O(n) |
| Longest common subsequence | O(mn) | O(mn) |
| Edit distance | O(mn) | O(mn) |
| Coin change | O(nA) | O(A) |
| Matrix chain multiplication | O(n³) | O(n²) |
| Rod cutting | O(n²) | O(n) |
| Longest palindromic subsequence | O(n²) | O(n²) |
| Unbounded knapsack | O(nW) | O(W) |
| Partition equal subset | O(nS) | O(S) |
| Bitmask TSP (Held-Karp) | O(2ⁿ n²) | O(2ⁿ n) |
| Tree DP (independent set) | O(V) | O(V) |
| Digit DP | O(digits × sum × 10) | O(sum) |
| Interval DP (burst balloons) | O(n³) | O(n²) |

## Strings

| Algorithm | Time | Space | Use |
| --- | --- | --- | --- |
| KMP | O(n + m) | O(m) | Pattern matching |
| Z-algorithm | O(n + m) | O(n + m) | Prefix matches, pattern search |
| Rabin-Karp | O(n + m) expected, O(nm) worst | O(1) | Rolling-hash matching |
| Manacher | O(n) | O(n) | Longest palindromic substring |
| Aho-Corasick | O(n + total pattern length + matches) | O(total pattern length) | Many patterns at once |
| Boyer-Moore | O(nm) worst, sublinear typical | O(alphabet) | Right-to-left matching with bad-character skips |
| Kasai LCP | O(n) | O(n) | LCP between adjacent suffixes |
| Two-string LCP | O(n log n) | O(1) | Binary search with direct comparisons |

## Backtracking

| Problem | Typical time | Pruning |
| --- | --- | --- |
| N-Queens | O(n!) | Columns and diagonals |
| Sudoku | Exponential | Row/column/box checks |
| Permutations | O(n!) | Used positions |
| Combinations / subsets | O(2ⁿ) | Loop bounds |
| Rat in a maze | O(2^(rows + columns)) worst | Walls and visited cells |
| Palindrome partitioning | O(n · 2ⁿ) | Palindrome check |

## Greedy

| Problem | Time | Greedy choice |
| --- | --- | --- |
| Activity selection | O(n log n) | Earliest finish |
| Fractional knapsack | O(n log n) | Best value per weight |
| Huffman coding | O(n log n) | Merge two least frequent |
| Merge intervals | O(n log n) | Keep the furthest end |
| Jump game | O(n) | Extend the current reach |

## Number theory

| Operation | Time |
| --- | --- |
| GCD / LCM (Euclid) | O(log min(a, b)) |
| Extended GCD | O(log min(a, b)) |
| Modular exponentiation | O(log exponent) |
| Modular inverse | O(log modulus) |
| Totient (trial factorisation) | O(√n) |
| Factorisation | O(√n) |
| Binomial modulo prime | O(n + log modulus) |
| Catalan (loop) | O(n) |
| Chinese remainder theorem | O(k²) for k moduli (with extended GCD) |
| Pollard's rho | O(n^(1/4)) expected per factor |
| Matrix exponentiation | O(k³ log n) for k × k matrices |
| Gaussian elimination | O(n³) |
| NTT polynomial multiplication | O(n log n) |

## Two pointers / sliding window

| Problem | Time |
| --- | --- |
| Fixed-size window sum | O(n) |
| Longest unique substring | O(n) |
| Minimum window substring | O(n) |
| Two-sum on sorted array | O(n) |

## Prefix sums and difference arrays

| Problem | Time |
| --- | --- |
| Build prefix sums | O(n) |
| Range sum query | O(1) |
| Range update (difference array) | O(1) per update |
| Reconstruct after range updates | O(n) |

## Binary search on the answer

| Problem | Time |
| --- | --- |
| Minimum capacity / speed with monotone predicate | O(n log(range)) |

## Game theory

| Problem | Time |
| --- | --- |
| Nim winner | O(n) |
| Nim winning move | O(n) |
| Grundy numbers up to N | O(N × moves) |

## Offline range queries

| Algorithm | Time | Notes |
| --- | --- | --- |
| Mo's algorithm | O((n + q)√n) | Sorting queries by block |

## Randomized algorithms

| Algorithm | Expected time | Space |
| --- | --- | --- |
| Fisher-Yates shuffle | O(n) | O(1) |
| Reservoir sampling | O(n) | O(k) |

## Primality

| Algorithm | Time | Type |
| --- | --- | --- |
| Trial division | O(√n) | Deterministic |
| Sieve of Eratosthenes | O(n log log n) | Deterministic, finds all primes up to n |
| Fermat test | O(k log n) | Probabilistic |
| Miller-Rabin | O(k log³ n) | Probabilistic |
| AKS | Polynomial | Deterministic (the current file is a teaching placeholder) |

## NP-complete examples

All of these explore a search tree and can take exponential time in the worst
case. They are here to show *how* the search works, not to solve large inputs.

| Problem | Typical time | Notes |
| --- | --- | --- |
| Subset sum | O(2ⁿ) | Include/exclude search |
| 0/1 knapsack (branch and bound style) | O(2ⁿ) | Compare with the DP version |
| Graph coloring | O(m^V) | Backtracking over colours |
| Hamiltonian cycle | O(V!) | Backtracking over vertices |
| Travelling salesman | O(V!) | Enumerate tours / permutations |

\* O(m log n) assumes the pattern search is added on top; this repo only builds the array.

## Compression

| Codec | Encode | Decode | Space | Lossless | Notes |
| --- | --- | --- | --- | --- | --- |
| RLE | O(n) | O(n) | O(n) | Yes | Great for long runs, can expand other data |
| Huffman (static) | O(n log k) | O(n) | O(k) | Yes | Canonical table stored in the stream |
| Adaptive Huffman (FGK) | O(n · k) | O(n · k) | O(k) | Yes | No table transmitted; leader search costs O(k) |
| Modified Huffman (variant) | O(n log k) | O(n) | O(n) | Yes | Teaching model, not T.4 compatible |
| LZ77 | O(n · w · m) | O(n) | O(n) | Yes | `w` window, `m` max match; hash chains make it faster |
| LZ78 | O(n · phrase) | O(n) | O(n) | Yes | Explicit growing dictionary |
| LZW | O(n) expected | O(n) | O(dictionary) | Yes | 12-bit codes, clear code resets at 4096 entries |
| Arithmetic coding | O(n · k) | O(n · k) | O(k) | Yes | Near entropy limit; static model here |
| Scalar quantization | O(n) | O(n) | O(n) | No | Error <= step/2 |
| Vector quantization | O(n · k · d) | O(n) | O(k · d) | No | k-means training, `d` dimensions |
| DPCM | O(n) | O(n) | O(n) | Yes (base) | Quantized variant is lossy |
| DCT (direct) | O(N³) for an N×N block | O(N³) | O(N²) | Yes (inverse) | Fast DCT is O(N² log N) for 2D |
| JPEG baseline | O(pixels · block work) | O(pixels · block work) | O(pixels) | No | Grayscale; standard tables |
| Motion estimation (full search) | O(pixels · (2r+1)²) | O(pixels) | O(pixels) | No | `r` search radius |
| MPEG-style I/P | dominated by motion search | O(pixels) | O(pixels) | No | Teaching model, not MPEG-conformant |

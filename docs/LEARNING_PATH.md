# Learning path

This is a suggested order for studying the repository. Each stage explains what
to read, what to run, and one small exercise that proves you understood the
idea. You do not have to follow the order, but later topics reuse earlier ones.

> **How to study a file:** read the top comment block first, then follow the
> main function, then read the helper functions it calls. Run the example. Then
> change one value and predict the output before running it again.

## Stage 0 — Set up

- Read [Building and running](BUILDING.md).
- Run the guided tour: `./build/bin/dsa_demo`.
- Keep [the complexity cheat sheet](COMPLEXITY_CHEATSHEET.md) open.

## Stage 1 — Linear data structures

**Read**

- [`linked_lists/`](../src/data_structures/linked_lists/README.md) — arrays vs
  pointers, singly, doubly, circular, ordered, skip list.
- [`stacks/`](../src/data_structures/stacks/README.md) — LIFO, balanced
  parentheses, infix and postfix.
- [`queues/`](../src/data_structures/queues/README.md) — FIFO, circular
  buffers, linked queues, priority queues.

**Run**

```bash
./build/bin/demo_heap
./build/bin/ds_disjoint_set_array
```

**Exercise**

Implement `reverse()` for the singly linked list by changing links (do not
copy the values into an array). Predict the complexity before you start.

## Stage 2 — Hashing

**Read** [`hashing/`](../src/data_structures/hashing/README.md).

**What to notice**

- The hash function decides the index.
- Collisions are normal; the second half of the folder is about handling them.
- Open addressing stores keys inside the table; separate chaining stores them
  outside the table.

**Exercise**

Insert the same eight keys into `linear_probing.h` and `separate_chaining.h`.
Count the probes needed to find the last key. Explain the difference.

## Stage 3 — Trees

**Read**

- [`trees/bst.h`](../src/data_structures/trees/bst.h) — the basic search tree.
- [`trees/avl.h`](../src/data_structures/trees/avl.h) — rotations keep the
  height logarithmic.
- [`trees/red_black_tree.h`](../src/data_structures/trees/red_black_tree.h) —
  colour rules instead of strict balance.
- [`trees/two_three_tree.h`](../src/data_structures/trees/two_three_tree.h) —
  multi-way nodes.

**Exercise**

Insert `1, 2, 3, 4, 5` into a plain BST and into an AVL tree. Draw both trees.
Why does the AVL tree stay shallow?

## Stage 4 — Heaps and priority queues

**Read** [`heaps/`](../src/data_structures/heaps/README.md) and the
[`priority queue`](../src/data_structures/queues/heap_priority_queue.h)
implementation.

**Exercise**

Add `decreaseKey(index, newValue)` to the max-heap. Which direction does the
element move, and why is the operation O(log n)?

## Stage 5 — Sorting

**Read** [`sorting/`](../src/algorithms/sorting/README.md).

Suggested order:

1. Bubble, selection, insertion — the O(n²) family.
2. Shell sort — a better insertion sort.
3. Merge sort — divide and conquer with extra space.
4. Quick sort — partition in place; notice the pivot choice.
5. Heap sort — reuse the heap idea.
6. Counting, radix, bucket — trade memory for linear time.

**Run**

```bash
./build/bin/dsa_demo
```

**Exercise**

Add a comparison counter to bubble sort. How many comparisons does it make on
an already sorted array? Now do the same for the optimized version.

## Stage 6 — Searching and selection

**Read** [`searching/`](../src/algorithms/searching/README.md) and
[`randomized_select.h`](../src/algorithms/sorting/randomized_select.h).

**Exercise**

Find the third smallest element with `randomizedSelect`. Then sort the array and
index it. Which is faster, and when does that matter?

## Stage 7 — Recursion and dynamic programming

**Read**

- [`recursion/`](../src/algorithms/recursion/README.md)
- [`dynamic_programming/`](../src/algorithms/dynamic_programming/README.md)

**Exercise**

Compare the three Fibonacci implementations in `fibonacci.cpp`. Time them for
`n = 35`. Explain why the recursive version is so much slower.

## Stage 8 — Graphs

**Read** [`graph/`](../src/algorithms/graph/README.md).

**Exercise**

Modify the Dijkstra graph so it contains an unreachable vertex. What distance
does the algorithm report? Why?

## Stage 9 — Advanced trees and spatial structures

**Read** [`trees/`](../src/data_structures/trees/README.md), then pick the ones
that match your interests:

- Interval trees and segment trees for range/overlap queries.
- KD-trees and range trees for spatial queries.
- Union-find for connectivity.
- Suffix arrays for string search.

**Exercise**

Insert `{1,5}`, `{4,8}`, `{10,14}` into the segment tree. Query point `6`. Which
intervals are returned, and how many tree nodes were visited?

## Stage 10 — Number theory and NP-complete problems

**Read**

- [`primality/`](../src/algorithms/primality/README.md)
- [`np_complete/`](../src/algorithms/np_complete/README.md)

**Exercise**

Run trial division and Miller-Rabin on `n = 561` (a Carmichael number). What
does each algorithm say, and why is that a useful lesson about probabilistic
tests?

## Stage 11 — Data compression and information theory

**Read**

- [`compression/`](../src/algorithms/compression/README.md) — start with the
  lossless coders, then quantization, then JPEG and motion estimation.
- [`huffman.h`](../src/algorithms/compression/huffman.h) — compare the encoded
  size with `byteEntropy()` to see how close the code gets to the limit.
- [`arithmetic_coding.h`](../src/algorithms/compression/arithmetic_coding.h) —
  read the floating-point form first; it is the clearest explanation.

**Run**

```bash
./build/bin/demo_compression
./build/bin/demo_jpeg
./build/bin/demo_mpeg
```

**Exercise**

Compress the same text with Huffman, LZW, and arithmetic coding. Which wins,
and why? Then lower the JPEG quality from 90 to 40 and watch the PSNR fall.

## What to do next

Pick one of the ideas in the [roadmap](ROADMAP.md), implement it in the same
style as the existing files, and add a test. Teaching a concept is the best way
to prove you understand it.

cmake_minimum_required(VERSION 3.16)
project(dsa_learning LANGUAGES CXX)

# ---------------------------------------------------------------------------
# Global settings
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
foreach(config Debug Release RelWithDebInfo MinSizeRel)
    string(TOUPPER "${config}" config_upper)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config_upper} "${CMAKE_BINARY_DIR}/bin")
endforeach()

option(DSA_BUILD_DEMOS "Build the standalone example programs" ON)
option(DSA_BUILD_TESTS "Build the test programs" ON)

# ---------------------------------------------------------------------------
# Header-only library
# ---------------------------------------------------------------------------
add_library(dsa_headers INTERFACE)
target_include_directories(dsa_headers INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# Some example files use the GNU-only <bits/stdc++.h> header. On MSVC we
# provide a small compatibility header so the same code still compiles.
if(MSVC)
    target_include_directories(dsa_headers INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/compat"
    )
endif()

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------
function(dsa_enable_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra)
    endif()
endfunction()

function(dsa_add_demo target source)
    if(DSA_BUILD_DEMOS)
        add_executable(${target} "${source}")
        target_link_libraries(${target} PRIVATE dsa_headers)
        dsa_enable_warnings(${target})
    endif()
endfunction()

# ---------------------------------------------------------------------------
# Learning tour (default target)
# ---------------------------------------------------------------------------
dsa_add_demo(dsa_demo "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/main.cpp")
dsa_add_demo(demo_heap "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/heap_demo.cpp")
dsa_add_demo(demo_trie "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/trie_demo.cpp")
dsa_add_demo(demo_fenwick "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/fenwick_demo.cpp")
dsa_add_demo(demo_sparse_table "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/sparse_table_demo.cpp")
dsa_add_demo(demo_deque "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/deque_demo.cpp")
dsa_add_demo(demo_lru_cache "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/lru_cache_demo.cpp")
dsa_add_demo(demo_hash_map "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/hash_map_demo.cpp")
dsa_add_demo(demo_matrix "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/matrix_demo.cpp")
dsa_add_demo(demo_bitset "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/bitset_demo.cpp")
dsa_add_demo(demo_min_heap "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/min_heap_demo.cpp")
dsa_add_demo(demo_union_find "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/union_find_demo.cpp")
dsa_add_demo(demo_dynamic_array "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/dynamic_array_demo.cpp")
dsa_add_demo(demo_big_integer "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/big_integer_demo.cpp")
dsa_add_demo(demo_ordered "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/ordered_demo.cpp")
dsa_add_demo(demo_splay_tree "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/splay_tree_demo.cpp")
dsa_add_demo(demo_min_max_heap "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/min_max_heap_demo.cpp")
dsa_add_demo(demo_binomial_heap "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/binomial_heap_demo.cpp")
dsa_add_demo(demo_lazy_segment_tree "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/lazy_segment_tree_demo.cpp")
dsa_add_demo(demo_sqrt_decomposition "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/sqrt_decomposition_demo.cpp")
dsa_add_demo(demo_count_min_sketch "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/count_min_sketch_demo.cpp")
dsa_add_demo(demo_hyperloglog "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/hyperloglog_demo.cpp")
dsa_add_demo(demo_cuckoo_filter "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/cuckoo_filter_demo.cpp")
dsa_add_demo(demo_suffix_automaton "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/suffix_automaton_demo.cpp")
dsa_add_demo(demo_merge_sort_tree "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/merge_sort_tree_demo.cpp")
dsa_add_demo(demo_persistent_segment_tree "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/persistent_segment_tree_demo.cpp")
dsa_add_demo(demo_wavelet_tree "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/wavelet_tree_demo.cpp")
dsa_add_demo(demo_disjoint_sparse_table "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/disjoint_sparse_table_demo.cpp")
dsa_add_demo(demo_pairing_heap "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/pairing_heap_demo.cpp")
dsa_add_demo(demo_radix_tree "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/radix_tree_demo.cpp")
dsa_add_demo(demo_suffix_tree "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/suffix_tree_demo.cpp")
dsa_add_demo(demo_van_emde_boas "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/van_emde_boas_demo.cpp")
dsa_add_demo(demo_b_tree "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/b_tree_demo.cpp")
dsa_add_demo(demo_b_plus_tree "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/b_plus_tree_demo.cpp")
dsa_add_demo(demo_fibonacci_heap "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/fibonacci_heap_demo.cpp")
dsa_add_demo(demo_rope "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/rope_demo.cpp")
dsa_add_demo(demo_compression "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/compression_demo.cpp")
dsa_add_demo(demo_jpeg "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/jpeg_demo.cpp")
dsa_add_demo(demo_mpeg "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/mpeg_demo.cpp")

# ---------------------------------------------------------------------------
# Data structure examples that are complete programs
# ---------------------------------------------------------------------------
dsa_add_demo(ds_bloom_filter            "${CMAKE_CURRENT_SOURCE_DIR}/src/data_structures/bloom_filter/bloom_filter.cpp")
dsa_add_demo(ds_disjoint_set_array      "${CMAKE_CURRENT_SOURCE_DIR}/src/data_structures/disjoint_set/disjoint_set_array.cpp")
dsa_add_demo(ds_disjoint_set_tree       "${CMAKE_CURRENT_SOURCE_DIR}/src/data_structures/disjoint_set/disjoint_set_tree.cpp")
dsa_add_demo(ds_suffix_array            "${CMAKE_CURRENT_SOURCE_DIR}/src/data_structures/strings/suffix_array.cpp")
dsa_add_demo(ds_interval_tree_augmented "${CMAKE_CURRENT_SOURCE_DIR}/src/data_structures/trees/interval_tree_augmented.cpp")
dsa_add_demo(ds_red_black_interval_tree "${CMAKE_CURRENT_SOURCE_DIR}/src/data_structures/trees/red_black_interval_tree.cpp")
dsa_add_demo(ds_kd_tree                 "${CMAKE_CURRENT_SOURCE_DIR}/src/data_structures/trees/kd_tree.cpp")
dsa_add_demo(ds_range_tree_2d           "${CMAKE_CURRENT_SOURCE_DIR}/src/data_structures/trees/range_tree_2d.cpp")
dsa_add_demo(ds_range_tree_d_dim        "${CMAKE_CURRENT_SOURCE_DIR}/src/data_structures/trees/range_tree_d_dim.cpp")
dsa_add_demo(ds_segment_tree            "${CMAKE_CURRENT_SOURCE_DIR}/src/data_structures/trees/segment_tree.cpp")

# ---------------------------------------------------------------------------
# Algorithm examples that are complete programs
# ---------------------------------------------------------------------------
dsa_add_demo(algo_fibonacci             "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/dynamic_programming/fibonacci.cpp")
dsa_add_demo(algo_knapsack_dp           "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/dynamic_programming/knapsack.cpp")
dsa_add_demo(algo_lis                   "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/dynamic_programming/lis.cpp")
dsa_add_demo(algo_lcs                   "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/dynamic_programming/lcs.cpp")
dsa_add_demo(algo_edit_distance         "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/dynamic_programming/edit_distance.cpp")
dsa_add_demo(algo_coin_change           "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/dynamic_programming/coin_change.cpp")
dsa_add_demo(algo_matrix_chain          "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/dynamic_programming/matrix_chain.cpp")
dsa_add_demo(algo_rod_cutting           "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/dynamic_programming/rod_cutting.cpp")
dsa_add_demo(algo_lps                   "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/dynamic_programming/longest_palindromic_subsequence.cpp")
dsa_add_demo(algo_unbounded_knapsack    "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/dynamic_programming/unbounded_knapsack.cpp")
dsa_add_demo(algo_partition             "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/dynamic_programming/partition_equal_subset.cpp")
dsa_add_demo(algo_tsp_bitmask           "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/dynamic_programming/tsp_bitmask.cpp")
dsa_add_demo(algo_tree_dp               "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/dynamic_programming/tree_dp.cpp")
dsa_add_demo(algo_digit_dp              "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/dynamic_programming/digit_dp.cpp")
dsa_add_demo(algo_interval_dp           "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/dynamic_programming/interval_dp.cpp")

dsa_add_demo(algo_kmp                   "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/strings/kmp.cpp")
dsa_add_demo(algo_z_algorithm           "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/strings/z_algorithm.cpp")
dsa_add_demo(algo_rabin_karp            "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/strings/rabin_karp.cpp")
dsa_add_demo(algo_manacher              "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/strings/manacher.cpp")
dsa_add_demo(algo_aho_corasick          "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/strings/aho_corasick.cpp")
dsa_add_demo(algo_boyer_moore           "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/strings/boyer_moore.cpp")
dsa_add_demo(algo_lcp_kasai             "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/strings/lcp_kasai.cpp")
dsa_add_demo(algo_lcp_two_strings       "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/strings/lcp_two_strings.cpp")

dsa_add_demo(algo_n_queens              "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/backtracking/n_queens.cpp")
dsa_add_demo(algo_sudoku                "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/backtracking/sudoku_solver.cpp")
dsa_add_demo(algo_permutations          "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/backtracking/permutations.cpp")
dsa_add_demo(algo_rat_in_maze           "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/backtracking/rat_in_maze.cpp")
dsa_add_demo(algo_palindrome_partition  "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/backtracking/palindrome_partitioning.cpp")

dsa_add_demo(algo_activity_selection    "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/greedy/activity_selection.cpp")
dsa_add_demo(algo_fractional_knapsack   "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/greedy/fractional_knapsack.cpp")
dsa_add_demo(algo_huffman               "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/greedy/huffman_coding.cpp")
dsa_add_demo(algo_merge_intervals       "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/greedy/merge_intervals.cpp")
dsa_add_demo(algo_jump_game             "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/greedy/jump_game.cpp")

dsa_add_demo(demo_number_theory         "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/number_theory_demo.cpp")
dsa_add_demo(demo_bit_manipulation      "${CMAKE_CURRENT_SOURCE_DIR}/src/demos/bit_manipulation_demo.cpp")

dsa_add_demo(algo_sliding_window         "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/two_pointers/sliding_window.cpp")
dsa_add_demo(algo_prefix_sums            "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/prefix_sums/prefix_sums.cpp")
dsa_add_demo(algo_binary_search_answer   "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/binary_search_answer/binary_search_answer.cpp")
dsa_add_demo(algo_game_theory            "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/game_theory/game_theory.cpp")
dsa_add_demo(algo_randomized             "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/randomized/randomized.cpp")

dsa_add_demo(algo_crt                   "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/number_theory/crt.cpp")
dsa_add_demo(algo_pollard_rho           "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/number_theory/pollard_rho.cpp")
dsa_add_demo(algo_matrix_exponentiation "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/number_theory/matrix_exponentiation.cpp")
dsa_add_demo(algo_gaussian_elimination  "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/number_theory/gaussian_elimination.cpp")
dsa_add_demo(algo_ntt                   "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/number_theory/ntt.cpp")
dsa_add_demo(algo_bfs                   "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/graph/bfs.cpp")
dsa_add_demo(algo_dfs                   "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/graph/dfs.cpp")
dsa_add_demo(algo_topological_sort      "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/graph/topological_sort.cpp")
dsa_add_demo(algo_cycle_detection       "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/graph/cycle_detection.cpp")
dsa_add_demo(algo_connected_components  "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/graph/connected_components.cpp")
dsa_add_demo(algo_scc_tarjan            "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/graph/scc_tarjan.cpp")
dsa_add_demo(algo_bellman_ford          "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/graph/bellman_ford.cpp")
dsa_add_demo(algo_floyd_warshall        "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/graph/floyd_warshall.cpp")
dsa_add_demo(algo_prim                  "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/graph/prim_mst.cpp")
dsa_add_demo(algo_bipartite_check       "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/graph/bipartite_check.cpp")
dsa_add_demo(algo_bridges_articulation  "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/graph/bridges_articulation.cpp")
dsa_add_demo(algo_lca                   "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/graph/lca_binary_lifting.cpp")
dsa_add_demo(algo_spfa                  "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/graph/spfa.cpp")
dsa_add_demo(algo_dag_shortest_path     "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/graph/dag_shortest_path.cpp")
dsa_add_demo(algo_a_star                "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/graph/a_star.cpp")
dsa_add_demo(algo_johnson               "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/graph/johnson.cpp")
dsa_add_demo(algo_mos                   "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/range_queries/mos_algorithm.cpp")
dsa_add_demo(algo_dijkstra              "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/graph/dijkstra.cpp")
dsa_add_demo(algo_kruskal               "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/graph/kruskal_mst.cpp")

dsa_add_demo(algo_trial_division        "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/primality/trial_division.cpp")
dsa_add_demo(algo_sieve                 "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/primality/sieve_of_eratosthenes.cpp")
dsa_add_demo(algo_fermat                "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/primality/fermat_primality_test.cpp")
dsa_add_demo(algo_miller_rabin          "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/primality/miller_rabin_test.cpp")
dsa_add_demo(algo_aks                   "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/primality/aks_primality_test.cpp")

dsa_add_demo(algo_np_graph_coloring     "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/np_complete/graph_coloring.cpp")
dsa_add_demo(algo_np_hamiltonian        "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/np_complete/hamiltonian_cycle.cpp")
dsa_add_demo(algo_np_knapsack           "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/np_complete/knapsack.cpp")
dsa_add_demo(algo_np_subset_sum         "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/np_complete/subset_sum.cpp")
dsa_add_demo(algo_np_tsp                "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/np_complete/traveling_salesman.cpp")

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------
if(DSA_BUILD_TESTS)
    enable_testing()

    add_executable(test_algorithms "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_algorithms.cpp")
    target_link_libraries(test_algorithms PRIVATE dsa_headers)
    dsa_enable_warnings(test_algorithms)
    add_test(NAME algorithms COMMAND test_algorithms)

    add_executable(test_structures "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_structures.cpp")
    target_link_libraries(test_structures PRIVATE dsa_headers)
    dsa_enable_warnings(test_structures)
    add_test(NAME structures COMMAND test_structures)

    add_executable(test_compression "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_compression.cpp")
    target_link_libraries(test_compression PRIVATE dsa_headers)
    dsa_enable_warnings(test_compression)
    add_test(NAME compression COMMAND test_compression)

    # Smoke tests for the standalone graph demos: each program must exit 0.
    foreach(demo
            algo_bfs algo_dfs algo_topological_sort algo_cycle_detection
            algo_connected_components algo_scc_tarjan algo_bellman_ford
            algo_floyd_warshall algo_prim algo_bipartite_check
            algo_bridges_articulation algo_lca
            demo_trie demo_fenwick demo_sparse_table demo_deque
            demo_lru_cache demo_hash_map demo_matrix demo_bitset
            demo_min_heap demo_union_find demo_dynamic_array demo_big_integer
            demo_ordered demo_splay_tree demo_min_max_heap demo_binomial_heap
            demo_lazy_segment_tree demo_sqrt_decomposition
            demo_count_min_sketch demo_hyperloglog demo_cuckoo_filter
            demo_suffix_automaton demo_merge_sort_tree
            demo_persistent_segment_tree demo_wavelet_tree
            demo_disjoint_sparse_table algo_mos demo_pairing_heap
            demo_radix_tree demo_suffix_tree demo_van_emde_boas
            demo_b_tree demo_b_plus_tree demo_fibonacci_heap demo_rope
            demo_compression demo_jpeg demo_mpeg
            algo_unbounded_knapsack algo_partition algo_tsp_bitmask
            algo_tree_dp algo_digit_dp
            algo_aho_corasick algo_boyer_moore algo_lcp_kasai
            algo_rat_in_maze algo_palindrome_partition
            algo_merge_intervals algo_jump_game
            algo_spfa algo_dag_shortest_path algo_a_star
            algo_prefix_sums algo_binary_search_answer algo_game_theory
            algo_randomized algo_crt algo_pollard_rho
            algo_matrix_exponentiation algo_gaussian_elimination algo_ntt
            algo_interval_dp algo_lcp_two_strings algo_johnson
            algo_lis algo_lcs algo_edit_distance algo_coin_change
            algo_matrix_chain algo_rod_cutting algo_lps
            algo_kmp algo_z_algorithm algo_rabin_karp algo_manacher
            algo_n_queens algo_sudoku algo_permutations
            algo_activity_selection algo_fractional_knapsack algo_huffman
            demo_number_theory demo_bit_manipulation algo_sliding_window)
        if(TARGET ${demo})
            add_test(NAME ${demo} COMMAND ${demo})
        endif()
    endforeach()

    if(UNIX)
        add_test(
            NAME header_compile_check
            COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/scripts/check_headers.sh"
                    "${CMAKE_CURRENT_SOURCE_DIR}"
                    "${CMAKE_CXX_COMPILER}"
        )
    endif()
endif()

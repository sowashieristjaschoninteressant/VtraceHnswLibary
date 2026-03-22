#ifndef HNSW_TESTS_H
#define HNSW_TESTS_H
#include "vtraceCommon.h"
#include "vtrace.h"
#include "heap.h"
#include "alloc.h"
// utils
extern Graph* mockGraphOneLayer(uint32 efSearch);
extern Graph* mockGraphTwoLayer(int32 efSearch);

extern void INSERT_POP_MAXHEAP_TEST();
extern void INSERT_POP_MINHEAP_TEST();
extern void DETERMINISTIC_INSERTPOP_TEST();
extern void HEAP_TESTS();

extern void SIMPLE_SEARCH_LAYERTEST();
extern void SELECT_NEAREST_NABOURS();
extern void INIT_GRAPH_TEST();
extern void ANN_SEARCH_TEST_ONE_LAYER();
extern void ANN_SEARCH_TEST_HEAP_TRIMMING();
extern void ANN_SEARCH_TEST_CHANGE_ENTRYPOINT();
extern void ANN_SEARCH_TEST_TWO_LAYERS_MOCK();
extern void FULL_API_INSERT_SEARCH_TEST();
extern void GRAPH_TESTS();

extern void ARENA_TESTS();
extern void ARENA_ALLOCATEBIG_TEST();
extern void ARENA_ALIGNMENT_TEST();
extern void ARENA_ALLOCATEOVERFLOW_TEST();
extern void CHAINARENA_READWRITEALLOCATION_TEST();
extern void CHAINARENA_USENEXT_TEST();
extern void CHAINARENA_GROW_TEST();


extern void INSERT_TESTS();
extern void test_insert_first_node();
extern void test_bidirectionalLinks();
extern void test_max_neigbours_respected();
extern void test_expandGraph();

#endif
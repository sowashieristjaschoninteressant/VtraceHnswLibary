#ifndef HNSW_TESTS_H
#define HNSW_TESTS_H
#include "vtraceCommon.h"
#include "vtrace.h"
#include "heap.h"

// utils
Graph* mockGraphOneLayer(uint32 efSearch);

extern void INSERT_POP_MAXHEAP_TEST();
extern void INSERT_POP_MINHEAP_TEST();
//NOTE: i implemented the select nabours algo and something went wrong with my heap implementation i have no clue what exactly as of rn
extern void DETERMINISTIC_INSERTPOP_TEST();
extern void HEAP_TESTS();

extern void SIMPLE_SEARCH_LAYERTEST();
extern void SELECT_NEAREST_NABOURS();
extern void INIT_GRAPH_TEST();
extern void GRAPH_TESTS();

#endif
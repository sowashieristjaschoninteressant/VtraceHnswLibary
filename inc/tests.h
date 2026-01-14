#ifndef HNSW_TESTS_H
#define HNSW_TESTS_H
#include "vtraceCommon.h"
#include "vtrace.h"
#include "heap.h"

// utils
Graph* mockGraphOneLayer(uint32 efSearch);

extern void INSERT_POP_MAXHEAP_TEST();
extern void INSERT_POP_MINHEAP_TEST();
extern void HEAP_TESTS();

extern void SIMPLE_SEARCH_LAYERTEST();
extern void INIT_GRAPH_TEST();
extern void GRAPH_TESTS();
extern void GraphTESTS();
#endif
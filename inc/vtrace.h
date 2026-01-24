/**
 * @file vtrace.h
 * @author Leon Margale (leon@margale.de)
 * @brief 
 * @version 0.1
 * @date 2025-12-11
 * 
 * @copyright Copyright (c) 2025
 * 
 * Vtrace is a vector search Libary, that implements the HNSW (similarity search in vector spaces using small hirachical Worlds) that gets developed for a bachlor thesis.
 * Vtrace tries to optimise the hnsw algorithm using the following techniques: Memory Layout structure, smid operations, L1 cache misses reduction.  
 */


#ifndef VTRACE_H
#define VTRACE_H

#include "vtraceCommon.h"
#include "distance.h"
#include "vec.h"
#include "graph.h"
#include "stdbool.h"

#define MAX_LEVEL 16

extern float VTsearch(Graph*, float vec);
extern void VTinsert(Graph*, float vec);

extern int VTlevelSample(uint32,float32);

// ALGORITHMS
Heap *SEARCH_LAYER(Graph *graph, vec q,uint32 ef, uint32 lc);
Heap* SELECT_NEIGBOURS_SIMPLE(Heap *c, uint32 M);
void INSERT(Graph* graph,vec vec,uint32 M, uint32 Mmax, uint32 efConstruction, uint32 ml);

// public API
typedef struct Graph HNSW;
HNSW* hnsw_init(int max_elements, int M,  uint32 efConstruction, float32 level_multiplyer);
void hnsw_insert(HNSW* graph, int id, float* vector);
void hnsw_search(HNSW* graph, float* query, int k, int efSearch, int* results);
void hnsw_free(HNSW* graph);
#endif
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
#include "heap.h"

struct vector {
  float* vec;
  uint32 dim;
};

typedef struct vector vec;

typedef struct {
  uint64 id;
  uint32 maxLevel;
  uint32** neigbours;
  void* data;
} node;
typedef node VT_node;

 struct Graph {
  uint32 efsearch;
  uint32 efconstruction;
  uint32 maxLayer;
  uint64 count;
  node* nodes;

  Heap* maxHeap, minHeap;
  
  node* entrypoint;
  
};

typedef struct Graph Graph;

typedef Graph VT_graph;

extern VT_graph* initializeGraph(uint32, uint32, uint32);
extern void uninitializeGraph(VT_graph* graph);


extern VT_graph VTcreateGraph(float* data, uint32 size);
extern float VTsearch(Graph*, float vec);
extern void VTinsert(Graph*, float vec);
extern int VTlevelSample(uint32,float);

// ALGORITHMS
node* SEARCH_LAYER(vec v, node* ep, uint32 ef, uint32 lc);


// public API
typedef struct Graph HNSW;

HNSW* hnsw_init(int max_elements, int M,  uint32 efConstruction, float32 level_multiplyer);
void hnsw_insert(HNSW* graph, int id, float* vector);
void hnsw_search(HNSW* graph, float* query, int k, int efSearch, int* results);
void hnsw_free(HNSW* graph);

#endif
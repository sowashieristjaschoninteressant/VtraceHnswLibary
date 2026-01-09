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
#include "distance.h"

#define START_LAYER_NODES 10000
#define DEFAULT_MAX_NEIGBOURS 64

struct vector {
  float* vec;
  uint32 dim;
};

typedef struct vector vec;

typedef struct {
  uint64 id;
  uint32 maxLevel;
  uint32** neigbours;
  uint32* numNeigbours;

  void* data;
} node;

struct hnsw_visitedList{
  uint32* visited;
  uint32 visited_mark;
  uint32 size;
};

typedef struct hnsw_visitedList visitedList;

visitedList initvList(uint32 size);

typedef node VT_node;

 struct Graph {

  // hyperparameters
  uint32 efsearch;
  uint32 efconstruction;
  uint32 M_maxNeigbours;
  uint32 maxLayer;
  uint64 count;

  visitedList visited;
  node* nodes;
  Heap* maxHeap, minHeap;
  node* entrypoint;
};

typedef struct Graph Graph;

typedef Graph VT_graph;

VT_graph *initializeGraph(uint32 maxLayer, uint32 efConstruction, uint32 efSearch, uint32 M_maxNeigbours);
extern void uninitializeGraph(VT_graph* graph);

extern VT_graph VTcreateGraph(float* data, uint32 size);
extern float VTsearch(Graph*, float vec);
extern void VTinsert(Graph*, float vec);

extern void VTaddNeigbour(node* target, uint32 neighbourId, uint32 layer, uint32 M_MAXneigbours);
extern int VTlevelSample(uint32,float32);

// ALGORITHMS
Heap* SEARCH_LAYER( Graph* graph ,vec q, uint32 lc);

// public API
typedef struct Graph HNSW;

HNSW* hnsw_init(int max_elements, int M,  uint32 efConstruction, float32 level_multiplyer);
void hnsw_insert(HNSW* graph, int id, float* vector);
void hnsw_search(HNSW* graph, float* query, int k, int efSearch, int* results);
void hnsw_free(HNSW* graph);
#endif
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

typedef struct {
  uint64 id;
  uint32 maxLevel;
  uint32** neigbours;
  void* data;
} node;

typedef node VT_node;

typedef struct {
  uint32 efsearch;
  uint32 efconstruction;
  uint32 maxLayer;
  uint64 count;
  node* nodes;

  node* entrypoint;
  
} Graph;

typedef Graph VT_graph;

extern VT_graph* initializeGraph(uint32, uint32, uint32);
extern void uninitializeGraph(VT_graph* graph);

extern VT_graph VTcreateGraph(float* data, uint32 size);
extern float VTsearch(Graph*, float vec);
extern void VTinsert(Graph*, float vec);


extern int VTlevelSample(uint32,float);

#endif
#ifndef VTRACE_INTERNAL_H
#define VTRACE_INTERNAL_H
#include "vtraceCommon.h"


#include "distance.h"
#include "vec.h"
#include "graph.h"


#define MAX_LEVEL 16


extern int VTlevelSample(uint32,float32);

// ALGORITHMS
Heap* SEARCH_LAYER(Graph *graph,hnswNode* entryPoint, vec q,uint32 ef, uint32 lc);
Heap* SELECT_NEIGBOURS_SIMPLE(Heap *c, uint32 M);
void  INSERT(Graph* graph,vec vec,uint32 M, uint32 Mmax, uint32 efConstruction, uint32 ml);
Heap* SELECT_NEIGBOURS_HEURISTIC(Graph* graph);
Heap* SEARCH(Graph* g, vec q);
#endif
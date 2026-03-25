#ifndef VTRACE_INTERNAL_H
#define VTRACE_INTERNAL_H
#include "vtraceCommon.h"


#include "distance.h"
#include "vec.h"
#include "graph.h"


#define MAX_LEVEL 16

extern int VTlevelSample(uint32,float32);
// ALGORITHMS
sortedBuffer* SEARCH_LAYER(hnswContext *ctx, hnswNode *entryPoint, vec q, uint32 ef, int32 lc);
Heap* SELECT_NEIGBOURS_SIMPLE( sortedBuffer *c, uint32 M, Heap* out);
sortedBuffer *SELECT_NEIGBOURS_HEURISTIC(hnswContext *ctx, hnswNode *baseElement, sortedBuffer *candidates,sortedBuffer* out, int32 M);
void  INSERT(Graph* graph,vec vec,int32 M, uint32 efConstruction, uint32 ml);
void K_NN_SEARCH(hnswContext* ctx, vec q,int32 K, int32 efsearch, Heap* out);
void NN_SIMPLE_LINEAR(Graph *g, vec *q, int K, int *out_ids);

// helper functions
 void connect_bidirectional(hnswContext* ctx, hnswNode *a, hnswNode *b, int32 layer);
 void prune_neighbours(hnswContext* ctx, hnswNode *node, int32 layer, int32 max);
 void flush_dirty_until_stable(hnswContext *ctx, Graph *g);

#endif
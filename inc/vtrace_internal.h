#ifndef VTRACE_INTERNAL_H
#define VTRACE_INTERNAL_H
#include "vtraceCommon.h"


#include "distance.h"
#include "vec.h"
#include "graph.h"


#define MAX_LEVEL 16

#define EPSILON 1e-6f


enum SELECT_NEIG_HEURISTIC_FLAGS {
    EXTENDCANDIDATES = 0,
    KEEP_P_CONN

};

#define HAS_FLAG(flags, bit) ((flags) & ( 1 << bit))
#define SET_EXTENDCANDIDATES(flags) ((flags) | 1 << EXTENDCANDIDATES)
#define SET_KEEP_P_CONN(flags) ((flags) | 1 << KEEP_P_CONN)



extern int VTlevelSample(uint32,float32);
// ALGORITHMS
Heap* SEARCH_LAYER(Graph *graph,hnswNode* entryPoint, vec q,uint32 ef, uint32 lc);
Heap* SELECT_NEIGBOURS_SIMPLE(Heap *c, uint32 M);
Heap* SELECT_NEIGBOURS_HEURISTIC(Graph* graph,hnswNode* baseElement,Heap* workingQueue,int32 lc,int32 M,int8 FLAGS );
void  INSERT(Graph* graph,vec vec,uint32 M, uint32 Mmax, uint32 efConstruction, uint32 ml);
Heap* K_NN_SEARCH(Graph* g, vec q,int32 K, int32 efsearch);


#endif
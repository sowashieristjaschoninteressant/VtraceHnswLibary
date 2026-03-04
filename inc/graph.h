#ifndef HNSW_GRAPH_H
#define HNSW_GRAPH_H
#include "vtraceCommon.h"
#include "alloc.h"
#include "heap.h"
#include "vec.h"

#define START_LAYER_NODES 10000
#define DEFAULT_MAX_NEIGBOURS 64


struct graph_internal_Storage{
   Heap* candidateHeap,*discardedHeap,*secondResultHeap,*resultHeap, *closestResults, *simpleHeap;
   sortedBuffer* buffer,*pruneBuffer;
};

typedef struct graph_internal_Storage graphInternalStorage;

typedef struct {
  uint64 id;
  uint32 level;
  uint32* neigbours;
  uint32* numNeigbours;

  vec v;
} node;

typedef node hnswNode;

struct hnsw_visitedList{
  uint32* visited;
  uint32 visited_mark;
  uint32 size;
};

typedef struct hnsw_visitedList visitedList;

visitedList initvList(uint32 size);

 struct Graph {

  // hyperparameters
  int32 efsearch;
  int32 efconstruction;
  int32 M_maxNeigbours;
  int32 Mmax0;
  int32 maxLayer;
  int64 count;
  int64 maxNodeCount;


  int64 entrypointID;
  visitedList visited;
  node* nodes;
  graphInternalStorage storage;

  
};


typedef struct Graph Graph;

typedef Graph VT_graph;
extern void makeNode(node *node, vec v, uint32 id, uint32 nodeLevel, uint32 maxNeigbours, int32 mMax0);
extern VT_graph *initializeGraph(uint32 maxLayer, uint32 efConstruction, uint32 efSearch, uint32 M_maxNeigbours, uint32 maxNodeCount);
extern void uninitializeGraph(VT_graph* graph);
extern void addNeigbour(node* target, uint32 neighbourId, uint32 layer, uint32 M_MAXneigbours);
extern void expandgraph(Graph* graph);
void initStorage(Graph* graph);
HNSW_INLINE node* getNodeById(Graph* g, uint32 id){
  return &g->nodes[id];
}

HNSW_INLINE void incVisitedMark(Graph* g){
     g->visited.visited_mark++;
    
}
HNSW_INLINE void markNodeVisited(Graph* g, uint64 id){
      if(!g->visited.visited || id > g->visited.size){
          printf("id index: %lu\n", id);
          HNSW_LOG("visit list needs to grow");
          abort();
        }
      g->visited.visited[id] = g->visited.visited_mark;
}
#endif
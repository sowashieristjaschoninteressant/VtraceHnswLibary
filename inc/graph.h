#ifndef HNSW_GRAPH_H
#define HNSW_GRAPH_H

#include "vtraceCommon.h"
#include "heap.h"

#define START_LAYER_NODES 10000
#define DEFAULT_MAX_NEIGBOURS 64

typedef struct {
  uint64 id;
  uint32 maxLevel;
  uint32** neigbours;
  uint32* numNeigbours;

  void* data;
} node;

typedef node VT_node;

struct hnsw_visitedList{
  uint32* visited;
  uint32 visited_mark;
  uint32 size;
};

typedef struct hnsw_visitedList visitedList;

visitedList initvList(uint32 size);

 struct Graph {

  // hyperparameters
  uint32 efsearch;
  uint32 efconstruction;
  uint32 M_maxNeigbours;
  uint32 maxLayer;
  uint64 count;

  visitedList visited;
  node* nodes;
  Heap* maxHeap,* minHeap;
  node* entrypoint;
};

typedef struct Graph Graph;

typedef Graph VT_graph;

extern VT_graph *initializeGraph(uint32 maxLayer, uint32 efConstruction, uint32 efSearch, uint32 M_maxNeigbours);
extern void uninitializeGraph(VT_graph* graph);
extern void VTaddNeigbour(node* target, uint32 neighbourId, uint32 layer, uint32 M_MAXneigbours);

HNSW_INLINE void incVisitedMark(Graph* g){
     g->visited.visited_mark++;
    
}
HNSW_INLINE void markNodeVisited(Graph* g, uint64 id){
      g->visited.visited[id] = g->visited.visited_mark;
}
#endif
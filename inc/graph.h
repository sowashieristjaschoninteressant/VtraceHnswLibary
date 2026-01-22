#ifndef HNSW_GRAPH_H
#define HNSW_GRAPH_H

#include "alloc.h"
#include "heap.h"
#include "vec.h"

#define START_LAYER_NODES 10000
#define DEFAULT_MAX_NEIGBOURS 64

typedef struct {
  uint64 id;
  uint32 level;
  uint32** neigbours;
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
  uint32 efsearch;
  uint32 efconstruction;
  uint32 M_maxNeigbours;
  uint32 maxLayer;
  uint64 count;
  uint64 maxNodeCount;

  visitedList visited;
  node* nodes;
  Heap* maxHeap,* minHeap;
  node* entrypoint;
};

typedef struct Graph Graph;

typedef Graph VT_graph;
extern void makeNode(node* node, vec v,uint32 id, uint32 nodeLevel, uint32 maxNeigbours);
extern VT_graph *initializeGraph(uint32 maxLayer, uint32 efConstruction, uint32 efSearch, uint32 M_maxNeigbours, uint32 maxNodeCount);
extern void uninitializeGraph(VT_graph* graph);
extern void VTaddNeigbour(node* target, uint32 neighbourId, uint32 layer, uint32 M_MAXneigbours);


HNSW_INLINE node* getNode(Graph* g, uint32 id){
  return &g->nodes[id];
}

HNSW_INLINE void incVisitedMark(Graph* g){
     g->visited.visited_mark++;
    
}
HNSW_INLINE void markNodeVisited(Graph* g, uint64 id){
      g->visited.visited[id] = g->visited.visited_mark;
}
#endif
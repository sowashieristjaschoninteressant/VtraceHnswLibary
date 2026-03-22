#ifndef HNSW_GRAPH_H
#define HNSW_GRAPH_H
#include "vtraceCommon.h"
#include "alloc.h"
#include "heap.h"
#include "vec.h"

#define START_LAYER_NODES 10000
#define DEFAULT_MAX_NEIGBOURS 64

typedef struct
{
  int64 id;
  int32 level;
  int64 *neigbours;
  int32 *numNeigbours;
  vec v;
} node;

typedef node hnswNode;

struct hnsw_visitedList
{
  uint32 *visited;
  uint32 visited_mark;
  size_t size;
};

typedef struct hnsw_visitedList visitedList;
typedef struct hnswContext hnswContext;
typedef struct Graph Graph;
typedef struct hnswContextPool hnswContextPool;

visitedList initvList(size_t size);
void resizeVisited(hnswContext *ctx, uint32 newSize);

struct hnswContext
{
  Graph *g;
  Heap *candidateHeap;
  Heap *resultHeap;
  Heap *secondResultHeap;
  Heap *discardHeap;
  Heap *outHeap;

  sortedBuffer *buffer;
  sortedBuffer *tempbuf;
  sortedBuffer *pruneBuffer;
  visitedList visited;
};

/**
 * A context is NEVER used by more than one operation at a time
  A context is ALWAYS reset before reuse
  A context is ALWAYS released after use
 *
 */
struct hnswContextPool
{
  hnswContext *pool;
  int32 size;
  int32 capacity;
  int8 *inUse;
};

struct Graph
{
  // hyperparameters
  int32 efsearch;
  int32 efconstruction;
  int32 M_maxNeigbours;
  int32 Mmax0;
  int32 maxLayer;
  int64 count;
  int64 maxNodeCount;
  int64 maxHeapSize;

  int64 entrypointID;
  node *nodes;
  hnswContextPool pool;
};

extern hnswContext *createHnswContext(Graph *g);
extern hnswContext *destroyHnswContext(hnswContext *ctx);
extern hnswContext *acquireContext(Graph *g);
extern void releaseContext(Graph *g, hnswContext *ctx);
extern void initContextPool(Graph *g, int32 capacity);

typedef Graph VT_graph;
extern void makeNode(node *node, vec v, int64 id, int32 nodeLevel, uint32 maxNeigbours, int32 mMax0);
extern VT_graph *initializeGraph(uint32 maxLayer, uint32 efConstruction, uint32 efSearch, uint32 M_maxNeigbours, uint32 maxNodeCount);
extern void uninitializeGraph(VT_graph *graph);
extern void addNeigbour(node *target, uint32 neighbourId, uint32 layer, uint32 M_MAXneigbours);
extern void expandgraph(Graph *graph);


HNSW_INLINE uint32 layer_offset(Graph *g, uint32 layer)
{
    if (layer == 0)
        return 0;

    return (g->Mmax0 * 2) + (layer - 1) * (g->M_maxNeigbours * 2);
}

HNSW_INLINE int hasNeigbour(hnswNode *node, int32 id, int32 layer, int32 off)
{

  for (int32 i = 0; i < node->numNeigbours[layer]; i++)
  {

    if (node->neigbours[off + i] == id)
    {
      return 1;
    }
  }
  return 0;
}

HNSW_INLINE node *getNodeById(Graph *g, uint32 id)
{
  return &g->nodes[id];
}

HNSW_INLINE void incVisitedMark(hnswContext *ctx)
{

  ctx->visited.visited_mark++;
  if (ctx->visited.visited_mark == 0)
  {
    memset(ctx->visited.visited, 0,
    sizeof(uint32) * ctx->visited.size);
    ctx->visited.visited_mark = 1;
  }
}

HNSW_INLINE void markNodeVisited(hnswContext* ctx, uint64 id)
{
  if (!ctx->visited.visited || id >= ctx->visited.size)
  {
    printf("id index: %lu, visited size as of right now: %llu\n", id, ctx->visited.size);
    HNSW_LOG("visit list needs to grow");
    abort();
  }
  ctx->visited.visited[id] = ctx->visited.visited_mark;
}

HNSW_INLINE void resetContext(hnswContext *ctx)
{

  heap_reset(ctx->resultHeap);
  heap_reset(ctx->candidateHeap);
  heap_reset(ctx->secondResultHeap);
  heap_reset(ctx->discardHeap);

  if (ctx->buffer)
    ctx->buffer->size = 0;
  if (ctx->tempbuf)
    ctx->tempbuf->size = 0;
  if (ctx->pruneBuffer)
    ctx->pruneBuffer->size = 0;

  incVisitedMark(ctx); // IMPORTANT (see below)
}
#endif
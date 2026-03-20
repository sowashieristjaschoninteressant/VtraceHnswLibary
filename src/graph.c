
#include "graph.h"

Graph *initializeGraph(uint32 maxLayer, uint32 efConstruction, uint32 efSearch, uint32 M_maxNeigbours, uint32 maxNodeCount)
{

    Graph *graph = malloc(sizeof(VT_graph));

    HNSW_ASSERT(graph);

    // seed random algorithm
    srand((int)time(NULL));
    graph->M_maxNeigbours = M_maxNeigbours <= 0 ? DEFAULT_MAX_NEIGBOURS : M_maxNeigbours;

    graph->Mmax0 = graph->M_maxNeigbours * 2;

    graph->efconstruction = efConstruction;
    graph->efsearch = efSearch;
    graph->maxLayer = maxLayer;
    graph->maxNodeCount = maxNodeCount;
    graph->maxHeapSize = efConstruction * (efConstruction / 2);
    graph->nodes = malloc(sizeof(hnswNode) * graph->maxNodeCount);
    if (!graph->nodes)
    {
        HNSW_LOG("cannot allocate graph nodes out of mem?\n");
        abort();
    }
    graph->count = 0;

    graph->entrypointID = -1;

    initContextPool(graph, 5);

    return graph;
}

void makeNode(node *node, vec v, uint32 id, uint32 nodeLevel, uint32 maxNeigbours, int32 mMax0)
{
    // because layer 1 will be saved in array slot 0 but when i pass 0 into the allocation func it will abort

    node->id = id;
    node->level = nodeLevel;

    node->v.dim = v.dim;
    node->v.vec = hnsw_alloc_mem(sizeof(float32) * v.dim, alignof(float32));
    memcpy(node->v.vec, v.vec, sizeof(float32) * v.dim);

    uint32 allocationLevel = nodeLevel + 1;
    uint32 capLayer0 = mMax0 * 2;       // layer0 overprovision +1
    uint32 capUpper = maxNeigbours * 2; // layer>0 overprovision +1

    uint32 totalSlots = capLayer0 + capUpper * nodeLevel;

    node->neigbours = hnsw_alloc_mem(sizeof(uint32) * totalSlots, alignof(uint32));
    node->numNeigbours = hnsw_alloc_mem(sizeof(uint32) * allocationLevel, alignof(uint32));

    node->dirty = hnsw_alloc_mem(sizeof(int8) * allocationLevel, alignof(int8));
    
    memset(node->numNeigbours, 0, sizeof(uint32) * allocationLevel);
}

hnswContext *acquireContext(Graph *g)
{
    hnswContextPool *pool = &g->pool;

    for (int32 i = 0; i < pool->size; i++)
    {

        if (!pool->inUse[i])
        {
            pool->inUse[i] = 1;

            hnswContext *context = &pool->pool[i];
            context->g = g;

            resetContext(context);

            return context;
        }
    }

    HNSW_LOG("no available hnsw contexts");
    abort();
}

void releaseContext(Graph *g, hnswContext *ctx)
{
    hnswContextPool *p = &g->pool;

    int32 index = (int32)(ctx - p->pool);

    if (index < 0 || index >= p->capacity)
    {
        HNSW_LOG("invalid context release");
        abort();
    }

    p->inUse[index] = 0;
}

void initContextPool(Graph *g, int32 capacity)
{

    g->pool.capacity = capacity;
    g->pool.size = capacity;

    g->pool.pool = calloc(capacity, sizeof(hnswContext));
    g->pool.inUse = calloc(capacity, sizeof(uint8));

    for (int i = 0; i < capacity; i++)
    {
        hnswContext *ctx = &g->pool.pool[i];

        ctx->candidateHeap = MIN_HEAP(g->maxHeapSize);
        ctx->resultHeap = MAX_HEAP(g->maxHeapSize);
        ctx->secondResultHeap = MAX_HEAP(g->maxHeapSize);
        ctx->discardHeap = MIN_HEAP(g->maxHeapSize);
        ctx->outHeap = MIN_HEAP(g->maxHeapSize);
        ctx->visited = initvList(g->maxNodeCount);
        ctx->buffer = initSortedBuffer(g->maxHeapSize);
        ctx->tempbuf = initSortedBuffer(g->maxHeapSize);
        ctx->pruneBuffer = initSortedBuffer(g->maxHeapSize);

        ctx->dirtyNodes = initDirtyBuffer(g->maxHeapSize);
        
    }
}

void resizeVisited(hnswContext *ctx, uint32 newSize)
{
    uint32* newPtr = realloc(ctx->visited.visited,
                                   sizeof(uint32) * newSize);

    if (!newPtr)
    {
        HNSW_LOG("visited realloc failed");
        abort();
    }

    ctx->visited.visited = newPtr;

   if (newSize > ctx->visited.size) {
    memset(ctx->visited.visited + ctx->visited.size,
           0,
           sizeof(uint32) * (newSize - ctx->visited.size));
    }   

    ctx->visited.size = newSize;
}

void expandgraph(Graph *graph)
{
    uint32 newMaxNodeCount = graph->maxNodeCount * 2;

    hnswNode *nodes = realloc(graph->nodes, sizeof(hnswNode) * newMaxNodeCount);

    hnswContextPool pool = graph->pool;

    for (int32 i = 0; i < pool.size; i++)
    {
        resizeVisited(&graph->pool.pool[i], newMaxNodeCount);
    }

    if (!nodes)
    {
        HNSW_LOG("cannot allocate more nodes out of memory? expandGraph\n");
        abort();
    }

    graph->nodes = nodes;
    graph->maxNodeCount = newMaxNodeCount;
}

visitedList initvList(size_t size)
{
    visitedList list;

    list.size = size;
    list.visited_mark = 1;
    list.visited = malloc(sizeof(uint32) * size);

    if (!list.visited)
    {
        HNSW_LOG("error cannot allocate the visited List");
        abort();
    }
    
    memset(list.visited, 0, sizeof(uint32) * size);
    
   

    return list;
}

void uninitializeGraph(VT_graph *graph)
{

    free(graph->nodes);
    
    free(graph);
}

inline void addNeigbour(node *target, uint32 neighbourId, uint32 layer, uint32 offset)
{

    target->neigbours[offset + target->numNeigbours[layer]++] = neighbourId;
}

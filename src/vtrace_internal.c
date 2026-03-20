#include "vtrace_internal.h"
#include "tests.h"

__attribute__((constructor)) static void vtrace_init(void)
{
    printf("okay constructor gets called!\n");
    // instaniate

    GRAPH_TESTS();
    HEAP_TESTS();
    ARENA_TESTS();
}

__attribute__((destructor)) static void vtrace_cleanup(void)
{
    printf("vtrace destructor...");

    destroy_global_chainArena();
}

/**
 * @brief
 * level sampler to create a geometric distribution between our graph layer sparse -> dense
 * @param lMax
 * @param level_mult
 * @return int
 */
inline int32 VTlevelSample(uint32 lMax, float32 level_mult)
{

    float u = (float)rand() / ((float)RAND_MAX + 1.0f);

    if (u == 0.0f)
        u = 1e-10f;
    uint32 level = (int)(-logf(u) * level_mult);

    return level > lMax ? lMax : level;
}

void prune_neighbours(hnswContext *ctx, hnswNode *node, int32 layer, int32 max)
{
    Graph *g = ctx->g;
    int32 off = layer_offset(g, layer);
    int32 maxCapacity = max * 2;
    sortedBuffer *buf = ctx->pruneBuffer;
    sortedBuffer *out = ctx->tempbuf;

    // am Anfang von prune_neighbours
    if (node->id == 2 && layer == 0)
    {
        printf("PRUNING node=2 layer=0 count=%d\n", node->numNeigbours[layer]);
    }

    buffer_reset(buf);
    buffer_reset(out);

    int32 oldCount = node->numNeigbours[layer];

    if (oldCount > maxCapacity)
    {
        printf("OLDCOUNT CORRUPT: node=%d layer=%d oldCount=%d maxCapacity=%d\n",
               node->id, layer, oldCount, maxCapacity);
        abort();
    }

    // -------------------------------------------------
    // 1. Collect all current neighbors as candidates
    // -------------------------------------------------
    for (int32 i = 0; i < oldCount; i++)
    {
        int32 nid = node->neigbours[off + i];
        hnswNode *n = getNodeById(g, nid);

        heapItem item;
        item.id = nid;
        item.dist = l2_sq_distance_neon_128v(&node->v, &n->v);

        push_buffer(buf, &item);
    }

    // -------------------------------------------------
    // 2. Select best neighbors (HNSW heuristic)
    // -------------------------------------------------
    sortedBuffer *selected =
        SELECT_NEIGBOURS_HEURISTIC(ctx, node, buf, out, max);

    if (selected->size > max)
        selected->size = max;

    // -------------------------------------------------
    // 3. CLEAN REBUILD
    // -------------------------------------------------

    for (int32 i = 0; i < oldCount; i++)
    {
        int32 oldID = node->neigbours[off + i];
        hnswNode *other = getNodeById(g, oldID);

        int32 off2 = layer_offset(g, layer);

        for (int32 k = 0; k < other->numNeigbours[layer];)
        {
            if (other->neigbours[off2 + k] == node->id)
            {
                other->neigbours[off2 + k] =
                    other->neigbours[off2 + (--other->numNeigbours[layer])];
            }
            else
            {
                k++;
            }
        }
    }

    // (B) reset node adjacency
    node->numNeigbours[layer] = 0;

    // (C) write new forward edges
    for (int i = 0; i < selected->size; i++)
    {
        int32 nid = selected->data[i].id;

        node->neigbours[off + node->numNeigbours[layer]++] = nid;

        if (node->numNeigbours[layer] > max)
        {
            HNSW_LOG("OKAY FOUND THE DIRTY LITTLE BASTARD");
            abort();
        }
    }

    // -------------------------------------------------
    // 4. rebuild reverse edges
    // -------------------------------------------------
    for (int i = 0; i < selected->size; i++)
    {
        hnswNode *other = getNodeById(g, selected->data[i].id);
        int32 off2 = layer_offset(g, layer);

        // check existence
        int exists = 0;
        for (int j = 0; j < other->numNeigbours[layer]; j++)
        {
            if (other->neigbours[off2 + j] == node->id)
            {
                exists = 1;
                break;
            }
        }

        if (!exists)
        {
            if (other->numNeigbours[layer] < max)
            {
                other->neigbours[off2 + other->numNeigbours[layer]++] = node->id;
               
            }
           
        }
    }

    if (node->id == 2 && layer == 0)
    {
        printf("AFTER PRUNE node=2 layer=0 count=%d\n", node->numNeigbours[layer]);
    }

    // am Ende von prune_neighbours, nach Schritt 4
    for (int32 i = 0; i < node->numNeigbours[layer]; i++)
    {
        if (node->numNeigbours[layer] > max)
        {
            printf("PRUNE FAILED: node=%d layer=%d count=%d max=%d\n",
                   node->id, layer, node->numNeigbours[layer], max);
            abort();
        }
    }

    // und check alle reverse-edge empfänger
    for (int32 i = 0; i < selected->size; i++)
    {
        hnswNode *other = getNodeById(g, selected->data[i].id);
        int32 otherMax = layer == 0 ? g->Mmax0 : g->M_maxNeigbours;
        if (other->numNeigbours[layer] > otherMax)
        {
            printf("REVERSE EDGE OVERFLOW: node=%d layer=%d count=%d max=%d\n",
                   other->id, layer, other->numNeigbours[layer], otherMax);
            abort();
        }
    }
}


 void connect_bidirectional(hnswContext *ctx, hnswNode *a, hnswNode *b, int32 layer)
{
    Graph *g = ctx->g;
    int32 maxNeigbours = layer == 0 ? g->Mmax0 : g->M_maxNeigbours;
    int32 off = layer_offset(g, layer);

    if (a->numNeigbours[layer] >= maxNeigbours)
        prune_neighbours(ctx, a, layer, maxNeigbours);

    if (b->numNeigbours[layer] >= maxNeigbours)
        prune_neighbours(ctx, b, layer, maxNeigbours);

    if (a->numNeigbours[layer] < maxNeigbours)
        a->neigbours[off + a->numNeigbours[layer]++] = b->id;

    if (b->numNeigbours[layer] < maxNeigbours)
        b->neigbours[off + b->numNeigbours[layer]++] = a->id;
}


void INSERT(Graph *graph, vec vec, int32 M, uint32 Mmax, uint32 efConstruction, uint32 ml)
{
    hnswContext *ctx = acquireContext(graph);

    hnswNode *originalEP, *ep, *newNode;
    int32 nodeLevel = VTlevelSample(MAX_LEVEL, ml);
    uint32 id = graph->count;

    if (graph->maxNodeCount <= id)
    {
        HNSW_LOG("graph is expanding!");
        expandgraph(graph);
    }

    newNode = getNodeById(graph, id);

    makeNode(newNode, vec, id, nodeLevel, graph->M_maxNeigbours, graph->Mmax0);

    if (graph->entrypointID < 0)
    {
        graph->entrypointID = newNode->id;
        graph->maxLayer = newNode->level;
        graph->count++;
        releaseContext(graph, ctx);
        return;
    }

    originalEP = ep = getNodeById(graph, graph->entrypointID);
    //------------------------
    // 1. SEARCH-PHASE
    //------------------------
    sortedBuffer *W;
    for (int32 j = graph->maxLayer; j > nodeLevel; j--)
    {
        W = SEARCH_LAYER(ctx, ep, vec, 1, j);
        ep = getNodeById(graph, W->data[0].id);
    }

    DirtyBuffer *dirtyNodes = ctx->dirtyNodes;

    //-----------------------
    // 2. INSERT PER LAYER
    //----------------------
   for (int32 layer = MIN(ep->level, nodeLevel); layer >= 0; layer--)
{
    sortedBuffer *resultBuffer = SEARCH_LAYER(ctx, ep, vec, efConstruction, layer);
    sortedBuffer *selected = ctx->tempbuf;
    buffer_reset(selected);
    SELECT_NEIGBOURS_HEURISTIC(ctx, newNode, resultBuffer, selected, M);

    int32 maxNeigbours = layer == 0 ? graph->Mmax0 : graph->M_maxNeigbours;

    for (int i = 0; i < selected->size; i++)
    {
        hnswNode *neighbor = getNodeById(graph, selected->data[i].id);

        if (layer <= newNode->level && layer <= neighbor->level)
            connect_bidirectional(ctx, newNode, neighbor, layer);

       
    }

    
}
   
    
    if (newNode->level > originalEP->level)
    {
        graph->entrypointID = newNode->id;
        graph->maxLayer = newNode->level;
    }

    graph->count++;

    releaseContext(graph, ctx);
}

Heap *SELECT_NEIGBOURS_SIMPLE(hnswContext *ctx, sortedBuffer *c, uint32 M, Heap *out)
{

    for (int32 i = 0; i < c->size && out->size < M; i++)
    {

        heap_insert(out, c->data[i].id, c->data[i].dist, NULL);
    }
    return out;
}

sortedBuffer *SEARCH_LAYER(hnswContext *ctx, hnswNode *entryPoint, vec q, uint32 ef, uint32 lc)
{
    const int off = layer_offset(ctx->g, lc);
    Heap *c = ctx->candidateHeap; // min heap candidate list
    Heap *w = ctx->resultHeap;    // closest results

    sortedBuffer *buffer = ctx->buffer;

    heap_reset(c);
    heap_reset(w);

    incVisitedMark(ctx);

    markNodeVisited(ctx, entryPoint->id);

    float32 epDistance = l2_sq_distance_neon_128v(&entryPoint->v, &q);

    heap_insert(c, entryPoint->id, epDistance, NULL);
    heap_insert(w, entryPoint->id, epDistance, NULL);
    uint32 *visited = ctx->visited.visited;

    while (c->size > 0)
    {
        heapItem current = heapPop(c);

        if (w->size >= ef && current.dist > heapPeek(w).dist)
        {
            //  HNSW_LOG("all elements are evaluated in searchLayer");

            break;
        }

        node *currentNode = getNodeById(ctx->g, current.id);

        if (currentNode->level < lc)
            continue;

        uint32 nabourCount = currentNode->numNeigbours[lc];

        for (uint32 i = 0; i < nabourCount; i++)
        {
            node *neigbour = getNodeById(ctx->g, currentNode->neigbours[off + i]);

            if (ctx->visited.visited[neigbour->id] != ctx->visited.visited_mark)
            {

                markNodeVisited(ctx, neigbour->id);

                float32 dist = l2_sq_distance_neon_128v(&neigbour->v, &q);

                if (dist < heapPeek(w).dist || w->size < ef)
                {
                    heap_insert(c, neigbour->id, dist, NULL);
                    heap_insert(w, neigbour->id, dist, NULL);
                }

                if (w->size > ef)
                {
                    heapPop(w);
                }
            }
        }
    }

    heap_drain_to_sorted_buffer(w, buffer);

    return buffer;
}

sortedBuffer *SELECT_NEIGBOURS_HEURISTIC(hnswContext *ctx, hnswNode *baseElement, sortedBuffer *candidates, sortedBuffer *out, int32 M)
{
    Graph *g = ctx->g;
    sortedBuffer *result = out;

    for (int i = 0; i < candidates->size; i++)
    {
        heapItem c = candidates->data[i];
        hnswNode *cand = getNodeById(g, c.id);
        float d_cd = l2_sq_distance_neon_128v(&baseElement->v, &cand->v);

        int8 good = 1;
        for (int j = 0; j < result->size; j++)
        {
            hnswNode *r = getNodeById(g, result->data[j].id);
            float d_cr = l2_sq_distance_neon_128v(&cand->v, &r->v);

            // heuristic condition
            if (d_cr < d_cd)
            {
                good = 0;
                break;
            }
        }

        if (good)
        {
            push_buffer(result, &c);
            if (result->size == M)
            {
                break;
            }
        }
    }

    return result;
}

void K_NN_SEARCH(hnswContext *ctx, vec q, int32 K, int32 efsearch, Heap *out)
{
    Graph *g = ctx->g;

    hnswNode *entryPoint = getNodeById(g, g->entrypointID);
    sortedBuffer *buffer;

    for (int32 layer = g->maxLayer - 1; layer > 0; layer--)
    {
        buffer = SEARCH_LAYER(ctx, entryPoint, q, 1, layer);

        entryPoint = getNodeById(g, buffer->data[0].id);
    }

    buffer = SEARCH_LAYER(ctx, entryPoint, q, efsearch, 0);

    SELECT_NEIGBOURS_SIMPLE(ctx, buffer, K, out);
}

vec NN_SIMPLE_LINEAR(Graph *g, vec q)
{

    int id = 0;
    float32 bestdist = LDBL_MAX;

    for (int i = 0; i < g->count; i++)
    {

        hnswNode *current = &g->nodes[i];

        float32 currDist = l2_sq_distance_neon_128v(&q, &current->v);

        if (currDist < bestdist)
        {
            bestdist = currDist;
            id = current->id;
        }
    }

    return getNodeById(g, id)->v;
}
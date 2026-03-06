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

 void prune_neighbours(Graph *g, hnswNode *node, int32 layer, int32 max)
{
    int32 off = layer_offset(g, layer);
    sortedBuffer *buf = g->storage.pruneBuffer;
    sortedBuffer *newSet = g->storage.oldCandidatesBuf;
    newSet->size = 0;

    int32 oldCount = node->numNeigbours[layer];

    buf->size = oldCount;
    for (int32 i = 0; i < oldCount; i++) {
        int32 nid = node->neigbours[off + i];
        hnswNode *n = getNodeById(g, nid);
        buf->data[i].id = nid;
        buf->data[i].dist = l2_sq_distance_neon_128_unroll(&node->v, &n->v);
    }

    Heap *selected = SELECT_NEIGBOURS_HEURISTIC(g, node, buf, layer, max, 0);

    node->numNeigbours[layer] = 0;

    while (selected->size) {
        heapItem it = heapPop(selected);
        node->neigbours[off + node->numNeigbours[layer]++] = it.id;
        newSet->data[newSet->size++].id = it.id;
    }

    
    for (int32 i = 0; i < oldCount; i++) {
        int32 oldID = buf->data[i].id;
        int survived = 0;
        for (int32 j = 0; j < newSet->size; j++) {
            if (newSet->data[j].id == oldID) { survived = 1; break; }
        }
        if (!survived) {
            hnswNode *other = getNodeById(g, oldID);
            int32 off2 = layer_offset(g, layer);
            for (int32 k = 0; k < other->numNeigbours[layer]; k++) {
                if (other->neigbours[off2 + k] == node->id) {
                    other->neigbours[off2 + k] = other->neigbours[off2 + (--other->numNeigbours[layer])];
                    break;
                }
            }
        }
    }

    for (int32 i = 0; i < newSet->size; i++) {
        hnswNode *survived = getNodeById(g, newSet->data[i].id);
        int32 off2 = layer_offset(g, layer);

        int alreadyLinked = 0;
        for (int32 j = 0; j < survived->numNeigbours[layer]; j++) {
            if (survived->neigbours[off2 + j] == node->id) {
                alreadyLinked = 1;
                break;
            }
        }

        if (!alreadyLinked) {
            if (survived->numNeigbours[layer] < max) {
                survived->neigbours[off2 + survived->numNeigbours[layer]++] = node->id;
            } else if (layer == 0) {
                // optionally prune layer 0 neighbor to make room
                prune_neighbours(g, survived, layer, max);
                survived->neigbours[off2 + survived->numNeigbours[layer]++] = node->id;
            }
        }
    }
}

 void connect_bidirectional(Graph *g, hnswNode *a, hnswNode *b, int32 layer)
{
    int32 maxNeigbours = layer == 0 ? g->Mmax0 : g->M_maxNeigbours;

    int32 off = layer_offset(g, layer);

    if (!hasNeigbour(a, b->id, layer, off))
    {
        a->neigbours[off + a->numNeigbours[layer]++] = b->id;
    }

    if (!hasNeigbour(b, a->id, layer, off))
    {
        b->neigbours[off + b->numNeigbours[layer]++] = a->id;
    }

    if (a->numNeigbours[layer] > maxNeigbours)
    {
       prune_neighbours(g, a, layer, maxNeigbours);
    }

    if (b->numNeigbours[layer] > maxNeigbours)
    {
        prune_neighbours(g, b, layer, maxNeigbours);
    }
}
void INSERT(Graph *graph, vec vec, int32 M, uint32 Mmax, uint32 efConstruction, uint32 ml)
{

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

#ifdef BILLION_SEARCH_HNSW
    // this is a hot loop so lets just write it like this. this will never happen basically unless you save over 4Billion nodes in this structure but i belive no consumer hardware has even the memory capacity for smth like that
    if (graph->count == UINT32_MAX)
    {
        HNSW_LOG("node ID overflow!");
        abort();
    }
#endif

    if (graph->entrypointID < 0)
    {
        graph->entrypointID = newNode->id;
        graph->maxLayer = newNode->level;
        graph->count++;
        return;
    }

    originalEP = ep = getNodeById(graph, graph->entrypointID);
    sortedBuffer *W;

    for (int32 j = graph->maxLayer; j > nodeLevel; j--)
    {
        W = SEARCH_LAYER(graph, ep, vec, 1, j);
        ep = getNodeById(graph, heapPeek(W).id);
    }
  
    for (int32 layer = MIN(ep->level, nodeLevel); layer >= 0; layer--)
    {
        sortedBuffer *resultBuffer = SEARCH_LAYER(graph, ep, vec, efConstruction, layer);

        Heap *selected = SELECT_NEIGBOURS_HEURISTIC(graph, newNode, resultBuffer, layer, M, 0);
        
       

        while (selected->size > 0)
        {

            hnswNode *node = getNodeById(graph, heapPop(selected).id);

            if(layer <= newNode->level && layer <= node->level){
                 connect_bidirectional(graph,newNode,node,layer);
            }
           
        }
    }

    graph->count++;

    if (newNode->level > originalEP->level)
    {
        graph->entrypointID = newNode->id;
        graph->maxLayer = newNode->level;
    }
}

/*Algorithm 3 SELECT-NEIGHBORS-SIMPLE(q, C, M)
Input: base element q,
 candidate elements C, number of neighbors to return M
 Output: M nearest elements to q return M nearest elements from C to q*/
Heap *SELECT_NEIGBOURS_SIMPLE(Graph *g, sortedBuffer *c, uint32 M)
{
    Heap *m = g->storage.simpleHeap;
    heap_reset(m);

    for (int32 i = 0; i < c->size && m->size < M; i++)
    {

        heap_insert(m, c->data[i].id, c->data[i].dist, NULL);
    }
    return m;
}

/**
 Algorithm 2
SEARCH-LAYER(q, ep, ef, lc)
Input: query element q, enter points ep, number of nearest to q ele-
ments to return ef, layer number lc
Output: ef closest neighbors to q
1 v ← ep // set of visited elements
2 C ← ep // set of candidates
3 W ← ep // dynamic list of found nearest neighbors
4 while │C│ > 0
5 c ← extract nearest element from C to q
6 f ← get furthest element from W to q
7 if distance(c, q) > distance(f, q)
8 break // all elements in W are evaluated
9 for each e ∈ neighbourhood(c) at layer lc // update C and W
10 if e ∉ v
11 v ← v ⋃ e
12 f ← get furthest element from W to q
13 if distance(e, q) < distance(f, q) or │W│ < ef
14 C ← C ⋃ e
15 W ← W ⋃ e
16 if │W│ > ef
17 remove furthest element from W to q
18 return W
 */

// node* SEARCH_LAYER(vec v, node* ep, uint32 ef, uint32 lc)
sortedBuffer *SEARCH_LAYER(Graph *graph, hnswNode *entryPoint, vec q, uint32 ef, uint32 lc)
{

    Heap *c = graph->storage.candidateHeap;  // min heap candidate list
    Heap *w = graph->storage.closestResults; // closest results

    sortedBuffer *buffer = graph->storage.buffer;

    heap_reset(c);
    heap_reset(w);

    incVisitedMark(graph);

    if (graph->visited.visited_mark == 0)
    {
        memset(graph->visited.visited, 0, sizeof(uint32) * graph->visited.size);
    }

    markNodeVisited(graph, entryPoint->id);

    float32 epDistance = l2_sq_distance_neon_128v(&entryPoint->v, &q);

    heap_insert(c, entryPoint->id, epDistance, NULL);
    heap_insert(w, entryPoint->id, epDistance, NULL);

    while (c->size > 0)
    {
        heapItem current = heapPop(c);

        if (w->size >= ef && current.dist > heapPeek(w).dist)
        {
            //  HNSW_LOG("all elements are evaluated in searchLayer");

            break;
        }

        node *currentNode = &graph->nodes[current.id];

        if (lc > currentNode->level)
            continue;

        uint32 nabourCount = currentNode->numNeigbours[lc];

        for (uint32 i = 0; i < nabourCount; i++)
        {
            node *neigbour = getNodeById(graph, currentNode->neigbours[(lc * graph->M_maxNeigbours) + i]);

            if (graph->visited.visited[neigbour->id] != graph->visited.visited_mark)
            {

                markNodeVisited(graph, neigbour->id);

                float32 dist = l2_sq_distance_neon_128v(&neigbour->v, &q);

                if (dist + EPSILON < heapPeek(w).dist || w->size < ef)
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

    maxHeapToSortedAscending(w, buffer);

    return buffer;
}

/*
Algorithm 4
SELECT-NEIGHBORS-HEURISTIC(q, C, M, lc, extendCandidates, keep-
PrunedConnections)
Input: base element q, candidate elements C, number of neighbors to
return M, layer number lc, flag indicating whether or not to extend
candidate list extendCandidates, flag indicating whether or not to add
discarded elements keepPrunedConnections
Output: M elements selected by the heuristic
1 R ← ∅
2 W ← C // working queue for the candidates
3 if extendCandidates // extend candidates by their neighbors
4 for each e ∈ C
5 for each eadj ∈ neighbourhood(e) at layer lc
6 if eadj ∉ W
7 W ← W ⋃ eadj
8 Wd ← ∅ // queue for the discarded candidates
9 while │W│ > 0 and │R│< M
10 e ← extract nearest element from W to q
11 if e is closer to q compared to any element from R
12 R ← R ⋃ e
13 else
14 Wd ← Wd ⋃ e
15 if keepPrunedConnections // add some of the discarded
// connections from Wd
16 while │Wd│> 0 and │R│< M
17 R ← R ⋃ extract nearest element from Wd to q

// FLAGS
extend connections
keepPrunnedConnections
18 return R*/

Heap *SELECT_NEIGBOURS_HEURISTIC(Graph *graph, hnswNode *baseElement, sortedBuffer *candidates, int32 lc, int32 M, int8 FLAGS)
{
    Heap *resultHeap = graph->storage.resultHeap->size > 0 ? graph->storage.secondResultHeap : graph->storage.resultHeap; // maxheap
    Heap *discarded = graph->storage.discardedHeap;

    heap_reset(resultHeap);
    heap_reset(discarded);

    // switch worst with better
    // extendCandidates
    if (HAS_FLAG(FLAGS, EXTENDCANDIDATES))
    {

        for (int32 i = 0; i < candidates->size; i++)
        {
            hnswNode *node = getNodeById(graph, candidates->data[i].id);

            for (int32 j = 0; j < node->numNeigbours[lc]; j++)
            {
                hnswNode *nabour = getNodeById(graph, node->neigbours[(lc * graph->M_maxNeigbours) + j]);

                float32 dist = l2_sq_distance_neon_128v(&baseElement->v, &nabour->v);
                if (dist < heapPeek(candidates).dist && graph->visited.visited[nabour->id] != graph->visited.visited_mark)
                {
                    markNodeVisited(graph, nabour->id);

                    //  heap_insert(candidates, nabour->id, dist, NULL);
                }
            }
        }
    }

    for (int32 i = 0; i < candidates->size && resultHeap->size < M; i++)
    {
        heapItem currItem = candidates->data[i];
        hnswNode *current = getNodeById(graph, currItem.id);

        float32 distToBase = currItem.dist;

        bool ok = true;

        for (uint32 j = 0; j < resultHeap->size; j++)
        {
            hnswNode *r = getNodeById(graph, resultHeap->data[j].id);

            if (l2_sq_distance_neon_128v(&current->v, &r->v) <= distToBase)
            {
                ok = false;
                break;
            }
        }

        if (ok)
            heap_insert(resultHeap, current->id, distToBase, NULL);
        else if (HAS_FLAG(FLAGS, KEEP_P_CONN))
            heap_insert(discarded, current->id, distToBase, NULL);
    }

    // keep Pruned connections
    if (HAS_FLAG(FLAGS, KEEP_P_CONN))
    {
        while (discarded->size > 0 && resultHeap->size < M)
        {
            heapItem node = heapPop(discarded);
            heap_insert(resultHeap, node.id, node.dist, NULL);
        }
    }

    return resultHeap;
}

Heap *K_NN_SEARCH(Graph *g, vec q, int32 K, int32 efsearch)
{

    hnswNode *entryPoint = getNodeById(g, g->entrypointID);
    sortedBuffer *buffer;

    for (int32 layer = g->maxLayer - 1; layer > 0; layer--)
    {
        buffer = SEARCH_LAYER(g, entryPoint, q, 1, layer);

        entryPoint = getNodeById(g, buffer->data[0].id);
    }

    buffer = SEARCH_LAYER(g, entryPoint, q, efsearch, 0);

    Heap *nearestHeap = SELECT_NEIGBOURS_SIMPLE(g, buffer, K);

    return nearestHeap;
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
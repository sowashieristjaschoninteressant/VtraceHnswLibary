#include "vtrace_internal.h"
/**
 * @brief
 * level sampler to create a geometric distribution between our graph layer sparse -> dense
 * @param lMax
 * @param level_mult
 * @return int
 */
int32 VTlevelSample(uint32 lMax, float32 level_mult)
{

    float u = (float)rand() / ((float)RAND_MAX + 1.0f);

    if (u == 0.0f)
        u = 1e-10f;
    uint32 level = (int)(-logf(u) * level_mult);

    return level > lMax ? lMax : level;
}

/*
Algorithm 1
INSERT(hnsw, q, M, Mmax, efConstruction, mL)
Input: multilayer graph hnsw, new element q, number of established
connections M, maximum number of connections for each element
per layer Mmax, size of the dynamic candidate list efConstruction, nor-
malization factor for level generation mL
Output: update hnsw inserting element q
1 W ← ∅ // list for the currently found nearest elements
2 ep ← get enter point for hnsw
3 L ← level of ep // top layer for hnsw
4 l ← ⌊-ln(unif(0..1))∙mL⌋ // new element’s level
5 for lc ← L … l+1
6 W ← SEARCH-LAYER(q, ep, ef=1, lc)
7 ep ← get the nearest element from W to q
8 for lc ← min(L, l) … 0
9 W ← SEARCH-LAYER(q, ep, efConstruction, lc)
10 neighbors ← SELECT-NEIGHBORS(q, W, M, lc) // alg. 3 or alg. 4
11 add bidirectionall connectionts from neighbors to q at layer lc
12 for each e ∈ neighbors // shrink connections if needed
13 eConn ← neighbourhood(e) at layer lc
14 if │eConn│ > Mmax // shrink connections of e
// if lc = 0 then Mmax = Mmax0
15 eNewConn ← SELECT-NEIGHBORS(e, eConn, Mmax, lc)
// alg. 3 or alg. 4
16 set neighbourhood(e) at layer lc to eNewConn
17 ep ← W
18 if l > L
19 set enter point for hnsw to q
 */
void INSERT(Graph* graph,vec vec,uint32 M, uint32 Mmax, uint32 efConstruction, uint32 ml){
    
    hnswNode* ep,* newNode;
    int32 nodeLevel = VTlevelSample(MAX_LEVEL, ml);
    uint32 id = graph->count;
    
    if(graph->maxNodeCount < id){
       HNSW_LOG("graph is expanding!");
	expandgraph(graph);
    }


    newNode = &graph->nodes[id];

    makeNode(newNode, vec,id, nodeLevel, graph->M_maxNeigbours);

    #ifdef BILLION_SEARCH_HNSW
    // this is a hot loop so lets just write it like this. this will never happen basically unless you save over 4Billion nodes in this structure but i belive no consumer hardware has even the memory capacity for smth like that
    if(graph->count == UINT32_MAX){
        HNSW_LOG("node ID overflow!");
        abort();
    }
    #endif
    
    if(!graph->entrypoint){
        graph->entrypoint = newNode;
        graph->maxLayer = newNode->level;
        graph->count++;
        return;
    }

    ep = graph->entrypoint;
    for( int32 j = nodeLevel; j > graph->entrypoint->level; j-- ){
        Heap* W = SEARCH_LAYER(graph, vec, 1 ,j);
        ep = getNode( graph, heapPeek(W).id);
    }

    for(int32 layer = MIN(ep->level, nodeLevel); layer >= 0; layer--){
        Heap* resultHeap = SEARCH_LAYER(graph, vec, efConstruction, layer );
        
        Heap* selected = SELECT_NEIGBOURS_SIMPLE(resultHeap, M); // alg 3 for now might change that

        for(uint32 j = 0; j < selected->size; j++){

            hnswNode* node = getNode(graph,selected->data[j].id);
            
            if(graph->M_maxNeigbours > node->numNeigbours[layer] && graph->M_maxNeigbours > newNode->numNeigbours[layer]){
                node->neigbours[layer][node->numNeigbours[layer]++] = newNode->id;
                newNode->neigbours[layer][newNode->numNeigbours[layer]++] = node->id;
                
            }
            // TODO: okay i could implement here the algo to restructure the nodes
        }
    }

    graph->count++;

    if( newNode->level > graph->entrypoint->level){
        graph->entrypoint = newNode;
        graph->maxLayer = newNode->level;
    }
}

/*Algorithm 3 SELECT-NEIGHBORS-SIMPLE(q, C, M) 
Input: base element q,
 candidate elements C, number of neighbors to return M 
 Output: M nearest elements to q return M nearest elements from C to q*/
Heap *SELECT_NEIGBOURS_SIMPLE(Heap *c, uint32 M)
{
    Heap *m = MAX_HEAP(M);
    while (c->size > 0)
    {
        heapItem current = heapPop(c);
        if (m->size < M)
        {
            heap_insert(m, current.id, current.dist, NULL);
        }
        else if (current.dist < heapPeek(m).dist)
        {
            heapPop(m);
            heap_insert(m, current.id, current.dist, NULL);
        }
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
Heap *SEARCH_LAYER(Graph *graph, vec q,uint32 ef, uint32 lc)
{
    Heap *c = heap_init(ef, min_cmp); // candidate list
    Heap *w = heap_init(ef, max_cmp); // closest results

    incVisitedMark(graph);

    if (graph->visited.visited_mark == 0)
    {
        memset(graph->visited.visited, 0, sizeof(uint32) * graph->visited.size);
    }

    markNodeVisited(graph, graph->entrypoint->id);

    float32 epDistance = l2_sq_distance((vec*) &graph->entrypoint->v, &q);

    heap_insert(c, graph->entrypoint->id, epDistance, NULL);
    heap_insert(w, graph->entrypoint->id, epDistance, NULL);

    while (c->size > 0)
    {
        heapItem current = heapPop(c);
        heapItem furthestElementQ = heapPeek(w);

        if (w->size >= ef && current.dist > furthestElementQ.dist)
        {
            HNSW_LOG("all elements are evaluated in searchLayer");
            
            break;
        }

        node *currentNode = &graph->nodes[current.id];

        if(lc > currentNode->level) continue;

        uint32 nabourCount = currentNode->numNeigbours[lc];
        HNSW_ASSERT(currentNode->level >= lc);
        for (uint32 i = 0; i < nabourCount; i++)
        {                    
            node *neigbour = &graph->nodes[currentNode->neigbours[lc][i]];

            if (graph->visited.visited[neigbour->id] != graph->visited.visited_mark)
            {

                markNodeVisited(graph, neigbour->id);


                
                float32 dist = l2_sq_distance( &graph->nodes[neigbour->id].v, &q);

                if (dist < furthestElementQ.dist || w->size < ef)
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

    heap_dispose(c);

    return w;
}

#include "vtrace.h"

/**
 * @brief
 * level sampler to create a geometric distribution between our graph layer sparse -> dense
 * @param lMax
 * @param level_mult
 * @return int
 */
int VTlevelSample(uint32 lMax, float32 level_mult)
{

    float u = (float)rand() / ((float)RAND_MAX + 1.0f);

    if (u == 0.0f)
        u = 1e-10f;
    uint32 level = (int)(-logf(u) * level_mult);

    return level > lMax ? lMax : level;
}

/*Algorithm 3 SELECT-NEIGHBORS-SIMPLE(q, C, M) 
Input: base element q,
 candidate elements C, number of neighbors to return M 
 Output: M nearest elements to q return M nearest elements from C to q*/
Heap *SELECT_NEIGBOURS_SIMPLE(Heap *c, uint32 M)
{
    Heap *m = heap_init(M, max_cmp);
    while (c->size > 0)
    {
        heapItem *current = heapPop(c);
        if (m->size < M)
        {
            heap_insert(m, current->id, current->dist);
        }
        else if (current->dist < heapPeek(m)->dist)
        {
            heapPop(m);
            heap_insert(m, current->id, current->dist);
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
Heap *SEARCH_LAYER(Graph *graph, vec q, uint32 lc)
{
    Heap *c = heap_init(graph->efsearch, min_cmp); // candidate list
    Heap *w = heap_init(graph->efsearch, max_cmp); // closest results

    // TODO: after aproximatly 3 Billion searches this should overflow so i need to detect that and memset the visited list also if my nodes increase so should my visited List

    incVisitedMark(graph);

    if (graph->visited.visited_mark == 0)
    {
        memset(graph->visited.visited, 0, sizeof(uint32) * graph->visited.size);
    }

    markNodeVisited(graph, graph->entrypoint->id);

    float32 epDistance = l2_sq_distance((vec *)graph->entrypoint->data, &q);

    heap_insert(c, graph->entrypoint->id, epDistance);
    heap_insert(w, graph->entrypoint->id, epDistance);

    while (c->size > 0)
    {
        heapItem *current = heapPop(c);
        heapItem *furthestElementQ = heapPeek(w);

        if (w->size >= graph->efsearch && current->dist > furthestElementQ->dist)
        {
            HNSW_LOG("all elements are evaluated in searchLayer");
            // all elements are evaluated;;;;;
            break;
        }

        node *currentNode = &graph->nodes[current->id];

        for (uint32 i = 0; i < currentNode->numNeigbours[lc]; i++)
        {
            node *neigbour = &graph->nodes[currentNode->neigbours[lc][i]];

            if (graph->visited.visited[neigbour->id] != graph->visited.visited_mark)
            {

                markNodeVisited(graph, neigbour->id);

                float32 dist = l2_sq_distance((vec *)graph->nodes[neigbour->id].data, &q);

                if (dist < furthestElementQ->dist || w->size < graph->efsearch)
                {
                    heap_insert(c, neigbour->id, dist);
                    heap_insert(w, neigbour->id, dist);
                }

                if (w->size > graph->efsearch)
                {
                    heapPop(w);
                }
            }
        }
    }

    heap_dispose(c);

    return w;
}

#include "graph.h"

VT_graph *initializeGraph(uint32 maxLayer, uint32 efConstruction, uint32 efSearch, uint32 M_maxNeigbours)
{

    VT_graph *graph = malloc(sizeof(VT_graph));

    HNSW_ASSERT(graph);

    // seed random algorithm
    srand((int)time(NULL));
    if( M_maxNeigbours <= 0){
        graph->M_maxNeigbours = DEFAULT_MAX_NEIGBOURS;
    }else{
        graph->M_maxNeigbours = M_maxNeigbours;
    }
    graph->efconstruction = efConstruction;
    graph->efsearch = efSearch;
    graph->maxLayer = maxLayer;
    graph->nodes = NULL;
    graph->entrypoint = NULL;
    graph->visited = initvList(START_LAYER_NODES);
    graph->maxHeap = heap_init(graph->efconstruction, max_cmp);
    graph->minHeap = heap_init(graph->efconstruction, min_cmp);

    return graph;
}

visitedList initvList(uint32 size)
{
    visitedList list;

    list.visited_mark = 0;
    list.size = size;
    list.visited = calloc(0, sizeof(uint32) * size);
    HNSW_ASSERT(list.visited);
    return list;
}

void uninitializeGraph(VT_graph *graph)
{

    free(graph->nodes);
    free(graph->visited.visited);
    free(graph);
}


 void VTaddNeigbour(node* target, uint32 neighbourId, uint32 layer, uint32 M_MAXneigbours){
    if(M_MAXneigbours < target->numNeigbours[layer]){
        HNSW_LOG("VTaddNeigbour to many neighbours in layer...");
        exit(EXIT_FAILURE);
    }
    target->neigbours[layer][target->numNeigbours[layer]++] = neighbourId;
}

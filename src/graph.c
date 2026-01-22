
#include "graph.h"

VT_graph *initializeGraph(uint32 maxLayer, uint32 efConstruction, uint32 efSearch, uint32 M_maxNeigbours, uint32 maxNodeCount)
{

    VT_graph *graph = hnsw_alloc_mem(sizeof(VT_graph));

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
    graph->maxNodeCount = maxNodeCount;

    graph->nodes = hnsw_alloc_mem(sizeof(hnswNode) * graph->maxNodeCount);
    graph->count = 0;    

    graph->entrypoint = NULL;
    graph->visited = initvList(START_LAYER_NODES);
    graph->maxHeap = heap_init(graph->efconstruction, max_cmp);
    graph->minHeap = heap_init(graph->efconstruction, min_cmp);

    return graph;
}

void makeNode(node* node, vec v,uint32 id, uint32 nodeLevel, uint32 maxNeigbours){

    node->id = id;
    node->level = nodeLevel;
    node->numNeigbours = 0;
    node->v = v;
    node->neigbours = hnsw_alloc_mem(sizeof(uint32*) * nodeLevel);

    for(uint32 i = 0; i < nodeLevel; i++){
        node->neigbours[i] = hnsw_alloc_mem(sizeof(uint32) * maxNeigbours);
        for(uint32 j = 0; j < maxNeigbours; j++){
            // this will never overflow unless run on a supercomputer :0 with like 10000gb of ram xd
            node->neigbours[i][j] = UINT32_MAX; // mark empty

        }
    }

 

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

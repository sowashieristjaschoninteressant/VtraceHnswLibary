#include "vtrace.h"

VT_graph* initializeGraph(uint32 maxLayer, uint32 efConstruction, uint32 efSearch){
    
    VT_graph* graph = malloc(sizeof(VT_graph));

    HNSW_ASSERT(graph);
    
    // seed random algorithm
    srand((int) time(NULL));

    graph->efconstruction = efConstruction;
    graph->efsearch = efSearch;
    graph->maxLayer = maxLayer;
    graph->nodes = NULL;
    graph->entrypoint = NULL;
    
    return graph;
}

void uninitializeGraph(VT_graph* graph){

    free(graph->nodes);
    free(graph);
}

/**
 * @brief 
 * level sampler to create a geometric distribution between our graph layer sparse -> dense
 * @param lMax 
 * @param level_mult 
 * @return int 
 */
int VTlevelSample(uint32 lMax,float level_mult){
    
    float u = (float)rand() / ((float) RAND_MAX + 1.0f);

    if(u == 0.0f) u = 1e-10f;

    uint32 level = (int) (-logf(u) * level_mult);

    return level > lMax ? lMax : level;
}



node* SEARCH_LAYER(vec v, node* ep, uint32 ef, uint32 lc){

    Heap* c = heap_init(ef, max_cmp); // candidate list
    Heap* w = heap_init(ef, min_cmp); // closest results

    

    return NULL;
}
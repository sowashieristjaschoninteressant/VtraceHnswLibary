
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
    // because layer 1 will be saved in array slot 0 but when i pass 0 into the allocation func it will abort
    uint32 allocationLevel = nodeLevel + 1;
    node->id = id;
    node->level = nodeLevel;

    node->v.dim = v.dim;
    node->v.vec = hnsw_alloc_mem(sizeof(float32) * v.dim);
    memcpy( node->v.vec, v.vec, sizeof(float32) * v.dim);

  

    node->neigbours = hnsw_alloc_mem(sizeof(uint32*) * (allocationLevel));

    for(uint32 i = 0; i < allocationLevel; i++){
        node->neigbours[i] = hnsw_alloc_mem(sizeof(uint32) * maxNeigbours);
        for(uint32 j = 0; j < maxNeigbours; j++){
            // this will never overflow unless run on a supercomputer :0 with like 10000gb of ram xd
            node->neigbours[i][j] = UINT32_MAX; // mark empty

        }
    }

    node->numNeigbours = hnsw_alloc_mem(sizeof(uint32) * allocationLevel);

    memset(node->numNeigbours, 0, sizeof(uint32) * allocationLevel);

}

 void expandgraph(Graph* graph){
    uint32 newMaxNodeCount = graph->maxNodeCount * 2;

     hnswNode* nodes = realloc(graph->nodes, sizeof(hnswNode) * newMaxNodeCount);

     if(!nodes){
        HNSW_LOG("cannot allocate more nodes out of memory? expandGraph\n");
        abort();
     }

     graph->nodes = nodes;
     graph->maxNodeCount = newMaxNodeCount;

}

visitedList initvList(uint32 size)
{
    visitedList list;

     list.size = size;
   list.visited_mark = 0;
    list.visited = hnsw_alloc_mem(sizeof(uint32) * size);
    
    if(!list.visited){
        HNSW_LOG("error cannot allocate the visited List");
        abort();
    }

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

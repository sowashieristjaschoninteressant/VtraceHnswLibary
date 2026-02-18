
#include "graph.h"

VT_graph *initializeGraph(uint32 maxLayer, uint32 efConstruction, uint32 efSearch, uint32 M_maxNeigbours, uint32 maxNodeCount)
{

    VT_graph *graph = hnsw_alloc_mem(sizeof(VT_graph), alignof(VT_graph));

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

    graph->nodes = malloc(sizeof(hnswNode) * graph->maxNodeCount);
    if(!graph->nodes){
        HNSW_LOG("cannot allocate graph nodes out of mem?\n");
        abort();
    }
    graph->count = 0;    

    graph->entrypointID = -1;
    graph->visited = initvList(graph->maxNodeCount);

    initStorage(graph);
     
    return graph;
}

void initStorage(Graph* graph){
    
    graph->storage.candidateHeap  = MIN_HEAP(graph->efconstruction);
    graph->storage.closestResults = MAX_HEAP(graph->efconstruction);
    graph->storage.discardedHeap  = MIN_HEAP(graph->efconstruction);
    graph->storage.simpleHeap     = MAX_HEAP(graph->efconstruction);
    graph->storage.resultHeap     = MAX_HEAP(graph->efconstruction);
    graph->storage.buffer = hnsw_alloc_mem(sizeof(sortedBuffer), alignof(sortedBuffer));
    if(!graph->storage.buffer){
        HNSW_LOG("cannot allocate storage.buffer is there a prob?");
        abort();
    }

    graph->storage.buffer->data = NULL;
    graph->storage.buffer->size = 0;

}

void makeNode(node* node, vec v,uint32 id, uint32 nodeLevel, uint32 maxNeigbours){
    // because layer 1 will be saved in array slot 0 but when i pass 0 into the allocation func it will abort
    uint32 allocationLevel = nodeLevel + 1;
    node->id = id;
    node->level = nodeLevel;

    node->v.dim = v.dim;
    node->v.vec = hnsw_alloc_mem(sizeof(float32) * v.dim, alignof(float32));
    memcpy( node->v.vec, v.vec, sizeof(float32) * v.dim);

    node->neigbours = hnsw_alloc_mem(sizeof(uint32) * (maxNeigbours * allocationLevel), alignof(uint32));

    node->numNeigbours = hnsw_alloc_mem(sizeof(uint32) * allocationLevel, alignof(uint32));

    memset(node->numNeigbours, 0, sizeof(uint32) * allocationLevel);
}

 void expandgraph(Graph* graph){
    uint32 newMaxNodeCount = graph->maxNodeCount * 2;

     hnswNode* nodes = realloc(graph->nodes, sizeof(hnswNode) * newMaxNodeCount);

     // increase visited list also
     uint32* visitedptr = realloc(graph->visited.visited, sizeof(uint32) * newMaxNodeCount);
     
     if(!nodes || !visitedptr){
        HNSW_LOG("cannot allocate more nodes out of memory? expandGraph\n");
        abort();
     }

     graph->visited.visited = visitedptr;
     graph->visited.size = newMaxNodeCount;
     
     graph->nodes = nodes;
     graph->maxNodeCount = newMaxNodeCount;

}

visitedList initvList(uint32 size)
{
    visitedList list;

     list.size = size;
     list.visited_mark = 1;
     list.visited = malloc(sizeof(uint32) * size);
    
    if(!list.visited){
        HNSW_LOG("error cannot allocate the visited List");
        abort();
    }

    memset(list.visited, 0,sizeof(uint32) * size);

    return list;
}

void uninitializeGraph(VT_graph *graph)
{
    /*
    free(graph->nodes);
    free(graph->visited.visited);
    free(graph);
    */
    }


 inline void addNeigbour(node* target, uint32 neighbourId, uint32 layer, uint32 M_MAXneigbours){
    if(M_MAXneigbours <= target->numNeigbours[layer]){
        HNSW_LOG("VTaddNeigbour to many neighbours in layer...");
       return;
    }

    target->neigbours[layer * M_MAXneigbours + target->numNeigbours[layer]++] = neighbourId;
}

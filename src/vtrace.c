#include "vtrace.h"

int populateresultSet(Graph*g, hnswResult* resultSet, int32 size){

    resultSet->size = size;
    resultSet->ids = graph_alloc(g,sizeof(uint32) * size, alignof(uint32));
    resultSet->distances = graph_alloc(g,sizeof(float32) * size, alignof(float32));

    if(!resultSet->ids || !resultSet->distances){
        HNSW_LOG("cannot populate resultset! out of memory");
        resultSet->distances = NULL;
        resultSet->ids = NULL;
        resultSet->size = 0;
        return HNSW_ENOMEM;
    }
 

    return HNSW_OK;

}

HNSW* hnsw_init(uint32 ef){
    
    Graph* hnsw = initializeGraph(DEFAULT_MAX_LAYERS,ef, ef,DEFAULT_MAX_NEIGBOURS,DEFAULT_MAX_ELEMENTS);
    
    return hnsw;
}
void hnsw_insert(HNSW* graph, vec* vector, int32 M){
    float32 ml = DEFAULT_LEVEL_MAG(graph->M_maxNeigbours);

    INSERT(graph,*vector, M,graph->efconstruction,ml);

}


int hnsw_search(HNSW* graph, vec* query, int32 k, hnswResult* results){
   
    hnswContext* ctx = acquireContext(graph);
    Heap* out = ctx->outHeap;

   K_NN_SEARCH(ctx, *query,k,graph->efsearch, out);
  
   int32 heapSize = out->size;
   int32 outSize = heapSize < k ? heapSize : k; 
   
   if(populateresultSet(graph,results, outSize) != HNSW_OK) return HNSW_ERROR;
   
   
   for(int32 i = 0; i < outSize; i++){
        heapItem item = heapPop(out);
        results->ids[i] = item.id;
        results->distances[i] = item.dist;
   }
   
    releaseContext(graph, ctx);
    return HNSW_OK;
}

void hnsw_free(HNSW* graph){

    uninitializeGraph(graph);
}

int hnsw_linear(HNSW* graph, vec* query){
    
    return NN_SIMPLE_LINEAR(graph, *query);
}
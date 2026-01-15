#include "tests.h"
#include "stdio.h"
#include "string.h"


static inline float32 randf(){
    return (float)rand() / (float) RAND_MAX;
}

static inline float32 getRandFloat(float32 min, float32 max){
    return min + (max - min) * randf();
}

// generate randome float array
float32* generateRFA(uint32 size, float32 min, float32 max){

    float32* farray = malloc(sizeof(float32) * size);
    assert(farray);

    for(uint32 i = 0; i < size; i++){
        farray[i] = getRandFloat(min,max);
    }

    return farray;
}

// this graph will be just a simple one layer graph with fixed values in terms of M_MAXNEIGBOURS and MAXLAYER we just will have 4 nodes that will be connected with a max of 2 connections
Graph* mockGraphOneLayer(uint32 efSearch){
    const uint32 M_MAX_LAYER = 0;
    const uint32 EF_CONSTRUCTION = 40; // irrelevant
    const uint32 M_MAXNEIGBOURS = 2;
    const uint32 vectorCount = 4;
    const uint32 LAYERS = 1;
    Graph* graph = initializeGraph(M_MAX_LAYER,EF_CONSTRUCTION, efSearch,M_MAXNEIGBOURS);

    float32 vals0[] = {0.0f,0.0f};
    float32 vals1[] = {1.0f,0.0f};
    float32 vals2[] = {0.0f,1.0f};
    float32 vals3[] = {1.0f,1.0f};
    
    vec* testVectores= malloc(sizeof(vec) * vectorCount);
    HNSW_ASSERT(testVectores);

    testVectores[0] = make_vec(2, vals0);
    testVectores[1] = make_vec(2, vals1);
    testVectores[2] = make_vec(2, vals2);
    testVectores[3] = make_vec(2, vals3);

    node* nodes = malloc(sizeof(node) * 4 );
    HNSW_ASSERT(nodes);

    for(uint32 i = 0; i < 4; i++){
        
        nodes[i].data = (vec*) &testVectores[i];
         

        nodes[i].numNeigbours = malloc(sizeof(uint32) * LAYERS);
        HNSW_ASSERT(nodes[i].numNeigbours);
        nodes[i].numNeigbours[0] = 0;

        nodes[i].neigbours = malloc(sizeof(uint32*) * LAYERS);
        HNSW_ASSERT(nodes[i].neigbours);

        nodes[i].neigbours[0] = malloc(sizeof(uint32) * M_MAXNEIGBOURS);
         HNSW_ASSERT(nodes[i].neigbours[0]);
         nodes[i].id = i;
    }

   
    VTaddNeigbour(&nodes[0], 1, 0, M_MAXNEIGBOURS);
    VTaddNeigbour(&nodes[0], 2, 0, M_MAXNEIGBOURS);

    VTaddNeigbour(&nodes[1], 0, 0, M_MAXNEIGBOURS);
    VTaddNeigbour(&nodes[1], 3, 0, M_MAXNEIGBOURS);

    VTaddNeigbour(&nodes[2], 0, 0, M_MAXNEIGBOURS);
    VTaddNeigbour(&nodes[2], 3, 0, M_MAXNEIGBOURS);

    VTaddNeigbour(&nodes[3], 1, 0, M_MAXNEIGBOURS);
    VTaddNeigbour(&nodes[3], 2, 0, M_MAXNEIGBOURS);

    graph->nodes = nodes;
    graph->count = 4;
    graph->entrypoint = &nodes[0];



    return graph;
}


/*

===========================
HEAP TESTS
===========================

*/

void INSERT_POP_MINHEAP_TEST(){
    int32 fsize = 20000;
    Heap* heap = heap_init(fsize, min_cmp);
    float32* farray = generateRFA(fsize, 0.0f, 20000);

    for(int32 i = 0; i < fsize; i++){
        heap_insert(heap, i, farray[i]);
    }

    heapItem before =  heapPop(heap);
    
    for(int32 i = 1; i < fsize; i++){
       
        heapItem temp = heapPop(heap);
         
        
        HNSW_ASSERT(before.dist <= temp.dist);
        before = temp;
    }

    HNSW_LOG("[+] INSERT_POP_MINHEAP WORKS!");
    fflush(stdout);
    heap_dispose(heap);
    free(farray);

    return;
}

void INSERT_POP_MAXHEAP_TEST(){
    int32 fsize = 100;
    Heap* heap = heap_init(fsize , max_cmp);
    float32* farray = generateRFA(fsize,__FLT_MIN__ ,__FLT_MAX__);

    for(int32 i = 0; i < fsize; i++){

        heap_insert(heap,i, farray[i]);
    }

    heapItem before = heapPop(heap);
    
    for(int32 i = 1; i < fsize; i++){
       
        heapItem temp = heapPop(heap);
        
        
        assert( temp.dist <= before.dist );

        before = temp;
    }
    
    HNSW_LOG("INSERT_POP_MAXHEAP_WORKS!");
    fflush(stdout);

    heap_dispose(heap);
    free(farray);
    return;
}

void HEAP_TESTS(){
    HNSW_LOG("STARTING HEAP TESTS");
    INSERT_POP_MAXHEAP_TEST();
    INSERT_POP_MINHEAP_TEST();
    DETERMINISTIC_INSERTPOP_TEST();
    HNSW_LOG("HEAP TESTS SUCCESSFULLY FINISHED");
    return;
}

 void DETERMINISTIC_INSERTPOP_TEST(){
    const uint32 size = 10;
    Heap* heap = heap_init(size, max_cmp);
    float32 sValues[size] = {1,2,3,4,5,6,7,8,9,10};
    float32 fValues[size] = { 10, 9,8,7,6,5,4,3,2,1};

    for(uint32 i = 0; i < size; i++){
        heap_insert(heap,i ,sValues[i]);
    }
    uint32 j = 0;
    while(heap->size > 0 ){
        heapItem current = heapPop(heap);
         printf("dist: %f, id: %i\n", current.dist, current.id );

        HNSW_ASSERT(current.dist == fValues[j++]);
    }

    HNSW_LOG("DETERMINISTIC INSERTPOP WORKS!");

}

/*

===========================
Graph Tests
===========================

*/

void SELECT_NEAREST_NABOURS(){

     // Example distances for candidates
    float distances[] = {10.5, 2.3, 7.7, 4.4, 6.6, 1.1, 8.8};
    uint32 num_candidates = sizeof(distances) / sizeof(distances[0]);
    uint32 M = 3;  // Number of nearest neighbors to select

    // Step 1: Create a max-heap of candidates
    Heap *c = heap_init(num_candidates, max_cmp);
    for (uint32 i = 0; i < num_candidates; i++) {
        heap_insert(c, i, distances[i]);
    }

    printf("Candidates in max-heap order (root = largest distance):\n");
    for (uint32 i = 0; i < c->size; i++) {
        printf("id=%u dist=%.2f\n", c->data[i].id, c->data[i].dist);
    }

    // Step 2: Select M nearest neighbors
    Heap *m = SELECT_NEIGBOURS_SIMPLE(c, M);

    // Step 3: Pop from the resulting max-heap to get M closest
    printf("\nSelected %u nearest neighbors (max-heap root = farthest of the closest):\n", M);
    while (m->size > 0) {
        heapItem item = heapPop(m);
        printf("id=%u dist=%.2f\n", item.id, item.dist);
    }

    // Cleanup
    heap_dispose(c);
    heap_dispose(m);

}

void SIMPLE_SEARCH_LAYERTEST(){

    // prepare
    Graph* graph = mockGraphOneLayer(4);
    vec query = make_vec(2, (float[]) {0.1f, 0.1f});
    Heap* results = SEARCH_LAYER(graph, query, 0);

    const uint32 expectedResultIds[] = {3, 1 ,2 ,0};
    int resultIteration = 0;
    while(results->size > 0){
     
      heapItem temp =  heapPop(results);
      HNSW_ASSERT(temp.id == expectedResultIds[resultIteration++]);
      printf("these are the results id:  %i distance: %f\n", temp.id, temp.dist);
    }
    HNSW_LOG("SEARCH-LAYER WORKING");
    heap_dispose(results);
}

 void INIT_GRAPH_TEST(){
    const uint32 maxLayer = 10;
    const uint32 efSearch = 20;
    const uint32 efConstruction = 40;
    const uint32 M_maxNeigbours = 10;

    Graph* graph = initializeGraph(maxLayer, efConstruction, efSearch, M_maxNeigbours);

    HNSW_ASSERT(graph);
    HNSW_ASSERT(graph->efconstruction == efConstruction);
    HNSW_ASSERT(graph->efsearch == efSearch);
    HNSW_ASSERT(graph->M_maxNeigbours == M_maxNeigbours);
    HNSW_ASSERT(graph->maxLayer == maxLayer);

    HNSW_ASSERT(graph->maxHeap);
    HNSW_ASSERT(graph->minHeap);
    HNSW_ASSERT(graph->visited.visited);

    uninitializeGraph(graph);
    HNSW_LOG("GRAPH INITIALISATION WORKS SUCESSFULLY");
 }

 void GRAPH_TESTS(){
    HNSW_LOG("STARTING GRAPH TESTS");
    INIT_GRAPH_TEST();
    SIMPLE_SEARCH_LAYERTEST();

   SELECT_NEAREST_NABOURS();
   HNSW_LOG("GRAPH TESTS RUN SUCESSFULLY!");
}